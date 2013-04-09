#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include "utils/platform/basictypes.hpp"
#include "api/node/service/cnodeservicechannel.hpp"

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
			bool registerApplicationProcess(San2::Api::CNodeServiceChannel& channel);
			bool unregisterApplicationProcess(San2::Api::CNodeServiceChannel& channel);
			
			// bindPort(SAN_UINT16 port, std::string capiAddress);
			// freePort(SAN_UINT16 port);
			San2::Api::CNodeServiceChannel& getChannelPort(SAN_UINT16 port);
		private:
			 // another msvc fix
			#ifdef LINUX
				CPortmap(const CPortmap& copyFromMe)=delete;
				CPortmap& operator=(const CPortmap& copyFromMe)=delete;
			#endif
			
			//std::map<std::uint16_t port, San2::Api::CNodeServiceChannel> mapPorts;

		};
	}
}
