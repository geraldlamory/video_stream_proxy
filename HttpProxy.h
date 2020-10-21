#ifndef __HTTP_PROXY_H__
#define __HTTP_PROXY_H__

#include <string>
#include <vector>
#include <thread>
#include "Socket.h"

class HttpStream;

class HttpProxy
{
public:

  // Create a new HTTP Proxy connected to the specified remote host/port
  HttpProxy(const std::string& host, int port);
  virtual ~HttpProxy();

  // Start and run the proxy server on localhost on the specified port
  void run(int port);

  HttpProxy(const HttpProxy&) = delete;
  HttpProxy& operator=(const HttpProxy&) = delete;

protected:

  // Called in a new thread when a client connects
  void processClient(Socket client);

  // Read an incoming request and send it to the remote server
  virtual bool processRequest(HttpStream& req, Socket& server);

  // Read the response and stream it to the client
  virtual bool processResponse(HttpStream& res, Socket& client);

  // Returns true if the connection should be preserved
  virtual bool shouldKeepAlive(HttpStream& res) const;

  int     _port;
  Socket  _server;

  const std::string _remotehost;
  const int         _remoteport;

};

#endif // __HTTP_PROXY_H__
