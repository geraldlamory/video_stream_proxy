#ifndef __UTILS_H__
#define __UTILS_H__

#include <string>

namespace utils
{
  bool equalsCharInsensitive(char c1, char c2);
  bool equalsCaseInsensitive(const std::string& str1, const std::string& str2);
}

#endif // __UTILS_H__
