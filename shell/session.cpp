#define WIN32_LEAN_AND_MEAN

#include <string>
#include <iostream>
#include "session.hpp"
#include "utils/log.h"
#include "messageconstructor.hpp"

#include "crypto/dsrp/common.hpp"
#include "crypto/dsrp/dsrpexception.hpp"

#include "crypto/drel/hashkeyderivator.hpp"
#include "crypto/drel/aesexception.hpp"
#include "crypto/dsrp/conversion.hpp"

#include "constants.hpp"

Session::Session(San2::Api::CNodeConnector &connector, const San2::Network::SanAddress& serverAddress, SAN_UINT16 serverPort, const San2::Network::SanAddress& clientAddress, SAN_UINT16 clientPort, DragonSRP::LookupInterface& lookup) :
	StopWaitRx(connector, serverAddress, serverPort, clientAddress, clientPort),
	m_connector(connector),
	m_state(SH_SRV_STATE_ZERO),
	ng(DragonSRP::Ng::predefined(SH_SRP_KEYPAIR)),
	math(hash, ng),
	m_lookup(lookup),
	srpserver(lookup, math, random, true),
	m_enc(NULL),
	m_dec(NULL)
{

}

bool Session::processDatagram(SAN_UINT64 sequenceNumber, const San2::Utils::bytes& request, San2::Utils::bytes& response)
{
	FILE_LOG(logDEBUG4) << "Session::processDatagram(): checkpoint #1";
	
	San2::Utils::bytes currentM1;
	San2::Utils::bytes currentM2;
	DragonSRP::bytes currentSalt;
	DragonSRP::bytes currentB;
	San2::Utils::bytes encrpytedMessage;
	San2::Utils::bytes encryptedResponse;
	
	int ret;
	
	switch(m_state)
	{
		case SH_SRV_STATE_ZERO:
			FILE_LOG(logDEBUG4) << "Session::processDatagram(): checkpoint #2";
			resetState();

			// parse A message
			ret = enc_parse_A_message(request, m_srpUsername, m_srpA);
			
			if (ret)
			{
				FILE_LOG(logDEBUG4) << "Session::processDatagram(): enc_parse_A_message() returned: " << ret;	
				// keep current state
				
				// inform client about the error
				ret = enc_construct_P_message_fail(response, SH_ERRORCODE_PARSINGFAILED);
				
				if (ret)
				{
					FILE_LOG(logDEBUG4) << "Session::processDatagram(): enc_construct_P_message_fail() #1 failed: " << ret;
					resetState();
					return false;
				}
			}	
		
			FILE_LOG(logDEBUG4) << "Session::processDatagram(): checkpoint #3";
		
			try
			{
				ver = srpserver.getVerificator(m_srpUsername, m_srpA);
			}
			catch (DragonSRP::UserNotFoundException e)
			{
				std::cout << "Session::processDatagram():UserNotFoundException: " << e.what() << std::endl;
					
				ret = enc_construct_P_message_fail(response, SH_ERRORCODE_USERNOTFOUND);
			
				if (ret)
				{
					FILE_LOG(logDEBUG4) << "Session::processDatagram(): enc_construct_P_message_fail() #2 failed: " << ret;
					resetState();
					return false;
				}
					
				resetState();
				return true;
			}
			catch (DragonSRP::DsrpException e)
			{
				std::cout << "Session::processDatagram():DsrpException: " << e.what() << std::endl;
					
				ret = enc_construct_P_message_fail(response, SH_ERRORCODE_SRPERROR);
					
				if (ret)
				{
					FILE_LOG(logDEBUG4) << "Session::processDatagram(): enc_construct_P_message_fail() #3 failed: " << ret;
					resetState();
					return false;
				}
					
				resetState();
				return true;
			}
				
			FILE_LOG(logDEBUG4) << "Session::processDatagram(): checkpoint #4";
				
			currentSalt = ver.getSalt();
			currentB = ver.getB();
				
			ret = enc_construct_P_message_success(currentSalt, currentB, response);
				
			if (ret)
			{
				FILE_LOG(logDEBUG4) << "Session::processDatagram(): enc_construct_P_message_fail() #4 failed: " << ret;
				resetState();
				return false;
			}
				
			// OK, change state				
			m_state = SH_SRV_STATE_ALPHA;
			FILE_LOG(logDEBUG4) << "Session::processDatagram(): STATE ---> APLHA";
			return true;
		case SH_SRV_STATE_ALPHA:
			ret = enc_parse_B_message(request, currentM1);
		
			if (ret)
			{
				FILE_LOG(logDEBUG4) << "Session::processDatagram(): enc_parse_A_message() returned: " << ret;
				
				// report error to client
				ret = enc_construct_Q_message_fail(response, SH_ERRORCODE_PARSINGFAILED);
				
				if (ret)
				{
					FILE_LOG(logDEBUG4) << "Session::processDatagram(): enc_construct_Q_message_fail() #1 failed: " << ret;
					resetState();
					return false;
				}
				
				resetState();
				return true;	
			}
			
			try 
			{
				// if M1 is OK we get M2 and K otherwise exception is thrown
				ver.authenticate(currentM1, currentM2, m_sessionK);
			}
			catch (DragonSRP::DsrpException e)
			{
				std::cout << "Session::processDatagram():SH_SRV_STATE_ALPHA: DsrpException: " << e.what() << std::endl;
				
				// report to client
				ret = enc_construct_Q_message_fail(response, SH_ERRORCODE_AUTHENTICATIONFAILED);
				
				if (ret)
				{
					FILE_LOG(logDEBUG4) << "Session::processDatagram(): enc_construct_Q_message_fail() #2 failed: " << ret;
					resetState();
					return false;
				}
				
				resetState();
				return true;
			}
		
			// Authentication OK
			
			ret = enc_construct_Q_message_success(currentM2, response);
			
			if (ret)
			{
				FILE_LOG(logDEBUG4) << "Session::processDatagram(): enc_construct_Q_message_success() failed: " << ret;
				resetState();
				return false;
			}

			// ok, change state
			m_state = SH_SRV_STATE_BETA;
			break;
			
		case SH_SRV_STATE_BETA:
			ret = enc_parse_C_message(request, encrpytedMessage);
			if (ret)
			{
				FILE_LOG(logDEBUG4) << "Session::processDatagram(): enc_parse_C_message failed (resteting state): " << ret;
				resetState();
				return false;
			}
			
			try
			{
				initCrypto(); // do not relocate! - srpSessionKey must be valid before calling
				
				FILE_LOG(logDEBUG4) << "initCrypto OK";
				
				if (processEncrpytedDatagram(sequenceNumber, encrpytedMessage, encryptedResponse) == false)
				{
					FILE_LOG(logDEBUG4) << "processEncrpytedDatagram() failed (resteting state)";
					resetState();
					return false;
				}
				
				// TODO: Fix error code handling
				ret = enc_construct_R_message(encryptedResponse, 0, response);
			}
			catch (DragonSRP::DsrpException &e)
			{
				FILE_LOG(logWARNING) << "Session::processDatagram(): initCrypto() failed DsrpException (resetting state)" << e.what();
				resetState();
				return false;
			}
			catch (...)
			{
				FILE_LOG(logWARNING) << "Session::processDatagram(): initCrypto() failed (some exception) (resetting state)";
				resetState();
				return false;
			}
			
			return true; // SUCCESS
			break;
		case SH_SRV_STATE_FAULT:
		default:
			break;
	} // end of swith
	
	return true;
}

