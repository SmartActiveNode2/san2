#pragma once

#include <string>
#include <vector>
#include <map>
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
		
			// bindPort(SAN_UINT16 port, std::string capiAddress);
			// freePort(SAN_UINT16 port);
			// CapiClient& getApiByPort(SAN_UINT16 port);
		private:
			 // another msvc fix
			#ifdef LINUX
				CPortmap(const CPortmap& copyFromMe)=delete;
				CPortmap& operator=(const CPortmap& copyFromMe)=delete;
			#endif
			
			std::map<SAN_UINT16, std::string> mapPort;
			// std::map<std::string, CapiClient&> mapObject;

		};
	}
}
