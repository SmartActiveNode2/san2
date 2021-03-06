
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include "../dsrp/common.hpp"
#include "../dsrp/dsrpexception.hpp"
#include "../drel/datagramencryptor.hpp"
#include "../drel/datagramdecryptor.hpp"
#include "../drel/simplekeyderivator.hpp"
#include "../drel/aesexception.hpp"
#include "../dsrp/conversion.hpp"

#define MAX_DATA_LEN 1500

using namespace std;
using namespace DragonSRP;

int sessionKey[128]  =   {0xFE, 0x5B, 0x4A, 0xCC, 0xFE, 0x5B, 0x4A, 0xCC,
						  0xFE, 0x5B, 0x4A, 0xCC, 0xFE, 0x5B, 0x4A, 0xCC,
						  0xFE, 0x5B, 0x4A, 0xCC, 0xFE, 0x5B, 0x4A, 0xCC,
						  0xFE, 0x5B, 0x4A, 0xCC, 0xFE, 0x5B, 0x4A, 0xCC,
						  0xFE, 0x5B, 0x4A, 0xCC, 0xFE, 0x5B, 0x4A, 0xCC,
						  0xFE, 0x5B, 0x4A, 0xCC, 0xFE, 0x5B, 0x4A, 0xCC,
						  0xFE, 0x5B, 0x4A, 0xCC, 0xFE, 0x5B, 0x4A, 0xCC,
						  0xFE, 0x5B, 0x4A, 0xCC, 0xFE, 0x5B, 0x4A, 0xCC,
						  0xFE, 0x5B, 0x4A, 0xCC, 0xFE, 0x5B, 0x4A, 0xCC,
						  0xFE, 0x5B, 0x4A, 0xCC, 0xFE, 0x5B, 0x4A, 0xCC,
						  0xFE, 0x5B, 0x4A, 0xCC, 0xFE, 0x5B, 0x4A, 0xCC,
						  0xFE, 0x5B, 0x4A, 0xCC, 0xFE, 0x5B, 0x4A, 0xCC,
						  0xFE, 0x5B, 0x4A, 0xCC, 0xFE, 0x5B, 0x4A, 0xCC,
						  0xFE, 0x5B, 0x4A, 0xCC, 0xFE, 0x5B, 0x4A, 0xCC,
						  0xFE, 0x5B, 0x4A, 0xCC, 0xFE, 0x5B, 0x4A, 0xCC,
						  0xFE, 0x5B, 0x4A, 0xCC, 0xFE, 0x5B, 0x4A, 0xCC};

unsigned char data[] = "In some ways, taxidermy can be viewed as a precursor to photography — a static, visual representation of life. And while the process itself is fascinating as a morbid half-resurrection, a bridge between the living and the dead, it’s the necromancers themselves who are the subject of photographer Mike McGregor’s series Preserve.";

void printVar(const char *szVarname, const unsigned char *data, unsigned int len)
{
	printf("%s: ", szVarname);
	for (unsigned int i = 0; i < len; i++) printf("%02X", data[i]);
	printf("\n");
}

int test_crypto(std::uint64_t seqNum, unsigned char *dataIn, std::size_t dataInLen, DatagramEncryptor &enc, DatagramDecryptor &dec) // throws
{
	if (dataInLen > MAX_DATA_LEN)
	{
		printf("data too big\n");
		return -1;
	}
	
	unsigned int encpacketLen;
	unsigned char encpacket[enc.getOverheadLen() + MAX_DATA_LEN];
	enc.encryptAndAuthenticate((unsigned char *)dataIn, dataInLen, seqNum, encpacket, &encpacketLen);
	
	// Decrypt
	unsigned int decpacketLen;
	unsigned char decpacket[dec.getOverheadLen() + MAX_DATA_LEN];
		
	dec.decryptAndVerifyMac(encpacket, encpacketLen, decpacket, &decpacketLen, seqNum);
		
	if (dataInLen != decpacketLen)
	{
		printf("enc/dec fail: different data length\n");
		throw DragonSRP::DsrpException("Cryptotest: Failed");
		return -1;
	}
	
	std::cout << "encpacket: ";
	DragonSRP::Conversion::printHex(encpacket, encpacketLen);
	std::cout << std::endl;
		
	// replace with memcmp	
	for (unsigned int i = 0; i < dataInLen; i++)
	{
	   	if (data[i] != decpacket[i])
		{
			throw DragonSRP::DsrpException("Cryptotest: enc/dec fail: decrypted data does not match original plaintext data");
			printf("enc/dec fail: decrypted data does not match original plaintext data\n");
			return -1;
		}
	}
		
	printf("decrypted text: %s\n", decpacket); // should check term zero;
	printf("finished\n");
}

