
#include "stopwaittx.hpp"
#include "utils/cdatapack.hpp"

StopWaitTx::StopWaitTx(unsigned int repetitions, unsigned int timeout, San2::Api::CNodeConnector &connector, San2::Network::SanAddress serverAddress, SAN_UINT16 serverPort, San2::Network::SanAddress clientAddress, SAN_UINT16 clientPort) :
	m_repetitions(repetitions),
	m_timeout(timeout),
	m_connector(connector),
	m_serverAddress(serverAddress),
	m_serverPort(serverPort),
	m_clientAddress(clientAddress),
	m_clientPort(clientPort),
	m_expectedSeqNum(1)
{
	
}

bool StopWaitTx::awaitDatagram(San2::Utils::bytes& data, unsigned int maxWaitMsec)
{
	SAN_UINT16 dstport, srcport;
	San2::Network::CCapsule capsule;
	if (SAN2_WAITFORCAPSULE_SUCCESS != m_connector.waitForCapsule(capsule, maxWaitMsec)) return false;
	
	if (capsule.getPortsDS(dstport, srcport) == false)
	{
		printf(" StopWaitTx::awaitDatagram(): getPortsDS - parse false\n");
		return false;
	}
	
	if (dstport != m_clientPort)
	{
		printf("Client port missmatch\n");
		return false;
	}
	
	if (srcport != m_serverPort)
	{
		printf("Server port missmatch\n");
		return false;
	}
	
	if (capsule.getSourceAddress() != m_serverAddress)
	{
		printf("Server address missmatch");
		return false;
	}
	
	
	data = capsule.getData();
	data.erase(data.begin(), data.begin() + 4); // remove ports
	
	return true;
}


bool StopWaitTx::sendReliableMessage(const San2::Utils::bytes& request, San2::Utils::bytes& response)
{
	response.clear();
	
	// Form the stop & wait packet format
	
	// SOF: ASCII "S"
	// SEQ: UINT64 . Starting with one!
	// PAYLOAD
	San2::Utils::bytes data;
	unsigned char sof = 0x53;
	data += sof;
	data += San2::Utils::CDataPack::pack(m_expectedSeqNum);
	data += request;
	
	San2::Network::CCapsule capsule;
	San2::Utils::bytes serializedCapsule;
	capsule.setDSdata(m_serverPort, m_clientPort, data);
	capsule.setDestinationAddress(m_serverAddress);
	capsule.setSourceAddress(m_clientAddress);
	capsule.pack(serializedCapsule);
	
	unsigned int tries = m_repetitions;
	
	while(tries > 0)
	{
		if (m_connector.sendCapsule(serializedCapsule) == false)
		{
			printf("StopWait::sendDatagram failed\n");
			return false;
		}
		
		if (awaitDatagram(response, m_timeout) == false)
		{
			tries--;
			printf("StopWait: Datagram lost; resending\n");
			continue;
		} 
		
		// Check structure of response
		if (response.size() < 9) return false;
		if (response[0] != 0x53) return false;
		
		SAN_UINT64 rxseq = San2::Utils::CDataPack::unpackUint64(response, 1);
		
		if (rxseq != m_expectedSeqNum)
		{
			tries--;
			printf("StopWait: incomming sequence number missmatch\n");
			continue;
		}
		
		// success
		response.erase(response.begin(), response.begin() + 9); // trim header
		m_expectedSeqNum++;
		return true;
	}
	
	return false;
}

San2::Network::SanAddress StopWaitTx::getServerAddress()
{
	return m_serverAddress;
}

SAN_UINT16 StopWaitTx::getServerPort()
{
	return m_serverPort;
}

San2::Network::SanAddress StopWaitTx::getClientAddress()
{
	return m_clientAddress;
}

SAN_UINT16 StopWaitTx::getClientPort()
{
	return m_clientPort;
}	
