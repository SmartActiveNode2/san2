
#pragma once

#include "utils/cvector.hpp"
#include "utils/platform/basictypes.hpp"
#include "stopwait.hpp"

class StopWaitTx : public StopWait
{
public:
	StopWaitTx(unsigned int repetitions, unsigned int timeout, San2::Api::CNodeConnector &connector, San2::Network::SanAddress dstAddress, SAN_UINT16 dstPort, San2::Network::SanAddress srcAddress, SAN_UINT16 srcPort);
	
	bool sendDatagram(San2::Utils::bytes& data);
	bool awaitDatagram(San2::Utils::bytes& data, unsigned int maxWaitMsec);

private:
	San2::Api::CNodeConnector &m_connector;
	San2::Network::SanAddress m_dstAddress;
	SAN_UINT16 m_dstPort;
	San2::Network::SanAddress m_srcAddress;
	SAN_UINT16 m_srcPort;
};
