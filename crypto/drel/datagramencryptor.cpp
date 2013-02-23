
#include <string.h>

#include <stdio.h>

#include "datagramencryptor.hpp"

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
		return DSRP_ENCPARAM_SEQ_SIZE + DSRP_ENCPARAM_TRUNCDIGEST_SIZE;
	}
	
	// Assumes sizeof(out) >= plaintextLen + getOverheadLen()   [MTU + OVERHEAD]
	void DatagramEncryptor::encryptAndAuthenticate(const unsigned char *plaintext, unsigned int plaintextLen, unsigned char *out, unsigned int *outLen) // throws
	{
		uint64_t seqNum = aesCtr.getCurrentIV() + 1;
		
		memcpy(out, &seqNum, DSRP_ENCPARAM_SEQ_SIZE); // set SEQ
		
		// SET ENC(DATA)
		// in -----encrypt-----> encdata
		// ---> seqNum
		aesCtr.encrypt(plaintext, out + DSRP_ENCPARAM_SEQ_SIZE , plaintextLen); // Possible optim. direct to &out[lenSize + seqSize]
		
		// SET DIGEST
		// Add trunc digest

		#ifdef WINDOWS
			bytes digest;
			digest.resize(hmac.outputLen());
			hmac.hmac(out, DSRP_ENCPARAM_SEQ_SIZE + plaintextLen, &digest[0]);
			memcpy(out + DSRP_ENCPARAM_SEQ_SIZE + plaintextLen, &digest[0], DSRP_ENCPARAM_TRUNCDIGEST_SIZE); // could be avoided (optim.)
		#endif

		#ifdef LINUX
			unsigned char digest[hmac.outputLen()];
			hmac.hmac(out, DSRP_ENCPARAM_SEQ_SIZE + plaintextLen, digest);
			memcpy(out + DSRP_ENCPARAM_SEQ_SIZE + plaintextLen, digest, DSRP_ENCPARAM_TRUNCDIGEST_SIZE); // could be avoided (optim.)
		#endif
		
		*outLen = plaintextLen + DSRP_ENCPARAM_SEQ_SIZE + DSRP_ENCPARAM_TRUNCDIGEST_SIZE;
	}


}
