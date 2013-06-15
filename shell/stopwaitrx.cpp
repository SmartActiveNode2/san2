
#include <stdio.h>
#include "stopwaitrx.hpp"
#include "utils/cdatapack.hpp"
#include "utils/cvector.hpp"

StopWaitRx::StopWaitRx(San2::Api::CNodeConnector &connector, const San2::Network::SanAddress& serverAddress, SAN_UINT16 serverPort, const San2::Network::SanAddress& clientAddress, SAN_UINT16 clientPort):
	m_connector(connector),
	m_serverAddress(serverAddress),
	m_serverPort(serverPort),	
	m_clientAddress(clientAddress),
	m_clientPort(clientPort),
	m_expectedSeqNum(1)
{
	
}

bool StopWaitRx::incommingCapsule(San2::Network::CCapsule &rxcapsule)
{	
	San2::Network::CCapsule txcapsule;
	SAN_UINT16 dstport, srcport;
	
	San2::Utils::bytes rxcapsuleData = rxcapsule.getData();
	rxcapsuleData.erase(rxcapsuleData.begin(), rxcapsuleData.begin() + 4); // remove ports
	
	// Check source address and port
	if (rxcapsule.getSourceAddress() != m_clientAddress)
	{
		printf("StopWaitRx::incommingCapsule(): Source address missmatch");
		return false;
	}
	
	if (rxcapsule.getPortsDS(dstport, srcport) == false)
	{
		printf("StopWaitRx::incommingCapsule(): Failed to parse ports");
		return false;
	}
	
	if (srcport != m_clientPort)
	{
		printf("StopWaitRx::incommingCapsule(): Source port missmatch");
		return false;
	}
	
	printf("inc00\n");
	if (rxcapsuleData.size() < 9) return false;
	printf("inc01\n");
	if (rxcapsuleData[0] != 0x53) return false;
	
	printf("inc02\n");
	
	// extract sequence number
	SAN_UINT64 rxseq = San2::Utils::CDataPack::unpackUint64(rxcapsuleData, 1);
		
	// check sequence number
	if (rxseq != m_expectedSeqNum)
	{
		printf("Sequence number missmatch\n");
		m_connector.sendCapsule(m_lastSerializedCapsule);
		return true;
	}
	
	printf("inc03\n");
	
	// trim header
	San2::Utils::bytes request;
	std::copy(rxcapsuleData.cbegin() + 9, rxcapsuleData.cend(), std::back_inserter(request));
	
	San2::Utils::bytes response;
	bool ret = processDatagram(rxseq, request, response);
	
	// add sof and sequence number to form correct response
	
	San2::Utils::bytes dataOut;
	dataOut += 0x53;
	dataOut += San2::Utils::CDataPack::pack(m_expectedSeqNum);
	dataOut += response;	
	
	m_expectedSeqNum++;
	printf("inc04\n");
	
	txcapsule.setSourceAddress(m_serverAddress);
	txcapsule.setDestinationAddress(m_clientAddress);
	txcapsule.setDSdata(m_clientPort, m_serverPort, dataOut); // m_dstPort == APPLICATION_PORT
	
	San2::Utils::bytes serializedCapsule;
	txcapsule.pack(serializedCapsule);
	
	m_lastSerializedCapsule = serializedCapsule;
	
	if (m_connector.sendCapsule(serializedCapsule) == false)
	{
		printf("StopWaitRx::incommingCapsule():m_connector.sendCapsule FALSE\n");
		return false;
	}
	
	printf("inc05\n");
	
	return ret;
}

