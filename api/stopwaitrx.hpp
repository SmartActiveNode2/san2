
#pragma once

#include <functional>

#include "utils/cvector.hpp"
#include "utils/platform/basictypes.hpp"
#include "utils/cthread.hpp"

// Stop and wait ARQ - Sender
class StopWaitRx : public San2::Utils::CThread
{
	public:
		StopWaitRx(unsigned int repetitions, unsigned int rxtimeout, std::function<bool(const San2::Utils::bytes&, San2::Utils::bytes&)> getAnswer);		
		void run(); // blocks
			
	protected:
		virtual bool sendDatagram(const San2::Utils::bytes& data)=0;
		virtual bool awaitDatagram(San2::Utils::bytes& data, unsigned int maxWaitMsec)=0;
	
	private:
		unsigned int m_repetitions;
		unsigned int m_rxtimeout;
		std::function<bool(const San2::Utils::bytes&, San2::Utils::bytes&)> m_getAnswer;
		
		SAN_UINT8 m_expectedSeqNum;
		
		San2::Utils::bytes lastDatagram;
};

