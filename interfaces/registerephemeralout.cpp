
#include <algorithm>
#include <assert.h>
#include "registerephemeralout.hpp"
#include "utils/cdatapack.hpp"
#include "network/ccapsule.hpp"
#include "network/nettypedef.hpp"
#include "utils/log.h"

namespace San2 { namespace Interfaces {

const int RegisterEphemeralOut::timeoutMilisec = 3000;

RegisterEphemeralOut::RegisterEphemeralOut():
	m_port(0)
{
	
}

unsigned int RegisterEphemeralOut::getUniqueId()const
{
    // FuncIn must match FunOut ID
	return 31332; // TODO: replace by global define file of func ids
}

bool RegisterEphemeralOut::pack(San2::Utils::bytes &out)
{
	out.clear();
	return true;
}

bool RegisterEphemeralOut::parseResponse(const San2::Utils::bytes &in)
{
	if (in.size() != 2) return false;
	m_port = San2::Utils::CDataPack::unpackUint16(in, 0);
	return true;
}			

unsigned short int RegisterEphemeralOut::getPort()
{
	return m_port;
}


}} // ns
