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
			
			bool registerPort(unsigned short int port, San2::Utils::CProducerConsumer<std::shared_ptr<San2::Network::CCapsule> > *applicationQueue);
			bool unregisterPort(unsigned short int port);
			San2::Utils::CProducerConsumer<std::shared_ptr<San2::Network::CCapsule> >* getPortQueue(unsigned short int port);
			
		private:
			 // another msvc fix
			#ifdef LINUX
				CPortmap(const CPortmap& copyFromMe)=delete;
				CPortmap& operator=(const CPortmap& copyFromMe)=delete;
			#endif
			
			std::map<unsigned short int, San2::Utils::CProducerConsumer<std::shared_ptr<San2::Network::CCapsule> >*> mapPorts;

		};
	}
}
