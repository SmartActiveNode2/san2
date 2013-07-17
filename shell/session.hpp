#pragma once

#include <memory>
#include <utils/cvector.hpp>

#include "network/nettypedef.hpp"
#include "stopwaitrx.hpp"
#include "api/node/connector/cnodeconnector.hpp"

#include "crypto/dsrp/srpserver.hpp"
#include "crypto/dsrp/srpverificator.hpp"
#include "crypto/dsrp/user.hpp"
#include "crypto/dsrp/ng.hpp"

#include "crypto/dsrp/dsrpexception.hpp"
#include "crypto/dsrp/conversionexception.hpp"
#include "crypto/dsrp/usernotfoundexception.hpp"
#include "crypto/dsrp/conversion.hpp"

#include "crypto/dsrp/memorylookup.hpp"

#include "crypto/ossl/osslsha1.hpp"
#include "crypto/ossl/osslmathimpl.hpp"
#include "crypto/ossl/osslrandom.hpp"

#include "protoconst.hpp"

#include "crypto/drel/datagramencryptor.hpp"
#include "crypto/drel/datagramdecryptor.hpp"

// initial state
#define SH_SRV_STATE_ZERO 0

// received C, A
#define SH_SRV_STATE_ALPHA 1

// received valid M1
#define SH_SRV_STATE_BETA 2

//	#define STATE_GAMMA 3

// irrecoverable state
#define SH_SRV_STATE_FAULT -1

// keypair: 1024; username: san; password: smartactivenode
#define SH_TESTSRP_USERNAME "san"
#define SH_TESTSRP_SALT "69FA85752CE7A22B7D9BF6368833FE1932299CC07D547C0BA9E999E33451BFE4"
#define SH_TESTSRP_VERIFICATOR "8E27B4D1467FBF5A53F7C30CE40871EEEBE6C1ACF0F67C738154D165876277DFB342D81B6FECF6793D79CCF364F9815ABBC38B3693DD3053D65C17D5A4FC4E6A92F6A72CAA617872FC8A34500A98DFDBCC2B1A3D1B66B6B7B3696A48106789C09B09EE596C313B6EB05110AF8A2AB449073ED597E8569C9FEF282917A482F4A4"


class Session : public StopWaitRx
{
public:
	Session(San2::Api::CNodeConnector &connector, const San2::Network::SanAddress& serverAddress, SAN_UINT16 serverPort, const San2::Network::SanAddress& clientAddress, SAN_UINT16 clientPort);
	
	bool processDatagram(SAN_UINT64 sequenceNumber, const San2::Utils::bytes& request, San2::Utils::bytes& response);
	bool processEncrpytedDatagram(SAN_UINT64 sequenceNumber, const San2::Utils::bytes& request, San2::Utils::bytes& encrpytedResponse);
	bool processApplicationDatagram(SAN_UINT64 sequenceNumber, const San2::Utils::bytes& applicationRequest, San2::Utils::bytes& applicationResponse);
	
	int getState();
	
	void resetState();
	~Session(); // do not call directly
protected:

private:
	San2::Api::CNodeConnector &m_connector;
	unsigned int m_secondsValid;
	
	// session authenticated state
	San2::Utils::bytes m_srpUsername;
	San2::Utils::bytes m_srpA;	
	San2::Utils::bytes m_sessionK;
	
	// Finite state machine state
	int m_state;
	
	// time lastActive
	// source addres, source port
	
	//SRP
	DragonSRP::OsslSha1 hash; // We will use OpenSSL SHA1 implementation
	DragonSRP::OsslRandom random; // We will use OpenSSL random number generator
	DragonSRP::MemoryLookup lookup; // This stores users in memory (linked-list)
	DragonSRP::Ng ng;
	DragonSRP::Ossl::OsslMathImpl math;
	DragonSRP::SrpServer srpserver;
	DragonSRP::SrpVerificator ver;
	
	DragonSRP::DatagramEncryptor *m_enc;
	DragonSRP::DatagramDecryptor *m_dec;


	void initCrypto(); // must be always private, called only by processDatagram(). Must be called in precise moment.
};
