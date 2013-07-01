
#include <string.h>

#include <cstdint>
#include <stdio.h>

#include "aescounter.hpp"
#include "aesexception.hpp"

namespace DragonSRP
{

	AesCounter::AesCounter(const unsigned char *salt, int saltlen, const unsigned char *key, int keylen) : mIv(0)
	{
		if (saltlen != DREL_AES_SALTLEN) throw AesException("Worng salt length");
		if (keylen != DREL_AES_KEYLEN) throw AesException("Worng key length");
		
		memcpy(mSalt, salt, DREL_AES_SALTLEN);
		memcpy(mKey, key, DREL_AES_KEYLEN);
		
		if (aes_encrypt_key256(mKey, aes_ctx) != EXIT_SUCCESS) throw AesException("Key setup failed");
	}


	void AesCounter::encrypt(const unsigned char *datain, unsigned char *dataout, int len)
	{
		memset(ctr_buf, 0, DREL_AES_BLOCKLEN_BYTES);
		
		if (len <= 0 || len > DREL_AES_MAXPACKETOCT) throw AesException("Invalid input plaintext message length");
		if (mIv == 0xFFFFFFFFFFFFFFFF) throw AesException("Packet counter exceeded - IV");
		mIv++; // increment IV
		memcpy(ctr_buf, mSalt, DREL_AES_SALTLEN);
		memcpy(ctr_buf + DREL_AES_SALTLEN, &mIv, DREL_AES_IVLEN);
		
		std::uint16_t packetCtr = 1;
		packetCtr = htobe16(packetCtr);
		
		memcpy(ctr_buf + DREL_AES_SALTLEN + DREL_AES_IVLEN, &mIv, sizeof(std::uint16_t));
		
		//aes_mode_reset(aes_ctx); // important!!! do not delete!!!
		if (aes_encrypt_key256(mKey, aes_ctx) != EXIT_SUCCESS) throw AesException("Key setup failed");		
		
		int ret = aes_ctr_crypt(datain, dataout, len, ctr_buf, aes_custom_inc, aes_ctx);
		if (ret != EXIT_SUCCESS) throw AesException("Aes encryption failed");
	}


	// Use only inside one packet
	void aes_custom_inc(unsigned char ctr_buf[DREL_AES_BLOCKLEN_BYTES])
	{
		std::uint16_t* packetCtr = (std::uint16_t*)(ctr_buf + DREL_AES_SALTLEN + DREL_AES_IVLEN);
		*packetCtr = be16toh(*packetCtr);
		if (*packetCtr == 0xFFFF) throw AesException("Packet counter exceeded - PA");
		*packetCtr++;
		*packetCtr = htobe16(*packetCtr);
	}

	std::uint64_t AesCounter::getCurrentIV()
	{
		return mIv;
	}
}
