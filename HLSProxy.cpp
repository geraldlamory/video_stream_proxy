#include "HLSProxy.h"
#include "HttpStream.h"
#include "Utils.h"

#include <iostream>

static thread_local std::chrono::steady_clock::time_point start_time;
static thread_local std::string targetUrl;

HLSProxy::HLSProxy(const std::string& host, int port) : HttpProxy(host, port)
{
}

HLSProxy::~HLSProxy()
{
}

bool HLSProxy::processRequest(HttpStream& req, Socket& server)
{
  start_time = std::chrono::steady_clock::now();

  if (HttpProxy::processRequest(req, server))
  {
    targetUrl = "http://" + _remotehost + req.getRequest().path;
    std::cout << "[IN]" << hlstypeToString(getHLSType(targetUrl)) << " " << targetUrl << std::endl;
    return true;
  }
  if (getHLSType(targetUrl) == HLSType::SEGMENT)
  {
    std::cout << "[TRACK SWITCH]" << std::endl;
  }
  return false;
}

bool HLSProxy::processResponse(HttpStream& res, Socket& client)
{  
  bool    keepAlive = false;
  HLSType type = getHLSType(targetUrl);

  if (type == HLSType::MANIFEST)
    keepAlive = this->processManifest(res, client);
  else
  {
    keepAlive = HttpProxy::processResponse(res, client);
  }

  auto current_time = std::chrono::steady_clock::now();

  std::cout << "[OUT]" << hlstypeToString(getHLSType(targetUrl)) << " " << targetUrl << " ("
    << std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time).count()
    << "ms)" << std::endl;

  return keepAlive;
}

bool HLSProxy::processManifest(HttpStream& res, Socket& client) const
{
  if (res.readHead() && res.readBody())
  {
    std::string body = res.getBuffer();

    const std::string absolutePrefix = "http://" + _remotehost + '/';
    size_t startline = 0;
    size_t endline = 0;
    size_t sep;
    std::string line;
    while ((endline = body.find('\n', startline)) != std::string::npos)
    {
      line = body.substr(startline, endline - startline);
      if (!line.empty() && line.at(0) != '#')
      {
        std::string url = line;

        // Transform to relative Url
        if (url.compare(0, 4, "http") == 0 && (sep = url.find_last_of('/')) != std::string::npos)
          url.erase(0, sep + 1);

        // If any change, replace in body
        if (url != line)
        {
          body.replace(startline, endline - startline, url);
          endline = body.find('\n', startline);
        }
      }
      startline = endline + 1;
    }

    res.setHeader("Content-Length", std::to_string(body.size()));

    std::string message = res.toString();
    message += body;

    client.send(message.c_str(), message.size());

    return this->shouldKeepAlive(res);
  }
  return false;
}

HLSProxy::HLSType HLSProxy::getHLSType(const std::string& filename)
{
  size_t p = filename.find_last_of('.');
  if (p != std::string::npos)
  {
    if (utils::equalsCaseInsensitive(filename.substr(p + 1), "m3u8"))
      return HLSType::MANIFEST;
    else if (utils::equalsCaseInsensitive(filename.substr(p + 1), "ts"))
      return HLSType::SEGMENT;
  }
  return HLSType::UNKNOWN;
}

const char* HLSProxy::hlstypeToString(HLSType type)
{
  switch (type)
  {
  case HLSType::MANIFEST:
    return "[MANIFEST]";
  case HLSType::SEGMENT:
    return "[SEGMENT]";
  default:
    return "";
  }
}