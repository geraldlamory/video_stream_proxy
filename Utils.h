#ifndef __UTILS_H__
#define __UTILS_H__

#include <string>

namespace utils
{
  bool equalsCharInsensitive(const char& c1, const char& c2);
  bool equalsCaseInsensitive(const std::string& str1, const std::string& str2);
}

#endif // __UTILS_H__
