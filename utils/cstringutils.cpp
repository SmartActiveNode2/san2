#include "cstringutils.hpp"

#include <sstream>

namespace San2 { namespace Utils {

std::vector<std::string>& CStringUtils::split(std::vector<std::string>& result, const std::string& inputString, char delimiter)
{
  result.clear();
  std::istringstream ss(inputString);
  while (!ss.eof())
  {
    std::string field;
    std::getline(ss, field, delimiter );
    if (field.empty()) continue;
    result.push_back(field);
  }
  return result;
}

std::string CStringUtils::uintToString(unsigned int num)
{
    std::ostringstream oss;
    oss << num;
    return oss.str();
}

bool CStringUtils::stringToUint(const std::string &sNum, unsigned int &result)
{
    std::istringstream is(sNum);
    is >> result;
    return !is.fail();
}
 
}} // ns
