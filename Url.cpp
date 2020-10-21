#include "Url.h"

Url::Url(const std::string& str) : _str(str)
{
  std::string url = str;

  if (url.substr(0, 7) == "http://")
  {
    _port = 80;
    url.erase(0, 7);
  }

  if (url.substr(0, 8) == "https://")
  {
    _port = 443;
    url.erase(0, 8);
  }

  size_t n = url.find('/');
  if (n != std::string::npos)
  {
    _host = url.substr(0, n);
    _path = url.substr(n);
    n = _path.rfind('/');
    _filename = _path.substr(n + 1);
  }
  else
  {
    _host = url;
    _path = "/";
  }
}

Url::~Url()
{
}