int main(int argc, char *argv[])
{	
	bytes bSessionKey;	
	bSessionKey.assign(sessionKey, sessionKey + 128);
	SimpleKeyDerivator skd(bSessionKey, 32, 6, 20);
	DatagramEncryptor enc(skd.getClientEncryptionKey(), skd.getClientIV(), skd.getClientMacKey());
	DatagramDecryptor dec(skd.getClientEncryptionKey(), skd.getClientIV(), skd.getClientMacKey());
	
	try 
	{
		test_crypto(1, data, sizeof(data), enc, dec);
		test_crypto(2, data, sizeof(data), enc, dec);
		test_crypto(3, data, sizeof(data), enc, dec);
		
		test_crypto(24653, data, sizeof(data), enc, dec);
		test_crypto(10000000000, data, sizeof(data), enc, dec);
	}
	catch (DsrpException &e)
	{
		cout << "DsrpException: " << e.what() << endl;
		return -1;
	}
	catch (AesException &e)
	{
		cout << "AESException: " << e.what() << endl;
		return -1;
	}
	catch (exception& e)
	{
		cout << e.what() << endl;
	}
	catch (...)
	{
		cout << "unknown exception occured" << endl;
		return -1;
	}
	
	printf("enc/dec seems ok\n");
}

/*
int main(int argc, char *argv[])
{	
	std::uint64_t seqNum = 1;
	
	try {
		bytes bSessionKey;
		
		bSessionKey.assign(sessionKey, sessionKey + 128);
		
		SimpleKeyDerivator skd(bSessionKey, 32, 6, 20);
		
		// printf("macKeySize: %d\n", skd.getClientMacKey().size());
		
		DatagramEncryptor enc(skd.getClientEncryptionKey(), skd.getClientIV(), skd.getClientMacKey());
		DatagramDecryptor dec(skd.getClientEncryptionKey(), skd.getClientIV(), skd.getClientMacKey());
		
		printf("data: %s\n", data);
		
		unsigned int dataLen = sizeof(data);
		printVar("data", data, dataLen);
		
		if (dataLen > MAX_DATA_LEN)
		{
			printf("data too big\n");
			return -1;
		}
			
		unsigned int encpacketLen;
		unsigned char encpacket[enc.getOverheadLen() + MAX_DATA_LEN];

		std::cout << "dataLen: " << dataLen << std::endl;
		enc.encryptAndAuthenticate((unsigned char *)data, dataLen, seqNum, encpacket, &encpacketLen);
		std::cout << "encpacketLen: " << encpacketLen << std::endl;
		
		
		std::cout << "Cryptotest: encpacket: ";
		Conversion::printHex(encpacket, encpacketLen);
		std::cout << std::endl;
		
		printf("encpacketLen: %d\n", encpacketLen);
		printVar("encpacket", encpacket, encpacketLen);
		
		// Decrypt
		unsigned int decpacketLen;
		unsigned char decpacket[dec.getOverheadLen() + MAX_DATA_LEN];
		
		dec.decryptAndVerifyMac(encpacket, encpacketLen, decpacket, &decpacketLen, seqNum);
		
		if (dataLen != decpacketLen)
		{
			printf("enc/dec fail: different data length\n");
			return -1;
		}
		
		for (unsigned int i = 0; i < dataLen; i++)
		{
			if (data[i] != decpacket[i])
			{
				printf("enc/dec fail: decrypted data does not match original plaintext data\n");
				return -1;
			}
		}
		
		printf("decrypted text: %s\n", decpacket); // should check term zero;
		printf("finished\n");
	}
	catch (DsrpException &e)
	{
		cout << "DsrpException: " << e.what() << endl;
		return -1;
	}
	catch (AesException &e)
	{
		cout << "AESException: " << e.what() << endl;
		return -1;
	}
	catch (exception& e)
	{
		cout << e.what() << endl;
	}
	catch (...)
	{
		cout << "unknown exception occured" << endl;
		return -1;
	}
	
	printf("enc/dec seems ok\n");
}
* */
