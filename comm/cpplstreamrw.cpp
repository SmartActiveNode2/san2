
#include <algorithm>
#include "cpplstreamrw.hpp"

namespace San2 { namespace Comm {

CpplStreamRW::CpplStreamRW(const unsigned int maxSingleReadSize, San2::Cppl::BufferProcessor *bp) :
	San2::Stream::CIStreamRW(maxSingleReadSize),
	m_bp(bp),
	m_maxSingleReadSize(maxSingleReadSize)
{
    // No, this is a msvc fix. (MSVC does not support variable length arrays)
	rsapBuffer = (char *) malloc(sizeof(char) * m_maxSingleReadSize);
}

CpplStreamRW::~CpplStreamRW()
{
    // No, this is a msvc fix. (MSVC does not support variable length arrays)
    if (rsapBuffer != NULL) free(rsapBuffer);
}

// It is guaranteed by base interface class that (maxCount <= maxSingleReadSize)
int CpplStreamRW::readSomeAppend(San2::Utils::bytes &data, unsigned int maxCount)
{
	unsigned int bytesRead;
    
	//printf("CpplStreamRW::readSomeAppend: maxCount:%d\n", maxCount);
	San2::Cppl::ErrorCode ret = m_bp->readSome(rsapBuffer, maxCount, &bytesRead); // Very ugly maxCount
	
	if (ret != San2::Cppl::ErrorCode::SUCCESS)
	{ 
        // TODO: logger
		// printf("FAIL: CpplStreamRW::readSomeAppend::1::::errcode:%d\n", San2::Cppl::errorCodeToInt(ret));
		return -1;
	}
	if (bytesRead <= 0)
	{ 
        // TODO: logger
		printf("FAIL: CpplStreamRW::readSomeAppend::2\n");
		return -1;
	}
	
	std::copy(rsapBuffer, rsapBuffer + bytesRead, std::back_inserter(data)); // append
	return bytesRead;
}

// RETURN VALUE: number of bytes read, -1 on error, 0 on timeout
int CpplStreamRW::writeSome(San2::Utils::bytes::const_iterator first, San2::Utils::bytes::const_iterator last)
{
	unsigned int dataSize = last - first;
	if (m_bp->send((char *)&(*first), dataSize) == San2::Cppl::ErrorCode::SUCCESS) // ugly!
		return dataSize;
	else return -1;
}

}} // ns
