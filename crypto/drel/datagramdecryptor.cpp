
#include <string.h>
#include <stdio.h>
#include <iostream>

#include "datagramdecryptor.hpp"
#include "../dsrp/conversion.hpp"
#include "utils/platform/basictypes.hpp"

namespace DragonSRP
{

	DatagramDecryptor::DatagramDecryptor(const bytes &encryptionKey, const bytes &IV, const bytes &macKey) :
		aesCtr(&IV[0], IV.size(), &encryptionKey[0], encryptionKey.size()),
		hmac(sha1, macKey)
	{
		if (macKey.size() < sha1.outputLen()) throw DsrpException("DatagramEncryptor::DatagramEncryptor: macKey not long enough");
		
	}
	
	unsigned int DatagramDecryptor::getOverheadLen()
	{
		return sizeof(std::uint64_t) + DSRP_ENCPARAM_TRUNCDIGEST_SIZE;
	}
	
	// Assumes that sizeof(dataOut) >= inLen + getOverheadLen() !!!
	void DatagramDecryptor::decryptAndVerifyMac(const unsigned char *dataIn, unsigned int dataInLen, unsigned char *dataOut, unsigned int *dataOutLen, std::uint64_t expectedSequenceNumber)
	{			
		if (dataInLen <= DSRP_ENCPARAM_TRUNCDIGEST_SIZE) throw DsrpException("Malformed packet decryption attempt");
		
		// First we need to verify the digest so we first compute the correct digest an then compare it
		memcpy(dataOut, dataIn, dataInLen - DSRP_ENCPARAM_TRUNCDIGEST_SIZE); // dataOut = EncdataIn - digest;
		std::uint64_t beSequencenumber = San2::Utils::Endian::san_u_htobe64(expectedSequenceNumber);
		memcpy(dataOut + dataInLen - DSRP_ENCPARAM_TRUNCDIGEST_SIZE, &beSequencenumber, sizeof(std::uint64_t)); // append sequence number to dataOut

		// Compute the real digest
		bytes toCompute(dataOut, dataOut + dataInLen - DSRP_ENCPARAM_TRUNCDIGEST_SIZE + sizeof(std::uint64_t));
		bytes correctDigest;
				
		hmac.hmac(toCompute, correctDigest);

		// Compare digests
		if (memcmp(&correctDigest[0], dataIn + dataInLen - DSRP_ENCPARAM_TRUNCDIGEST_SIZE, DSRP_ENCPARAM_TRUNCDIGEST_SIZE)) throw DsrpException("Mac signature inccorect. Possible attack detected.");
				
		// Set corret output length
		*dataOutLen = dataInLen - DSRP_ENCPARAM_TRUNCDIGEST_SIZE;
		
		// Finally decrypt data	 (dataIn -----decrypt-----> dataOut)
		// Note that we reused the DataOut to save memory
		aesCtr.encrypt(dataIn, dataOut, *dataOutLen, expectedSequenceNumber); // throws on error
	}


}
