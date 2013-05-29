
#include <algorithm>
#include <assert.h>
#include "getaddressesin.hpp"
#include "utils/cdatapack.hpp"
#include "network/ccapsule.hpp"
#include "network/nettypedef.hpp"
#include "utils/log.h"
#include "network/cnetinterface.hpp"
#include "node/cnode.hpp"

namespace San2 { namespace Interfaces {

const int GetAddressesIn::timeoutMilisec = 3000;

GetAddressesIn::GetAddressesIn(San2::Node::CNode &node):
	m_node(node)
{
	
}

unsigned int GetAddressesIn::getUniqueId() const
{
	return 5014; // TODO: replace by global define file of func ids
}

bool GetAddressesIn::operator()(void)
{	
	FILE_LOG(logDEBUG4) << "begin GetAddressesIn::operator()";
	m_response.clear();
	for (std::shared_ptr<San2::Network::CNetInterface> x: m_node.getInterfaces()) 
	{	
		m_response += San2::Utils::bytes::array2bytes((const unsigned char *)&((x->getInterfaceAddress())[0]), sizeof(San2::Network::SanAddress));
	}
	
	FILE_LOG(logDEBUG4) << "end GetAddressesIn::operator()";
	return true;
}

bool GetAddressesIn::unpack(const San2::Utils::bytes &in)
{	
	// nothing to unpack
	return true;
}

San2::Utils::bytes GetAddressesIn::getResponse()
{
	return m_response;
}

}} // ns
