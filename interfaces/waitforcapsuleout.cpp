
#include <algorithm>
#include <assert.h>

#include "waitforcapsuleout.hpp"
#include "utils/cdatapack.hpp"
#include "network/ccapsule.hpp"
#include "network/nettypedef.hpp"
#include "utils/log.h"

namespace San2 { namespace Interfaces {

const int WaitForCapsuleOut::timeoutMilisec = 3000;

WaitForCapsuleOut::WaitForCapsuleOut():
	m_timeout(1000) // default timeout
{
	
}

unsigned int WaitForCapsuleOut::getUniqueId()const
{
    // FuncIn must match FunOut ID
	return 2215; // TODO: replace by global define file of func ids
}

bool WaitForCapsuleOut::pack(San2::Utils::bytes &out)
{
	out = San2::Utils::CDataPack::pack(m_timeout);
	return true;
}

/* First 4 bytes is errorcode */ 
bool WaitForCapsuleOut::parseResponse(const San2::Utils::bytes &in)
{
	if (m_response.size() < 4) return false;
	m_response = in;
	return true;
}

bool WaitForCapsuleOut::getCapsule(San2::Network::CCapsule &capsule)
{
	SAN_INT32 errcode = San2::Utils::CDataPack::unpackInt32(m_response, 0);
	// 3 == SUCCES (never use zero, zero is failure for unpackInt32)
	if (errcode != 3) return false;
	if (m_response.size() < 4) return false;
	m_response.erase(m_response.begin(), m_response.begin() + 4); // cut errorcode at the beginning
	return capsule.unpack(m_response);
}

void WaitForCapsuleOut::setTimeout(SAN_UINT32 timeout)
{
	m_timeout = timeout;
}			

}} // ns
