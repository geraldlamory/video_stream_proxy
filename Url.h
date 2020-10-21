#ifndef __URL_H__
#define __URL_H__

#include <string>

/*
* Utility class to parse an URL 
*/
class Url
{
public:

  Url(const std::string& str);
  virtual ~Url();

  const std::string& toString() const { return _str; };

  const std::string& getHost() const { return _host; }
  const std::string& getPath() const { return _path; }
  const std::string& getFileName() const { return _filename; }
  int getPort() const { return _port;  }

private:

  std::string _str;
  std::string _host;
  std::string _path;
  std::string _filename;
  int         _port;
};

#endif // __URL_H__
