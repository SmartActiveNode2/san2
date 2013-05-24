#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include "utils/platform/basictypes.hpp"
#include "utils/cproducerconsumer.hpp"
#include "network/ccapsule.hpp"
#include "utils/log.h"

#define SAN2_BEGIN_EPHEMERAL_PORT 40000

namespace San2 
{
	namespace Node
	{
		class CPortmap
		{
		public:
			CPortmap();
			virtual ~CPortmap();
			
			bool registerPort(unsigned short int port, San2::Utils::CProducerConsumer<std::shared_ptr<San2::Network::CCapsule> > *applicationQueue);
			unsigned short int getEphemeralPort(San2::Utils::CProducerConsumer<std::shared_ptr<San2::Network::CCapsule> > *applicationQueue);
			
			bool unregisterPort(unsigned short int port);
			
			
			void freePorts(San2::Utils::CProducerConsumer<std::shared_ptr<San2::Network::CCapsule> > *applicationQueue);
			
			San2::Utils::CProducerConsumer<std::shared_ptr<San2::Network::CCapsule> >* getPortQueue(unsigned short int port);
			
		private:
			 // another msvc fix
			#ifdef LINUX
				CPortmap(const CPortmap& copyFromMe)=delete;
				CPortmap& operator=(const CPortmap& copyFromMe)=delete;
			#endif
			
			std::map<unsigned short int, San2::Utils::CProducerConsumer<std::shared_ptr<San2::Network::CCapsule> >*> mapPorts;
			
			unsigned short m_ephemeralPort = SAN2_BEGIN_EPHEMERAL_PORT;
		};
	}
}
