
#ifndef DREL_AESCOUNTER_HPP
#define DREL_AESCOUNTER_HPP

#define DREL_AES_BLOCKLEN_BYTES 16 // AES blocksize (constant, do not change)
#define DREL_AESCOUNTER_SECRETIVLEN 6 // constant, derived form SRP session key at construction
#define DREL_AES_KEYLEN 32 // 256 bit AES key
#define DREL_AES_MAXPACKETOCT 65534 // maximum size of encrypted payload

#include <cstdint>

#ifdef LINUX
	#include <sys/types.h>
	#include <inttypes.h>
#endif

#include "../aes/aes.h"


namespace DragonSRP
{
	void aes_custom_inc(unsigned char ctr_buf[DREL_AES_BLOCKLEN_BYTES]);
	
	class AesCounter
	{
		private:
			unsigned char mSecretIV[DREL_AESCOUNTER_SECRETIVLEN]; // derived form SRP session key
			unsigned char mKey[DREL_AES_KEYLEN]; // AES256 key
			aes_encrypt_ctx aes_ctx[1]; // AES state (internal)
			unsigned char ctr_buf[DREL_AES_BLOCKLEN_BYTES]; // counter buffer
			
		public:
			AesCounter(const unsigned char *secretIV, int secretIVlen, const unsigned char *key, int keylen);
			void encrypt(const unsigned char *datain, unsigned char *dataout, int len, std::uint64_t sequenceNumber);
	};
}
#endif

