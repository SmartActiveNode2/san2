#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include "utils/platform/basictypes.hpp"

namespace San2 
{
	namespace Node
	{
		class CPortmap
		{
		public:
			CPortmap();
			virtual ~CPortmap();
			
			// Request connection Node------>Application
			// Allowed only one registration per connection (must be ensured inside the rpc interface function)
			bool registerApplicationProcess(std::string apiIpcAddress);
			
			// bindPort(SAN_UINT16 port, std::string capiAddress);
			// freePort(SAN_UINT16 port);
			// CapiClient& getApiByPort(SAN_UINT16 port);
		private:
			 // another msvc fix
			#ifdef LINUX
				CPortmap(const CPortmap& copyFromMe)=delete;
				CPortmap& operator=(const CPortmap& copyFromMe)=delete;
			#endif
			
			std::map<SAN_UINT16, std::string> mapPortAddress;
			std::map<std::string, std::shared_ptr<San2::Api::CApplicationConnector> > mapAddressConnector;

		};
	}
}
