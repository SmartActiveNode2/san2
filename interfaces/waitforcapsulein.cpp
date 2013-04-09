
#include <algorithm>
#include <assert.h>
#include "waitforcapsulein.hpp"
#include "utils/cdatapack.hpp"
#include "network/ccapsule.hpp"
#include "network/nettypedef.hpp"
#include "utils/log.h"

namespace San2 { namespace Interfaces {

const int WaitForCapsuleIn::timeoutMilisec = 3000;

WaitForCapsuleIn::WaitForCapsuleIn(San2::Utils::CProducerConsumer<std::shared_ptr<San2::Network::CCapsule> > *queue, San2::Utils::CThread* thr):
	m_queue(queue),
	m_thr(thr)
{
	
}

unsigned int WaitForCapsuleIn::getUniqueId() const
{
	return 2215; // TODO: replace by global define file of func ids
}

bool WaitForCapsuleIn::operator()(void)
{	
	// register m_port
	
	
	
	std::shared_ptr<San2::Network::CCapsule> capsule;
	// pop timeout from queue
	int rval = m_queue->try_pop(&capsule, m_thr, m_requestedTimeout);
	
	if (rval == -1)
	{
		printf("try_pop() fatal\n");
		return false;
	}
	
	// set m_response
	// 3 = OK
	// 4 = TIMEOUT
	
	
	
	SAN_INT32 returnValue;
	
	if (rval == 0) // SUCCESS
	{
		returnValue = 3;
	}
	
	if (rval == -2)
	{
		returnValue = 4; // Timeout
	}
	
	San2::Utils::bytes serializedCapsule;
	
	capsule->pack(serializedCapsule);
	
	m_response = San2::Utils::CDataPack::pack(returnValue) + serializedCapsule;
	
	return true;
}


bool WaitForCapsuleIn::unpack(const San2::Utils::bytes &in)
{	
	if (in.size() != 4) return false;
	m_requestedTimeout = San2::Utils::CDataPack::unpackInt32(in, 0);
	return true;
}

San2::Utils::bytes WaitForCapsuleIn::getResponse()
{
	return m_response;
}

}} // ns
