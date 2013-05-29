

#include "stopwaitrx.hpp"
#include "utils/cdatapack.hpp"
#include "utils/cvector.hpp"

StopWaitRx::StopWaitRx():
	m_expectedSeqNum(1)
{
	
}

bool StopWaitRx::incommingDatagram(const San2::Utils::bytes& request, San2::Utils::bytes& response)
{	
	// Check structure of request
	if (request.size() < 9) return false;
	if (request[0] != 0x53) return false;
		
	SAN_UINT64 rxseq = San2::Utils::CDataPack::unpackUint64(request, 1);
		
	if (rxseq != m_expectedSeqNum)
	{
		printf("Sequence number missmatch\n");
		response = lastDatagram;
		return true;
	}
	
	// trim header
	San2::Utils::bytes dataIn;
	std::copy(request.cbegin() + 9, request.cend(), std::back_inserter(dataIn));
	
	bool ret = processDatagram(dataIn, response);
	lastDatagram = response;
	m_expectedSeqNum++;
	return ret;
}

