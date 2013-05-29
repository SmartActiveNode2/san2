#pragma once

#include <memory>
#include <utils/cvector.hpp>

#include "stopwaitrx.hpp"

#define STATE_INITIAL 0
#define STATE_SRP1_CASB 1
#define STATE_SRP2_VERIFIED 2

// irrecoverable state
#define STATE_FAULT -1

class Session : public StopWaitRx
{
public:
	Session();
	
	bool processDatagram(const San2::Utils::bytes& request, San2::Utils::bytes& response);
	int getState();
protected:

private:
	unsigned int m_secondsValid;
	int m_state;
	San2::Utils::bytes m_srpUsername;
	San2::Utils::bytes m_srpA;
	
	
	// time lastActive
	// source addres, source port

};
