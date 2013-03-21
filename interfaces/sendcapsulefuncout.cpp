
#include <algorithm>
#include <assert.h>
#include "sendcapsulefuncout.hpp"
#include "utils/cdatapack.hpp"
#include "network/ccapsule.hpp"
#include "network/nettypedef.hpp"
#include "utils/log.h"

namespace San2 { namespace Interfaces {

const int SendCapsuleFuncOut::timeoutMilisec = 3000;

SendCapsuleFuncOut::SendCapsuleFuncOut()
{
	
}

bool SendCapsuleFuncOut::setCapsuleToSend(std::shared_ptr<San2::Network::CCapsule> capsule)
{
	capsule->pack(m_SerializedCapsule);
	return true;
}

void SendCapsuleFuncOut::setCapsuleToSend(San2::Utils::bytes &packedCaplsule)
{
	m_SerializedCapsule = packedCaplsule;
}

unsigned int SendCapsuleFuncOut::getUniqueId()const
{
    // FuncIn must match FunOut ID
	return 1; // TODO: replace by global define file of func ids
}

bool SendCapsuleFuncOut::pack(San2::Utils::bytes &out)
{
	out = m_SerializedCapsule;
	return true;
}


}} // ns
