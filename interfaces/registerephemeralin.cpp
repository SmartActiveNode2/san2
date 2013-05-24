
#include <algorithm>
#include <assert.h>
#include "registerephemeralin.hpp"
#include "utils/cdatapack.hpp"
#include "network/ccapsule.hpp"
#include "network/nettypedef.hpp"
#include "utils/log.h"

namespace San2 { namespace Interfaces {

const int RegisterEphemeralIn::timeoutMilisec = 3000;

RegisterEphemeralIn::RegisterEphemeralIn(San2::Node::CPortmap& portmapper, San2::Utils::CProducerConsumer<std::shared_ptr<San2::Network::CCapsule> >& applicationQueue):
	m_portmapper(portmapper),
	m_applicationQueue(applicationQueue)
{
	
}

unsigned int RegisterEphemeralIn::getUniqueId() const
{
	return 31332; // TODO: replace by global define file of func ids
}

bool RegisterEphemeralIn::operator()(void)
{	
	unsigned short port = m_portmapper.getEphemeralPort(&m_applicationQueue);
	m_response = San2::Utils::CDataPack::pack(port);
	return true;
}

bool RegisterEphemeralIn::unpack(const San2::Utils::bytes &in)
{	
	return true;
}

San2::Utils::bytes RegisterEphemeralIn::getResponse()
{
	return m_response;
}

}} // ns
