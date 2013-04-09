
#include <string>
#include <string.h>
#include <utility>
#include <iterator>

#include "cportmap.hpp"

namespace San2 { namespace Node {

CPortmap::CPortmap()
{
	
} 

CPortmap::~CPortmap()
{
	// empty
}

bool CPortmap::registerPort(unsigned short int port, San2::Utils::CProducerConsumer<std::shared_ptr<San2::Network::CCapsule> >* applicationQueue)
{
	std::pair<std::map<unsigned short int, San2::Utils::CProducerConsumer<std::shared_ptr<San2::Network::CCapsule> >* >::iterator, bool> pair = mapPorts.insert(std::pair<unsigned short int, San2::Utils::CProducerConsumer<std::shared_ptr<San2::Network::CCapsule> >*>(port, applicationQueue));
	return pair.second;
}

bool CPortmap::unregisterPort(unsigned short int port)
{
	return mapPorts.erase(port);
}

San2::Utils::CProducerConsumer<std::shared_ptr<San2::Network::CCapsule> >* CPortmap::getPortQueue(unsigned short int port)
{
	std::map<unsigned short int, San2::Utils::CProducerConsumer<std::shared_ptr<San2::Network::CCapsule> >* >::iterator iter;
	
	iter = mapPorts.find(port);
	if (iter == mapPorts.end()) return NULL;
	return iter->second;
}


}} // ns
