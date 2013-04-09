
#include <algorithm>
#include <assert.h>
#include "registerout.hpp"
#include "utils/cdatapack.hpp"
#include "network/ccapsule.hpp"
#include "network/nettypedef.hpp"
#include "utils/log.h"

namespace San2 { namespace Interfaces {

const int RegisterOut::timeoutMilisec = 3000;

RegisterOut::RegisterOut():
	m_result(-1)
{
	
}

bool RegisterOut::setPort(SAN_UINT16 port)
{
	m_port = port;
	return true;
}

unsigned int RegisterOut::getUniqueId()const
{
    // FuncIn must match FunOut ID
	return 4896; // TODO: replace by global define file of func ids
}

bool RegisterOut::pack(San2::Utils::bytes &out)
{
	out.resize(2);
	out = San2::Utils::CDataPack::pack(m_port);
	return true;
}

bool RegisterOut::parseResponse(const San2::Utils::bytes &in)
{
	m_result = San2::Utils::CDataPack::unpackInt32(in, 0);
	return true;
}

bool RegisterOut::wasRegistered()
{
	return m_result == 3;
}
			

}} // ns
