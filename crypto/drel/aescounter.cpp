
#include <string.h>

#include <cstdint>
#include <stdio.h>

#include "aescounter.hpp"
#include "aesexception.hpp"
#include "utils/platform/basictypes.hpp"

namespace DragonSRP
{

	AesCounter::AesCounter(const unsigned char *secretIV, int secretIVlen, const unsigned char *key, int keylen)
	{
		if (secretIVlen != DREL_AESCOUNTER_SECRETIVLEN) throw AesException("Worng secretIV length");
		if (keylen != DREL_AES_KEYLEN) throw AesException("Worng key length");
		
		memcpy(mSecretIV, secretIV, DREL_AESCOUNTER_SECRETIVLEN);
		memcpy(mKey, key, DREL_AES_KEYLEN);
		
		if (aes_encrypt_key256(mKey, aes_ctx) != EXIT_SUCCESS) throw AesException("Key setup failed");
	}


	void AesCounter::encrypt(const unsigned char *datain, unsigned char *dataout, int len, std::uint64_t sequenceNumber)
	{
		memset(ctr_buf, 0, DREL_AES_BLOCKLEN_BYTES);
		
		if (len <= 0 || len > DREL_AES_MAXPACKETOCT) throw AesException("Invalid input plaintext message length");
		if (sequenceNumber == 0xFFFFFFFFFFFFFFFF) throw AesException("Packet counter exceeded - IV");
		
		
		// Copy secretIV to counter buffer
		memcpy(ctr_buf, mSecretIV, DREL_AESCOUNTER_SECRETIVLEN);
		
		// Copy packet sequence number to counter buffer
		std::uint64_t beSequencenumber = San2::Utils::Endian::san_s_be64toh(sequenceNumber);
		memcpy(ctr_buf + DREL_AESCOUNTER_SECRETIVLEN, &beSequencenumber, sizeof(std::uint64_t));
		
		// Set blockCounter to one
		std::uint16_t blockCounter = 1;
		blockCounter = San2::Utils::Endian::san_u_htobe16(blockCounter);
		memcpy(ctr_buf + DREL_AESCOUNTER_SECRETIVLEN + sizeof(std::uint64_t), &blockCounter, sizeof(std::uint16_t));
		
		//aes_mode_reset(aes_ctx); // important!!! do not delete!!!
		if (aes_encrypt_key256(mKey, aes_ctx) != EXIT_SUCCESS) throw AesException("Key setup failed");		
		
		int ret = aes_ctr_crypt(datain, dataout, len, ctr_buf, aes_custom_inc, aes_ctx);
		if (ret != EXIT_SUCCESS) throw AesException("Aes encryption failed");
	}


	// Use only inside one packet
	void aes_custom_inc(unsigned char ctr_buf[DREL_AES_BLOCKLEN_BYTES])
	{
		std::uint16_t* blockCounter = (std::uint16_t*)(ctr_buf + DREL_AESCOUNTER_SECRETIVLEN + sizeof(std::uint64_t));
		*blockCounter = San2::Utils::Endian::san_u_be16toh(*blockCounter);
		if (*blockCounter == 0xFFFF) throw AesException("Block counter exceeded!");
		*blockCounter++;
		*blockCounter = San2::Utils::Endian::san_u_htobe16(*blockCounter);
	}
}
