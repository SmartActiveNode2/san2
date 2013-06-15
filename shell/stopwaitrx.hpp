
#pragma once

#include <functional>

#include "utils/cvector.hpp"
#include "utils/platform/basictypes.hpp"
#include "utils/cthread.hpp"
#include "api/node/connector/cnodeconnector.hpp"

// Stop and wait ARQ - Sender
class StopWaitRx
{
	public:
		StopWaitRx(San2::Api::CNodeConnector &connector, const San2::Network::SanAddress& serverAddress, SAN_UINT16 serverPort, const San2::Network::SanAddress& clientAddress, SAN_UINT16 clientPort);
		virtual bool incommingCapsule(San2::Network::CCapsule &rxcapsule);	
	
	protected:
		virtual bool processDatagram(SAN_UINT64 sequenceNummber, const San2::Utils::bytes& request, San2::Utils::bytes& response)=0;
	
	private:	
		San2::Api::CNodeConnector &m_connector;
		
		const San2::Network::SanAddress m_serverAddress;
		SAN_UINT16 m_serverPort; // APPLICATION_PORT
		
		const San2::Network::SanAddress m_clientAddress;
		SAN_UINT16 m_clientPort;

		SAN_UINT64 m_expectedSeqNum;
		
		San2::Utils::bytes m_lastSerializedCapsule;
};

