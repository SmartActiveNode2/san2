
#include <string>
#include <string.h>

#include "cportmap.hpp"

namespace San2 { namespace Node {

CPortmap::CPortmap()
{
	
}

CPortmap::~CPortmap()
{
	// empty
}

bool CPortmap::registerPort(SAN_UINT32 port, San2::Utils::CProducerConsumer<std::shared_ptr<San2::Network::CCapsule> >& applicationQueue)
{
	
	return true;
}

bool CPortmap::unregisterPort(SAN_UINT32 port)
{
	
	return true;
}

void CPortmap::getPortQueue(SAN_UINT16 port, San2::Utils::CProducerConsumer<std::shared_ptr<San2::Network::CCapsule> >& queue)
{
	
}


}} // ns
