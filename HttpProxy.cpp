#include "HttpProxy.h"
#include "HttpStream.h"
#include "Utils.h"
#include <iostream>

HttpProxy::HttpProxy(const std::string& host, int port) :
  _port(-1),
  _remotehost(host),
  _remoteport(port)
{
}

HttpProxy::~HttpProxy()
{
  _server.close();
}

void HttpProxy::run(int port)
{
  _port = port;
  if (!_server.listen(port))
    throw std::runtime_error("Failed to start proxy server on port " + std::to_string(port));

  std::cout << "Listening on port " << port << " ..." << std::endl;

  while (_server.isValid())
  {
    Socket  client = _server.accept();

    if (client.isValid())
    {
      std::thread([this, client] {
        this->processClient(client);
      }).detach();
    }
  }
}

void HttpProxy::processClient(Socket client)
{
  Socket  server;

  if (!server.connect(_remotehost, _remoteport))
  {
    std::cerr << "Failed to connect to remote host" << std::endl;
    return;
  }

  HttpStream  req(client);
  HttpStream  res(server);

  while (
    this->processRequest(req, server) &&
    this->processResponse(res, client)
    );
  server.close();
  client.close();
}

bool HttpProxy::processRequest(HttpStream& req, Socket& server)
{
  if (!req.readHead())
    return false;

  req.setHeader("Host", _remotehost);
  req.stream(server);

  return true;
}

bool HttpProxy::processResponse(HttpStream& res, Socket& client)
{
  if (!res.readHead())
    return false;

  res.stream(client);

  return this->shouldKeepAlive(res);
}

bool HttpProxy::shouldKeepAlive(HttpStream& res) const
{
  const HttpStream::Header* connection = res.getHeader("Connection");

  return connection == nullptr // Keep Alive by default
    || utils::equalsCaseInsensitive(connection->value, "keep-alive");
}
