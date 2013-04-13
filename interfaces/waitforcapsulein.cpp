
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
	FILE_LOG(logDEBUG4) << "WaitForCapsuleIn::operator() invoked";
	
	std::shared_ptr<San2::Network::CCapsule> capsule;
	// pop timeout from queue
	int rval = m_queue->try_pop(&capsule, m_thr, m_requestedTimeout);
	
	FILE_LOG(logDEBUG4) << "WaitForCapsuleIn::operator() rval: " << rval;
	
	if (rval == -1)
	{
		printf("WaitForCapsuleIn::operator(): try_pop() fatal\n");
	}
	
	// set m_response
	SAN_INT32 returnValue;
	
	
	
	switch(rval)
	{
		case 0:
			returnValue = SAN2_WAITFORCAPSULE_SUCCESS;
			break;
		case -2:
			returnValue = SAN2_WAITFORCAPSULE_TIMEOUT;
			break;
		default:
			returnValue = SAN2_WAITFORCAPSULE_ERROR;
			break;
	}
	
	if (returnValue == SAN2_WAITFORCAPSULE_SUCCESS)
	{
		San2::Utils::bytes serializedCapsule;
		capsule->pack(serializedCapsule);
		m_response = San2::Utils::CDataPack::pack(returnValue) + serializedCapsule;
	}
	else
	{
		m_response = San2::Utils::CDataPack::pack(returnValue);
	}
	
	FILE_LOG(logDEBUG4) << "WaitForCapsuleIn::operator() success; returnValue: " << returnValue;
	return true;
}


bool WaitForCapsuleIn::unpack(const San2::Utils::bytes &in)
{	
	if (in.size() != 4) return false;
	m_requestedTimeout = San2::Utils::CDataPack::unpackUint32(in, 0);
	return true;
}

San2::Utils::bytes WaitForCapsuleIn::getResponse()
{
	return m_response;
}

}} // ns
