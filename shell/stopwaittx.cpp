
#include "stopwaittx.hpp"

StopWaitTx::StopWaitTx(unsigned int repetitions, unsigned int timeout, San2::Api::CNodeConnector &connector, San2::Network::SanAddress serverAddress, SAN_UINT16 serverPort, San2::Network::SanAddress clientAddress, SAN_UINT16 clientPort) :
	StopWait(repetitions, timeout),
	m_connector(connector),
	m_serverAddress(serverAddress),
	m_serverPort(serverPort),
	m_clientAddress(clientAddress),
	m_clientPort(clientPort)
{
	
}
	
bool StopWaitTx::sendDatagram(San2::Utils::bytes& data)
{
	San2::Network::CCapsule capsule;
	San2::Utils::bytes serializedCapsule;
	capsule.setDSdata(m_serverPort, m_clientPort, data);
	capsule.setDestinationAddress(m_serverAddress);
	capsule.setSourceAddress(m_clientAddress);
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
