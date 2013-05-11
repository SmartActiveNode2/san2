
#include <algorithm>
#include <assert.h>
#include "getaddressesout.hpp"
#include "utils/cdatapack.hpp"
#include "network/ccapsule.hpp"
#include "network/nettypedef.hpp"
#include "utils/log.h"
#include "utils/platform/basictypes.hpp"

namespace San2 { namespace Interfaces {

const int GetAddressesOut::timeoutMilisec = 3000;

GetAddressesOut::GetAddressesOut():
	m_result(-1)
{
	
}

unsigned int GetAddressesOut::getUniqueId()const
{
    // FuncIn must match FunOut ID
	return 5014; // TODO: replace by global define file of func ids
}

bool GetAddressesOut::pack(San2::Utils::bytes &out)
{
	out.resize(0);
	out.clear();
	return true;
}

bool GetAddressesOut::parseResponse(const San2::Utils::bytes &in)
{
	m_response = in;
	return true;
}

unsigned int GetAddressesOut::getAddresses(std::list<San2::Network::SanAddress> &adrs)
{	
	adrs.clear();
	if (m_response.size() % sizeof(San2::Network::SanAddress)) return -1;
	
	unsigned int items = m_response.size() / sizeof(San2::Network::SanAddress);
	
	if (items == 0) return 0;
	
	San2::Network::SanAddress address;
	
	San2::Utils::bytes::iterator iter = m_response.begin();
	
	
	for(unsigned int i = 0; i < items; i++)
	{
		std::copy(iter, iter + sizeof(San2::Network::SanAddress), &address[0]);
		iter += sizeof(San2::Network::SanAddress);
		adrs.push_back(address);
	}
	
	
	return items;
}
			

}} // ns
