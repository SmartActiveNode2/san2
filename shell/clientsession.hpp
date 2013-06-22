#pragma once


#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "stopwaittx.hpp"

#include "crypto/dsrp/srpclient.hpp"
#include "crypto/dsrp/srpclientauthenticator.hpp"
#include "crypto/dsrp/user.hpp"
#include "crypto/dsrp/ng.hpp"

#include "crypto/dsrp/dsrpexception.hpp"
#include "crypto/dsrp/conversionexception.hpp"
#include "crypto/dsrp/usernotfoundexception.hpp"
#include "crypto/dsrp/conversion.hpp"

#include "crypto/ossl/osslsha1.hpp"
#include "crypto/ossl/osslmathimpl.hpp"
#include "crypto/ossl/osslrandom.hpp"

#include "protoconst.hpp"

class ClientSession
{
public:
	ClientSession(StopWaitTx &swtx);
	int run(std::string strUsername, std::string strPassword);
	
	bool processDecryptedDatagram(SAN_UINT64 sequenceNumber, const San2::Utils::bytes& request, San2::Utils::bytes& response);
private:
	StopWaitTx &m_swtx;
	
	DragonSRP::OsslSha1 hash;
	DragonSRP::OsslRandom random;
	DragonSRP::Ng ng;
	DragonSRP::Ossl::OsslMathImpl math;
	DragonSRP::SrpClient srpclient;
	DragonSRP::SrpClientAuthenticator sca;
};
