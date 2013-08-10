
#include <string.h>
#include <stdio.h>
#include <iostream>

#include "datagramencryptor.hpp"
#include "../dsrp/conversion.hpp"
#include "utils/platform/basictypes.hpp"


// check if key persist aes_encrypt_key; !!!

namespace DragonSRP
{

	DatagramEncryptor::DatagramEncryptor(const bytes &encryptionKey, const bytes &IV, const bytes &macKey) :
		aesCtr(&IV[0], IV.size(), &encryptionKey[0], encryptionKey.size()),
		hmac(sha1, macKey)
	{
		if (macKey.size() < sha1.outputLen()) throw DsrpException("DatagramEncryptor::DatagramEncryptor: macKey not long enough");
		
	}
	
	unsigned int DatagramEncryptor::getOverheadLen()
	{
		return sizeof(std::uint64_t) + DSRP_ENCPARAM_TRUNCDIGEST_SIZE;
	}
	
	// Assumes sizeof(out) >= plaintextLen + getOverheadLen()   [MTU + OVERHEAD]
	void DatagramEncryptor::encryptAndAuthenticate(const unsigned char *plaintext, unsigned int plaintextLen, std::uint64_t sequenceNumber, unsigned char *out, unsigned int *outLen) // throws
	{	
		aesCtr.encrypt(plaintext, out, plaintextLen, sequenceNumber); // encrpyt plaintext
		
		// append sequence number to plainetxt
		std::uint64_t beSequencenumber = San2::Utils::Endian::san_u_htobe64(sequenceNumber);
		memcpy(out + plaintextLen, &beSequencenumber, sizeof(std::uint64_t)); // set SEQ
		
		// calculate digest	
		bytes digest;
		digest.resize(hmac.outputLen());
	
		bytes toSign(out, out + plaintextLen + sizeof(std::uint64_t));
		hmac.hmac(toSign, digest);
			
		// append just digest (overwrite sequence number)
		memcpy(out + plaintextLen, &digest[0], DSRP_ENCPARAM_TRUNCDIGEST_SIZE); // could be avoided (optim.)
		*outLen = plaintextLen + DSRP_ENCPARAM_TRUNCDIGEST_SIZE;
	}


}
