// Dragon Encryption Layer

#ifndef DREL_AESCOUNTER_HPP
#define DREL_AESCOUNTER_HPP

#define DREL_AES_BLOCKLEN_BYTES 16
#define DREL_AES_SALTLEN 7
#define DREL_AES_IVLEN 8
#define DREL_AES_PALEN 1

#define DREL_AES_KEYLEN 32

// ( (DREL_AES_KEYLEN * DREL_AES_BLOCKLEN_BYTES) - 1)
#define DREL_AES_MAXPACKETOCT 4095

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
			
			// Secret salt
			unsigned char mSalt[DREL_AES_SALTLEN];
		
			//  Incremented initialization vector
			//  starts with one
			//  owerflow check needed

			std::uint64_t mIv;
			
			// Packet counter
			// starts with 1
			// owerflow check needed - implemented in aes_custom_inc
			
			// AES256 key
			unsigned char mKey[DREL_AES_KEYLEN];
			
			aes_encrypt_ctx aes_ctx[1];
			unsigned char ctr_buf[DREL_AES_BLOCKLEN_BYTES];
		
		public:
			AesCounter(const unsigned char *salt, int saltlen, const unsigned char *key, int keylen);
			void encrypt(const unsigned char *datain, unsigned char *dataout, int len);
			uint64_t getCurrentIV();
	};
}
#endif