bool Session::processEncrpytedDatagram(SAN_UINT64 sequenceNumber, const San2::Utils::bytes& encrpytedMessage, San2::Utils::bytes& encrpytedResponse)
{	
	unsigned int encpacketLen, decpacketLen;
	San2::Utils::bytes applicationRequest, applicationResponse;
	
	if (encrpytedMessage.size() == 0) return false;
	
	applicationRequest.resize(m_dec->getOverheadLen() + SH_MAX_MSGLEN);
	m_dec->decryptAndVerifyMac(&encrpytedMessage[0], encrpytedMessage.size(), &applicationRequest[0], &decpacketLen, sequenceNumber);
	applicationRequest.resize(decpacketLen);
	
	if (processApplicationDatagram(sequenceNumber, applicationRequest, applicationResponse) == false) return false;
	if (applicationResponse.size() == 0) return false;
	
	// Now encrypt applicationResponse
	encrpytedResponse.resize(m_enc->getOverheadLen() + SH_MAX_MSGLEN);
	m_enc->encryptAndAuthenticate((unsigned char *)&applicationResponse[0], applicationResponse.size(), sequenceNumber, &encrpytedResponse[0], &encpacketLen); // throws
	encrpytedResponse.resize(encpacketLen);
	
	return true;
}

// applicationReponse cannot be empty at function return
bool Session::processApplicationDatagram(SAN_UINT64 sequenceNumber, const San2::Utils::bytes& applicationRequest, San2::Utils::bytes& applicationResponse)
{
	std::string strRequest = San2::Utils::bytes::bytes2string(applicationRequest);
	std::string result;
	
	if (strRequest.compare("help") == 0)
	{
		applicationResponse = "peers | uptime | version | hostname";
	}
	
	
	if (strRequest.compare("hostname") == 0)
	{
		if (m_connector.getParameter("hostname", result) != 0)
		{
			applicationResponse = "Shell server could not received hostname";
		}
	}
	
	if (strRequest.compare("uptime") == 0)
	{
		if (m_connector.getParameter("uptime", result) != 0)
		{
			applicationResponse = "Shell server could not receive uptime";
		}
	}
	
	if (strRequest.compare("version") == 0)
	{
		if (m_connector.getParameter("version", result) != 0)
		{
			applicationResponse = "Shell server could not receive SAN Node version";
		}
		
		result += "\nShell server version 1.0";
		applicationResponse = result;
	}
	
	if (strRequest.compare("peers") == 0)
	{
		if (m_connector.getParameter("peers", result) != 0)
		{
			applicationResponse = "Shell server could not received peer addresses";
		}
	}
	
	if (result.size() == 0) result = "Unknown command";
	applicationResponse = result;
	return true;
}

