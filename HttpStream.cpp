#include "Socket.h"
#include "Utils.h"
#include "HttpStream.h"

#include <cstring>
#include <sstream>

HttpStream::HttpStream(const Socket& socket, size_t chunk_size) :
  _socket(socket),
  _chunk_size(chunk_size),
  _capacity(chunk_size),
  _position(0),
  _content_start(0)
{
  _data = static_cast<char*>(malloc(_capacity));
}

HttpStream::~HttpStream()
{
  free(_data);
}

bool HttpStream::readHead()
{
  int   len;
  char* header_end;

  _position = 0;
  _content_start = 0;
  _headers.clear();

  while (true)
  {
    len = this->consume();

    if (len <= 0)
    {
      _socket.close();
      return false;
    }

    header_end = strstr(_data, "\r\n\r\n");

    if (header_end != nullptr)
    {
      size_t  header_len = (header_end - _data) + 4;

      this->parseHeaders(std::string(_data, header_len));

      _content_start = header_len;

      return true;
    }
  }
  return true;
}

bool HttpStream::readBody()
{
  size_t contentLength = this->getContentLength();

  if (contentLength > 0)
  {
    size_t readLength = (_position - _content_start);
    int len;

    while (readLength < contentLength)
    {
      len = this->consume();
      if (len <= 0)
        return false;
      readLength += len;
    }
  }
  return true;
}

bool HttpStream::stream(Socket& target)
{
  std::string head = this->toString();

  target.send(head.c_str(), head.size());

  size_t contentLength = this->getContentLength();

  if (contentLength > 0)
  {
    size_t readLength = (_position - _content_start);
    int len;

    while (readLength < contentLength)
    {
      len = this->consume();
      if (len <= 0)
        return false;

      readLength += len;

      if (this->provide(target) <= 0)
        return false;
    }
    if (_position > _content_start)
    {
      if (this->provide(target) <= 0)
        return false;
    }
  }
  return true;
}

int HttpStream::consume()
{
  this->ensureBufferSize(_position + _chunk_size + 1);

  int len = _socket.read(_data + _position, _chunk_size);
  if (len > 0)
    _position += len;
  _data[_position] = '\0';
  return len;
}

int HttpStream::provide(const Socket& target)
{
  int len;
  int total_sent = 0;
  int to_send = _position - _content_start;
  int position = _content_start;

  while (total_sent < to_send)
  {
    len = target.send(_data + position, (_position - position));
    if (len <= 0)
      return len;
    position += len;
    total_sent += len;
  }
  _position = 0;
  _content_start = 0;
  return total_sent;
}

void HttpStream::ensureBufferSize(size_t size)
{
  if (size > _capacity)
  {
    _capacity = size;
    void* ptr = realloc(_data, _capacity);
    if (ptr == NULL)
      throw std::bad_alloc();
    _data = static_cast<char*>(ptr);
  }
}

void HttpStream::setHeader(const std::string& name, const std::string& value)
{
  for (auto it = _headers.begin(); it != _headers.end(); ++it)
  {
    if (utils::equalsCaseInsensitive(it->name, name))
    {
      it->value = value;
      return;
    }
  }

  _headers.push_back({ name, value });
}

const HttpStream::Header* HttpStream::getHeader(const std::string& name) const
{
  for (const Header& header : _headers)
  {
    if (utils::equalsCaseInsensitive(header.name, name))
      return &header;
  }
  return nullptr;
}

size_t HttpStream::getContentLength() const
{
  const Header* contentLength = this->getHeader("Content-Length");

  if (contentLength != nullptr)
    return std::stol(contentLength->value);
  return 0;
}

std::string HttpStream::toString() const
{
  std::stringstream ss;

  ss << _request.method << ' ' << _request.path << ' ' << _request.http_version << "\r\n";
  for (const Header& header : _headers)
  {
    ss << header.name << ": " << header.value << "\r\n";
  }
  ss << "\r\n";
  return ss.str();
}

void HttpStream::parseHeaders(const std::string& str)
{
  // Request
  size_t sep = str.find(' ');
  size_t path_sep = str.find(' ', sep + 1);
  size_t endline = str.find("\r\n", path_sep + 1);

  if (sep != std::string::npos && path_sep != std::string::npos && endline != std::string::npos)
  {
    _request.method = str.substr(0, sep);
    _request.path = str.substr(sep + 1, path_sep - sep - 1);
    _request.http_version = str.substr(path_sep + 1, endline - path_sep - 1);
  }

  // Headers
  size_t start = endline + 2;
  sep = str.find(": ", start);
  endline = str.find("\r\n", sep + 1);

  while (sep != std::string::npos)
  {
    _headers.push_back({str.substr(start, sep - start), str.substr(sep + 2, endline - sep - 2)});
    start = endline + 2;
    sep = str.find(':', start);
    endline = str.find("\r\n", sep);
  }
}
