#include "Utils.h"

bool utils::equalsCharInsensitive(const char& c1, const char& c2)
{
  return std::tolower(c1) == std::tolower(c2);
}

bool utils::equalsCaseInsensitive(const std::string& str1, const std::string& str2)
{
  return
    str1.size() == str2.size() &&
    std::equal(str1.begin(), str1.end(), str2.begin(), &utils::equalsCharInsensitive);
}