int Session::getState()
{
	return m_state;
}

void Session::resetState()
{
	m_state = SH_SRV_STATE_ZERO;
	m_srpUsername.clear();
    m_srpA.clear();	
    m_sessionK.clear();
    
    if (m_enc != NULL)
    {
		delete m_enc;
		m_enc = NULL;
	} 
	
	if (m_dec != NULL)
    {
		delete m_dec;
		m_dec = NULL;
	} 
}

void Session::initCrypto() // throws DsrpExpception
{
	if (m_enc != NULL && m_dec == NULL) throw DragonSRP::DsrpException("Session::initCrypto(): fatal");
	if (m_enc == NULL && m_dec != NULL) throw DragonSRP::DsrpException("Session::initCrypto(): fatal");
	
	if (m_enc != NULL) return; // already initialized
		
	DragonSRP::HashKeyDerivator keydrv(m_sessionK, SH_AES256_KEYLEN, SH_IVLEN, SH_SHA1_OUTPUTLEN);
	
	/*
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
	*/
		
	// UsesClient keys!
	m_enc =  new DragonSRP::DatagramEncryptor(keydrv.getServerEncryptionKey(), keydrv.getServerIV(), keydrv.getServerMacKey());
		
	// Uses server keys!
	m_dec =  new DragonSRP::DatagramDecryptor(keydrv.getClientEncryptionKey(), keydrv.getClientIV(), keydrv.getClientMacKey());
}


Session::~Session()
{
	if (m_enc != NULL)
    {
		delete m_enc;
		m_enc = NULL;
	} 
	
	if (m_dec != NULL)
    {
		delete m_dec;
		m_dec = NULL;
	} 
}
