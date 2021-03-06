
#include <algorithm>
#include <assert.h>
#include "registerin.hpp"
#include "utils/cdatapack.hpp"
#include "network/ccapsule.hpp"
#include "network/nettypedef.hpp"
#include "utils/log.h"

namespace San2 { namespace Interfaces {

const int RegisterIn::timeoutMilisec = 3000;

RegisterIn::RegisterIn(San2::Node::CPortmap& portmapper, San2::Utils::CProducerConsumer<std::shared_ptr<San2::Network::CCapsule> >& applicationQueue):
	m_portmapper(portmapper),
	m_applicationQueue(applicationQueue)
{
	
}

unsigned int RegisterIn::getUniqueId() const
{
	return 4896; // TODO: replace by global define file of func ids
}

bool RegisterIn::operator()(void)
{	
	// register m_port
	bool registered = m_portmapper.registerPort(m_port, &m_applicationQueue);
	
	SAN_INT32 returnValue;
	
	if (registered) returnValue = 3; // SUCCESS
	else if (registered) returnValue = 2; // ALREADY TAKEN
	
	// set m_response
	m_response = San2::Utils::CDataPack::pack(returnValue);
	
	return true;
}

bool RegisterIn::unpack(const San2::Utils::bytes &in)
{	
	m_port = San2::Utils::CDataPack::unpackUint16(in, 0);
	return true;
}

San2::Utils::bytes RegisterIn::getResponse()
{
	return m_response;
}

}} // ns
