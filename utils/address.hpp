
#pragma once

#include "network/nettypedef.hpp"

namespace San2
{
    namespace Utils
    {
		bool exapndHexSanAddress(std::string& strAddress);
		bool getExpandedAddress(std::string strAddress, San2::Network::SanAddress &sanAddress);
		
        bool string2address(const std::string & strAddress, San2::Network::SanAddress &sanAddress);
        std::string address2string(const San2::Network::SanAddress &sanAddress);

        // TODO: should be moved from address.hpp file (naming does not make sense)
        bool string2appid(const std::string & strAppId, San2::Network::SanApplicationId &sanAppId);
        std::string appid2string(const San2::Network::SanApplicationId &sanAppId);
    }
}
