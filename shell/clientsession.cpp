
#include <iostream>

#include "clientsession.hpp"
#include "messageconstructor.hpp"

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
	char line[SH_TERMINAL_MAXLINLEN];
	
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
	
	if (errorCode == SH_ERRORCODE_USERNOTFOUND)
	{
		printf("User was not found in the servers database\n");
		return -9;
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
		printf("ClientSession::run:enc_parse_Q_message failed: %d\n", rval);
		return -8;
	}
	
	if (errorCode == SH_ERRORCODE_AUTHENTICATIONFAILED)
	{
		printf("Authentication FAILED - bad password\n");
		return -9;
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
	
	std::cout << "Authentication SUCCESSFUL." << std::endl;
		
	// ==============================================================================
	
	// Setup key derivator
	DragonSRP::HashKeyDerivator keydrv(K, SH_AES256_KEYLEN, SH_IVLEN, SH_SHA1_OUTPUTLEN);
	
	
	// Oposite keys on server!
	// Uses client keys!
	DragonSRP::DatagramEncryptor enc(keydrv.getClientEncryptionKey(), keydrv.getClientIV(), keydrv.getClientMacKey());
	// Uses server keys!
	DragonSRP::DatagramDecryptor dec(keydrv.getServerEncryptionKey(), keydrv.getServerIV(), keydrv.getServerMacKey());
	
	// ==============================================================================
	
	
	San2::Utils::bytes shellRequest, shellResponse;
	
	shellRequest = "hostname";
	getShellServerResponse(enc, dec, shellRequest, shellResponse);
	
	std::string shell = San2::Utils::bytes::bytes2string(shellResponse) + ">";
	
	while(1)
	{
		std::cout << shell;
		
		std::cin.getline(line, SH_TERMINAL_MAXLINLEN);
		
		if (strlen(line) == 0) continue;
		
		if (strcmp(line, "exit") == 0)
		{
			return 0; // ??
		}
		
		shellRequest = line;
		getShellServerResponse(enc, dec, shellRequest, shellResponse);
		
		for (unsigned int b = 0; b < shellResponse.size(); b++) printf("%c", shellResponse[b]); printf("\n");
	}
	
	
	// shellRequest = "This is a sample message";
	// getShellServerResponse(enc, dec, shellRequest, shellResponse);
	// printf("decpacket: "); for (int b = 0; b < shellResponse.size(); b++) printf("%c", shellResponse[b]); printf("\n");
	
	return 0;
}


int ClientSession::getShellServerResponse(DragonSRP::DatagramEncryptor& encryptor, DragonSRP::DatagramDecryptor& decryptor, const San2::Utils::bytes& shellRequest, San2::Utils::bytes& shellResponse)
{
	int rval;
	unsigned char errorCode;
	San2::Utils::bytes encryptedRequest, serverRequest, serverResponse, encryptedResponse;
	std::uint64_t sequenceNumber = m_swtx.getNextSequenceNumber(); // important
	unsigned int encpacketLen;
	unsigned char encpacket[encryptor.getOverheadLen() + SH_MAX_MSGLEN];
	unsigned int decpacketLen;
	unsigned char decpacket[decryptor.getOverheadLen() + SH_MAX_MSGLEN];
	
	shellResponse.clear();
	
	if (shellRequest.size() > SH_MAX_MSGLEN)
	{
		printf("ClientSession::getShellServerResponse:error: Message too big (shellRequest.size() > SH_MAX_MSGLEN)\n");
		return -1;
	}
	
	encryptor.encryptAndAuthenticate((unsigned char *)&shellRequest[0], shellRequest.size(), sequenceNumber, encpacket, &encpacketLen); // throws
	encryptedRequest.assign(encpacket, encpacket + encpacketLen);
	rval = enc_construct_C_message(encryptedRequest, serverRequest);
	
	if (rval)
	{
		printf("ClientSession::getShellServerResponse:enc_construct_C_message failed: %d\n", rval);
		return -2;
	}
	
	if (m_swtx.sendReliableMessage(serverRequest, serverResponse) == false)
	{
		printf("ClientSession::getShellServerResponse:sending C message failed\n");
		return -3;
	}
	
	rval = enc_parse_R_message(serverResponse, encryptedResponse, errorCode);
	
	if (rval)
	{
		printf("ClientSession::getShellServerResponse:enc_parse_R_message failed: %d\n", rval);
		return -4;
	}
	
	if (errorCode != 0)
	{
		printf("ClientSession::getShellServerResponse: R message errorcode is non zero: %d\n", errorCode);
		return -5;
	}
	
	decryptor.decryptAndVerifyMac(&encryptedResponse[0], encryptedResponse.size(), decpacket, &decpacketLen, sequenceNumber);
	shellResponse.assign(decpacket, decpacket + decpacketLen);
	
	return 0;
}
