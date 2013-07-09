
#include <iostream>

#include "clientsession.hpp"
#include "messageconstructor.hpp"

#include "crypto/dsrp/common.hpp"
#include "crypto/dsrp/dsrpexception.hpp"
#include "crypto/drel/datagramencryptor.hpp"
#include "crypto/drel/datagramdecryptor.hpp"
#include "crypto/drel/hashkeyderivator.hpp"
#include "crypto/drel/aesexception.hpp"
#include "crypto/dsrp/conversion.hpp"
#include "constants.hpp"

ClientSession::ClientSession(StopWaitTx &swtx) :
	m_swtx(swtx),
	ng(DragonSRP::Ng::predefined(SH_SRP_KEYPAIR)),
	math(hash, ng),
	srpclient(math, random, true)
{
	
}

int ClientSession::run(std::string strUsername, std::string strPassword)
{
	int rval;
	San2::Utils::bytes request, response;
	San2::Utils::bytes srpA;
	San2::Utils::bytes username, password;
	
	username = strUsername;
	password = strPassword;
	sca = srpclient.getAuthenticator(username, password);
	srpA = sca.getA();
				
	rval = enc_construct_A_message(strUsername, srpA, request);
			
	if (rval)
	{
		printf("ClientSession::run:enc_construct_A_message failed: %d\n", rval);
		return -1;
	}
			
	if (m_swtx.sendReliableMessage(request, response) == false)
	{
		printf("sending A message failed\n");
		return -2;
	}
	
	unsigned char errorCode;
	San2::Utils::bytes srpB;
	San2::Utils::bytes srpSalt;
	
	rval = enc_parse_P_message(response, srpSalt, srpB, errorCode);
	
	if (rval)
	{
		printf("ClientSession::run:enc_parse_P_message failed: %d\n", rval);
		return -3;
	}
	
	if (errorCode)
	{
		printf("ClientSession::run:enc_parse_P_message errorCode is non zero: %d\n", errorCode);
		return -4;
	}
	
	San2::Utils::bytes srpM1;
	
	try
	{
		srpM1 = srpclient.getM1(srpSalt, srpB, sca);
	}
	catch(DragonSRP::DsrpException &e)
	{
		std::cout << "DsrpException: " << e.what() << std::endl;
		return -5;
	}
	
	rval = enc_construct_B_message(srpM1, request);
	
	if (rval)
	{
		printf("ClientSession::run:enc_construct_B_message failed: %d\n", rval);
		return -6;
	}
	
	if (m_swtx.sendReliableMessage(request, response) == false)
	{
		printf("sending B message failed\n");
		return -7;
	}
	
	San2::Utils::bytes srpM2;
	
	rval = enc_parse_Q_message(response, srpM2, errorCode);
	
	if (rval)
	{
		printf("ClientSession::run:enc_parse_P_message failed: %d\n", rval);
		return -8;
	}
	
	if (errorCode)
	{
		printf("ClientSession::run:enc_parse_Q_message errorCode is non zero: %d\n", errorCode);
		return -9;
	}
	
	// check M2
	
	San2::Utils::bytes K; // session key
	
	try 
	{
		K = sca.getSessionKey(srpM2); 
	}
	catch(DragonSRP::DsrpException &e)
	{
		std::cout << "Authentication failed - bad password or other protocol failure." << std::endl;
		std::cout << "DsrpException: " << e.what() << std::endl;
		return -10;
	}
	
	std::cout << "Shared session key: ";
	San2::Utils::bytes::printBytes(K);
	std::cout << std::endl;
	std::cout << "Authentication SUCCESSFUL." << std::endl;
	
	
	// ---------------------------------
	
	// Setup key derivator
	DragonSRP::HashKeyDerivator keydrv(K, SH_AES256_KEYLEN, SH_IVLEN, SH_SHA1_OUTPUTLEN);
	
	std::cout << "srv enc:";
	San2::Utils::bytes::printBytes(keydrv.getServerEncryptionKey());
	std::cout << std::endl;
	
	std::cout << "srv IV():";
	San2::Utils::bytes::printBytes(keydrv.getServerIV());
	std::cout << std::endl;
	
	std::cout << "srv Mac():";
	San2::Utils::bytes::printBytes(keydrv.getServerMacKey());
	std::cout << std::endl;
	
	std::cout << "cli enc:";
	San2::Utils::bytes::printBytes(keydrv.getClientEncryptionKey());
	std::cout << std::endl;
	
	std::cout << "cli IV():";
	San2::Utils::bytes::printBytes(keydrv.getClientIV());
	std::cout << std::endl;
	
	std::cout << "cli Mac():";
	San2::Utils::bytes::printBytes(keydrv.getClientMacKey());
	std::cout << std::endl;
	
	// Oposite keys on server!
	// UsesClient keys!
	DragonSRP::DatagramEncryptor enc(keydrv.getClientEncryptionKey(), keydrv.getClientIV(), keydrv.getClientMacKey());
	
	// Uses server keys!
	DragonSRP::DatagramDecryptor dec(keydrv.getServerEncryptionKey(), keydrv.getServerIV(), keydrv.getServerMacKey());
	
	std::uint64_t seqNum = m_swtx.getNextSequenceNumber(); // important
	
	San2::Utils::bytes message;
	message = "This is a sample message";
	
	if (message.size() > SH_MAX_MSGLEN)
	{
		printf("message too big\n");
		return -11;
	}
	
	
	unsigned int encpacketLen;
	unsigned char encpacket[enc.getOverheadLen() + SH_MAX_MSGLEN];
	
	std::cout <<  "cli encryptedDatagram seqnum:" << seqNum << std::endl;
	enc.encryptAndAuthenticate((unsigned char *)&message[0], message.size(), seqNum, encpacket, &encpacketLen); // throws
	

	San2::Utils::bytes encmessage;
	encmessage.assign(encpacket, encpacket + encpacketLen);
	
	std::cout << "Client enc message: ";
	San2::Utils::bytes::printBytes(encmessage);
	std::cout << std::endl;
	
	rval = enc_construct_C_message(encmessage, request);
	
	if (rval)
	{
		printf("ClientSession::run:enc_construct_C_message failed: %d\n", rval);
		return -12;
	}
	
	if (m_swtx.sendReliableMessage(request, response) == false)
	{
		printf("sending C message failed\n");
		return -13;
	}
	
	San2::Utils::bytes encryptedResponse;
	// parse message from server
	rval = enc_parse_R_message(response, encryptedResponse, errorCode);
	
	if (rval)
	{
		printf("ClientSession::run:enc_parse_R_message failed: %d\n", rval);
		return -14;
	}
	
	if (errorCode != 0)
	{
		printf("ClientSession::run: R message errorcode is non zero: %d\n", errorCode);
		return -15;
	}
	
	// ok, now we have succesfully received some encrypted data, lets decrpyt them
	// Decrypt
	unsigned int decpacketLen;
	unsigned char decpacket[dec.getOverheadLen() + SH_MAX_MSGLEN];
		
	dec.decryptAndVerifyMac(&encryptedResponse[0], encryptedResponse.size(), decpacket, &decpacketLen, seqNum);
	
	std::cout << "decpacket: ";
	// DragonSRP::Conversion::printHex(decpacket, decpacketLen);
	
	for (int b = 0; b < decpacketLen; b++)
	{
		printf("%c", decpacket[b]);
	}
	
	std::cout << std::endl;
	
	return 0;
}

bool ClientSession::processDecryptedDatagram(SAN_UINT64 sequenceNumber, const San2::Utils::bytes& request, San2::Utils::bytes& response)
{
	return true;
}
