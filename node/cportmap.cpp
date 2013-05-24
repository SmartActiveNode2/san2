
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
	FILE_LOG(logDEBUG4) << "CPortmap::registerPort():: attempting to register port " << port << "\n";
	std::pair<std::map<unsigned short int, San2::Utils::CProducerConsumer<std::shared_ptr<San2::Network::CCapsule> >* >::iterator, bool> pair = mapPorts.insert(std::pair<unsigned short int, San2::Utils::CProducerConsumer<std::shared_ptr<San2::Network::CCapsule> >*>(port, applicationQueue));
	
	if (pair.second) FILE_LOG(logDEBUG4) << "CPortmap::registerPort():: port REG OK \n";
	else FILE_LOG(logDEBUG4) << "CPortmap::registerPort():: port REG FAILED \n";
	
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

void CPortmap::freePorts(San2::Utils::CProducerConsumer<std::shared_ptr<San2::Network::CCapsule> > *applicationQueue)
{
	FILE_LOG(logDEBUG4) << "CPortmap::freePorts():: freeing ports \n";
	// thanks to http://stackoverflow.com/questions/4600567/c-deleting-elements-with-iterator
	
	std::function<bool (std::pair<unsigned short int, San2::Utils::CProducerConsumer<std::shared_ptr<San2::Network::CCapsule> >*>)> shouldDelete = [applicationQueue] (std::pair<unsigned short int, San2::Utils::CProducerConsumer<std::shared_ptr<San2::Network::CCapsule> >*> pair) -> bool
	{
		return pair.second == applicationQueue;
	};
	
	std::map<unsigned short int, San2::Utils::CProducerConsumer<std::shared_ptr<San2::Network::CCapsule> >*>::iterator itr = mapPorts.begin();
	while (itr != mapPorts.end()) 
	{
		if (shouldDelete(*itr)) 
		{
		   std::map<unsigned short int, San2::Utils::CProducerConsumer<std::shared_ptr<San2::Network::CCapsule> >*>::iterator toErase = itr;
		   ++itr;
		   mapPorts.erase(toErase);
		   FILE_LOG(logDEBUG4) << "CPortmap::freePorts():: port has been freed OK\n";
		} 
		else 
		{
		   ++itr;
		}
	}	
}

unsigned short int CPortmap::getEphemeralPort(San2::Utils::CProducerConsumer<std::shared_ptr<San2::Network::CCapsule> > *applicationQueue)
{	
	unsigned short port;
	bool registered = false;
	
	for (port = m_ephemeralPort; port < 65535; port++)
	{
		registered = registerPort(port, applicationQueue);
		if (registered) break;
	}
		
	if (registered)
	{
		if (port + 1 > 65535) m_ephemeralPort = SAN2_BEGIN_EPHEMERAL_PORT;
		else m_ephemeralPort = port + 1;
		
		return port;
	}
	
	for (port = SAN2_BEGIN_EPHEMERAL_PORT; port < m_ephemeralPort; port++)
	{
		registered = registerPort(port, applicationQueue);
		if (registered) break;
	}
	
	if (registered)
	{
		if (port + 1 > 65535) m_ephemeralPort = SAN2_BEGIN_EPHEMERAL_PORT;
		else m_ephemeralPort = port + 1;
		
		return port;
	}
	
	return 0;
}

}} // ns
