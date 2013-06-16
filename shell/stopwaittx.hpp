
#pragma once

#include "utils/cvector.hpp"
#include "utils/platform/basictypes.hpp"
#include "api/node/connector/cnodeconnector.hpp"

class StopWaitTx
{
public:
	StopWaitTx(unsigned int repetitions, unsigned int timeout, San2::Api::CNodeConnector &connector, San2::Network::SanAddress dstAddress, SAN_UINT16 dstPort, San2::Network::SanAddress srcAddress, SAN_UINT16 srcPort);
	bool sendReliableMessage(const San2::Utils::bytes& request, San2::Utils::bytes& response);
	
	San2::Network::SanAddress getServerAddress();
	SAN_UINT16 getServerPort();
	San2::Network::SanAddress getClientAddress();
	SAN_UINT16 getClientPort();
private:
	//bool sendDatagram(San2::Utils::bytes& data);
	bool awaitDatagram(San2::Utils::bytes& data, unsigned int maxWaitMsec);

	unsigned int m_repetitions;
	unsigned int m_timeout;
	San2::Api::CNodeConnector &m_connector;
	San2::Network::SanAddress m_serverAddress;
	SAN_UINT16 m_serverPort;
	San2::Network::SanAddress m_clientAddress;
	SAN_UINT16 m_clientPort;
	SAN_UINT64 m_expectedSeqNum;
};
