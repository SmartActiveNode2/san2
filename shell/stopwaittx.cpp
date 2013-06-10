
#include "stopwaittx.hpp"

StopWaitTx::StopWaitTx(unsigned int repetitions, unsigned int timeout, San2::Api::CNodeConnector &connector, San2::Network::SanAddress dstAddress, SAN_UINT16 dstPort, San2::Network::SanAddress srcAddress, SAN_UINT16 srcPort) :
	StopWait(repetitions, timeout),
	m_connector(connector),
	m_dstAddress(dstAddress),
	m_dstPort(dstPort),
	m_srcAddress(srcAddress),
	m_srcPort(srcPort)
{
	
}
	
bool StopWaitTx::sendDatagram(San2::Utils::bytes& data)
{
	San2::Network::CCapsule capsule;
	San2::Utils::bytes serializedCapsule;
	capsule.setDSdata(m_dstPort, m_srcPort, data);
	capsule.setDestinationAddress(m_dstAddress);
	capsule.setSourceAddress(m_srcAddress);
	capsule.pack(serializedCapsule);
	return m_connector.sendCapsule(serializedCapsule);
}


bool StopWaitTx::awaitDatagram(San2::Utils::bytes& data, unsigned int maxWaitMsec)
{
	San2::Network::CCapsule capsule;
	if (SAN2_WAITFORCAPSULE_SUCCESS != m_connector.waitForCapsule(capsule, maxWaitMsec)) return false;
	data = capsule.getData();
	return true;
}
