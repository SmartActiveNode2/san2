#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include "utils/platform/basictypes.hpp"
#include "utils/cproducerconsumer.hpp"
#include "network/ccapsule.hpp"

namespace San2 
{
	namespace Node
	{
		class CPortmap
		{
		public:
			CPortmap();
			virtual ~CPortmap();
			
			bool registerPort(SAN_UINT32 port, San2::Utils::CProducerConsumer<std::shared_ptr<San2::Network::CCapsule> >& applicationQueue);
			bool unregisterPort(SAN_UINT32 port);
			void getPortQueue(SAN_UINT16 port, San2::Utils::CProducerConsumer<std::shared_ptr<San2::Network::CCapsule> >& queue);
			
		private:
			 // another msvc fix
			#ifdef LINUX
				CPortmap(const CPortmap& copyFromMe)=delete;
				CPortmap& operator=(const CPortmap& copyFromMe)=delete;
			#endif
			
			std::map<unsigned short int, San2::Utils::CProducerConsumer<std::shared_ptr<San2::Network::CCapsule> >&> mapPorts;

		};
	}
}
