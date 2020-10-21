#ifndef __HTTP_REQUEST_H__
#define __HTTP_REQUEST_H__

#include <vector>

class Socket;

class HttpStream
{
public:

  struct Request
  {
    std::string method;
    std::string path;
    std::string http_version;
  };

  struct Header
  {
    std::string name;
    std::string value;
  };
  typedef std::vector<Header> HeaderList;

  HttpStream(const Socket& socket, size_t chunk_size = 4096);
  virtual ~HttpStream();

  // Start the request by reading at least the headers
  bool readHead();

  // Finish reading all the body (after a first call to readHead)
  bool readBody();

  // Stream all the request content to the specified socket
  bool stream(Socket& target);

  void setHeader(const std::string& name, const std::string& value);
  const Header* getHeader(const std::string& name) const;
  const HeaderList& getHeaders() const { return _headers; }

  const char* getBuffer() const { return _data + _content_start; };

  const Request& getRequest() const { return _request; }

  std::string toString() const;

  size_t getContentLength() const;

private:

  int consume();
  int provide(const Socket& target);

  void ensureBufferSize(size_t size);
  void parseHeaders(const std::string& str);

  Socket          _socket;
  Request         _request;
  HeaderList      _headers;

  const size_t    _chunk_size;
  char*           _data;
  size_t          _capacity;
  size_t          _position;
  size_t          _content_start;

};

#endif // __HTTP_REQUEST_H__
