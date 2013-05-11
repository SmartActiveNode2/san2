
#pragma once

#include "utils/cvector.hpp"
#include "utils/platform/basictypes.hpp"

// Stop and wait ARQ - Sender
class StopWait 
{
	public:
		StopWait(unsigned int repetitions, unsigned int timeout);		
		bool sendReliableMessage(const San2::Utils::bytes& request, San2::Utils::bytes& response);
		
	protected:
		virtual bool sendDatagram(const San2::Utils::bytes& data)=0;
		virtual bool awaitDatagram(San2::Utils::bytes& data, unsigned int maxWaitMsec)=0;
	
	private:
		unsigned int m_repetitions;
		unsigned int m_timeout;
		SAN_UINT8 m_expectedSeqNum;
};
