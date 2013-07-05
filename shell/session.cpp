
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

Session::Session(San2::Api::CNodeConnector &connector, const San2::Network::SanAddress& serverAddress, SAN_UINT16 serverPort, const San2::Network::SanAddress& clientAddress, SAN_UINT16 clientPort) :
	StopWaitRx(connector, serverAddress, serverPort, clientAddress, clientPort),
	m_state(SH_SRV_STATE_ZERO),
	ng(DragonSRP::Ng::predefined(SH_SRP_KEYPAIR)),
	math(hash, ng),
	srpserver(lookup, math, random, true),
	m_enc(NULL),
	m_dec(NULL)
{
	// setup test user in the memory database
	DragonSRP::bytes username = DragonSRP::Conversion::string2bytes(SH_TESTSRP_USERNAME);
	DragonSRP::bytes verificator = DragonSRP::Conversion::hexstring2bytes(SH_TESTSRP_VERIFICATOR);
	DragonSRP::bytes salt = DragonSRP::Conversion::hexstring2bytes(SH_TESTSRP_SALT);
		
	DragonSRP::User u(username, verificator, salt);
		
	if (!lookup.userAdd(u)) // May throw ConversionException
	{
		FILE_LOG(logERROR) << "Error: user already exists";
	}
}

bool Session::processDatagram(SAN_UINT64 sequenceNumber, const San2::Utils::bytes& request, San2::Utils::bytes& response)
{
	FILE_LOG(logDEBUG4) << "Session::processDatagram(): checkpoint #1";
	
	San2::Utils::bytes currentM1;
	San2::Utils::bytes currentM2;
	DragonSRP::bytes currentSalt;
	DragonSRP::bytes currentB;
	San2::Utils::bytes encrpytedMessage;
	
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
			
			std::cout << "Shared session key: ";
			San2::Utils::bytes::printBytes(m_sessionK);
			std::cout << std::endl;
			
			// ok, change state
			m_state = SH_SRV_STATE_BETA;
			break;
			
		case SH_SRV_STATE_BETA:
			std::cout << "Yes I am in BETA state :)" << std::endl;
			
			ret = enc_parse_C_message(request, encrpytedMessage);
			if (ret)
			{
				FILE_LOG(logDEBUG4) << "Session::processDatagram(): enc_parse_C_message failed (resteting state): " << ret;
				resetState();
				return false;
			}
			
			try
			{
				initCrypto();
				
				if (processEncrpytedDatagram(sequenceNumber, request, response) == false)
				{
					FILE_LOG(logDEBUG4) << "Session::processDatagram(): processEncrpytedDatagram() failed (resteting state)";
					resetState();
					return false;
				}	
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
	FILE_LOG(logDEBUG4) <<  "Received encryptedDatagram";
	
	// Now we can decrypt the message

	unsigned int decpacketLen;
	unsigned char decpacket[m_dec->getOverheadLen() + SH_MAX_MSGLEN];
		
	m_dec->decryptAndVerifyMac(&encrpytedMessage[0], encrpytedMessage.size(), decpacket, &decpacketLen, sequenceNumber);
	
	std::cout << "decpacket: ";
	DragonSRP::Conversion::printHex(decpacket, decpacketLen);
	std::cout << std::endl;
	
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
