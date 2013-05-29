
#pragma once

#include <functional>

#include "utils/cvector.hpp"
#include "utils/platform/basictypes.hpp"
#include "utils/cthread.hpp"

// Stop and wait ARQ - Sender
class StopWaitRx
{
	public:
		StopWaitRx();		
			
	protected:
		virtual bool incommingDatagram(const San2::Utils::bytes& request, San2::Utils::bytes& response);
		virtual bool processDatagram(SAN_UINT64 sequenceNummber, const San2::Utils::bytes& request, San2::Utils::bytes& response)=0;
	
	private:
		SAN_UINT64 m_expectedSeqNum;
		San2::Utils::bytes lastDatagram;
};

