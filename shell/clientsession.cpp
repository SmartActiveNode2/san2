
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
	return 0;
}

bool ClientSession::processDecryptedDatagram(SAN_UINT64 sequenceNumber, const San2::Utils::bytes& request, San2::Utils::bytes& response)
{
	return true;
}
