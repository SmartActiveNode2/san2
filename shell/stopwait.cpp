

#include "stopwait.hpp"
#include "utils/cdatapack.hpp"


StopWait::StopWait(unsigned int repetitions, unsigned int timeout):
	m_repetitions(repetitions),
	m_timeout(timeout),
	m_expectedSeqNum(1)
{
	
}

bool StopWait::sendReliableMessage(const San2::Utils::bytes& request, San2::Utils::bytes& response)
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
	
	unsigned int tries = m_repetitions;
	
	while(tries > 0)
	{
		if (sendDatagram(data) == false)
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
