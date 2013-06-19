
#include "clientsession.hpp"
#include "messageconstructor.hpp"

ClientSession::ClientSession(StopWaitTx &swtx) :
	m_state(SH_CLSESSION_STATE_ONE),
	m_swtx(swtx),
	ng(DragonSRP::Ng::predefined(SH_CLIENT_SRP_KEYPAIR)),
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
	
	switch(m_state)
	{
		case SH_CLSESSION_STATE_ONE:
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
			
			m_swtx.sendReliableMessage(request, response);
			break;
		default:
			printf("ClientSession Invalid state\n");
			return -1;
			break;
	}
}

bool ClientSession::processDecryptedDatagram(SAN_UINT64 sequenceNumber, const San2::Utils::bytes& request, San2::Utils::bytes& response)
{
	return true;
}
