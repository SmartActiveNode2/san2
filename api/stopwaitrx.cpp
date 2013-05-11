

#include "stopwaitrx.hpp"
#include "utils/cdatapack.hpp"


StopWaitRx::StopWaitRx(unsigned int repetitions, unsigned int rxtimeout, std::function<bool(const San2::Utils::bytes&, San2::Utils::bytes&)> getAnswer):
	m_repetitions(repetitions),
	m_rxtimeout(rxtimeout),
	m_getAnswer(getAnswer)
	m_expectedSeqNum(1)
{
	
}


bool StopWaitRx::awaitMessage(San2::Utils::bytes &request)
{
	request.clear();
	
	if (awaitDatagram(request, m_rxtimeout) == false) return false;
	
	// Check structure of request
	if (request.size() < 2) return false;
	if (request[0] != 0x53) return false;
		
	SAN_UINT8 rxseq = San2::Utils::CDataPack::unpackUint8(request, 1);
		
	if (rxseq != m_expectedSeqNum)
	{
		printf("Sequence number missmatch\n");
		if (sendDatagram(lastDatagram) == false)
		{
			printf("Could not resend message\n");
			return false;
		}
		
		
		return false;
	}
		
	// success
	request.erase(request.begin(), request.begin() + 2); // trim header
	return true;
}

void StopWaitRx::run()
{
	San2::Utils::bytes request, response;
	
	while(!isTerminated())
	{
		// wait for message
		if (awaitDatagram(request, m_rxtimeout) == false) continue;
		// Check structure of request
		if (request.size() < 2) return false;
		if (request[0] != 0x53) return false;
		SAN_UINT8 rxseq = San2::Utils::CDataPack::unpackUint8(request, 1);
		
		if (rxseq != m_expectedSeqNum)
		{
			printf("Sequence number missmatch\n");
			if (sendDatagram(lastDatagram) == false)
			{
				printf("Could not resend message\n");
				return false;
			}	
		return false;
		}
	}
}
