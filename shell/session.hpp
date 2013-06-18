#pragma once

#include <memory>
#include <utils/cvector.hpp>

#include "network/nettypedef.hpp"
#include "stopwaitrx.hpp"
#include "api/node/connector/cnodeconnector.hpp"

#define STATE_INITIAL 0
#define STATE_SRP1_CASB 1
#define STATE_SRP2_VERIFIED 2

// irrecoverable state
#define STATE_FAULT -1

class Session : public StopWaitRx
{
public:
	Session(San2::Api::CNodeConnector &connector, const San2::Network::SanAddress& serverAddress, SAN_UINT16 serverPort, const San2::Network::SanAddress& clientAddress, SAN_UINT16 clientPort);
	
	bool processDatagram(SAN_UINT64 sequenceNumber, const San2::Utils::bytes& request, San2::Utils::bytes& response);
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