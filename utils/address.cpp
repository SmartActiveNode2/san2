
#include "address.hpp"
#include "hex.hpp"

namespace San2
{
    namespace Utils
    {
        // ugly conversion
        bool string2address(const std::string & strAddress, San2::Network::SanAddress &sanAddress)
        {
	        San2::Utils::bytes b;
	        if (San2::Utils::hexToBytes(strAddress, b) != true) // string -> bytes
	        {
		        return false;
	        }
	
	        if (b.size() != San2::Network::sanAddressSize) return false;
	        std::copy(b.begin(), b.end(), sanAddress.begin()); // bytes -> SanAddress
	        return true;
        }

        std::string address2string(const San2::Network::SanAddress &sanAddress)
		{
            std::string result;
            for(auto& s: sanAddress) result += byteToString(s);
		    return result;
		}

        // ---- appid -- should me moved to other file

        // ugly conversion
        bool string2appid(const std::string & strAppId, San2::Network::SanApplicationId &sanAppId)
        {
	        San2::Utils::bytes b;
	        if (San2::Utils::hexToBytes(strAppId, b) != true) // string -> bytes
	        {
		        return false;
	        }
	
	        if (b.size() != San2::Network::sanHashSize) return false;
	        std::copy(b.begin(), b.end(), sanAppId.begin()); // bytes -> SanAddress
	        return true;
        }


        std::string appid2string(const San2::Network::SanApplicationId &sanAppId)
		{
            std::string result;
            for(auto& s: sanAppId) result += byteToString(s);
		    return result;
		}
    }
}