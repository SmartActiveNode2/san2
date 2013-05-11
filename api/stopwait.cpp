

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
	// SEQ: UINT8 . Starting with one!
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
			tries--;
			continue;
		}
		
		if (awaitDatagram(response, m_timeout) == false)
		{
			tries--;
			continue;
		} 
		
		// Check structure of response
		if (response.size() < 2) return false;
		if (response[0] != 0x53) return false;
		
		SAN_UINT8 rxseq = San2::Utils::CDataPack::unpackUint8(response, 1);
		
		if (rxseq != m_expectedSeqNum)
		{
			tries--;
			continue;
		}
		
		// success
		response.erase(response.begin(), response.begin() + 2); // trim header
		m_expectedSeqNum++;
		return true;
	}
	
	return false;
}
