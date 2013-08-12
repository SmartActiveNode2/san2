
#pragma once

#include <sstream>
#include <string>
#include <vector>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

namespace San2
{
    namespace Utils
    {
        class CStringUtils
        {
        public:
            static std::vector<std::string>& split(std::vector<std::string>& result, const std::string& inputString, char delimiter);
            
            // true = SUCCESS
            static std::string uintToString(unsigned int num);
            static bool stringToUint(const std::string &sNum, unsigned int &result);

			static std::string ulongToString(unsigned long num);

			// trim from start
			static inline std::string &ltrim(std::string &s) 
			{
				s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
				return s;
			}

			// trim from end
			static inline std::string &rtrim(std::string &s) 
			{
				s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
				return s;
			}

			// trim from both ends
			static inline std::string &trim(std::string &s) 
			{
				return ltrim(rtrim(s));
			}

        protected:


        private:
        
      };
   }
}
