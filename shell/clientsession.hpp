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

#include "crypto/dsrp/common.hpp"
#include "crypto/dsrp/dsrpexception.hpp"
#include "crypto/drel/datagramencryptor.hpp"
#include "crypto/drel/datagramdecryptor.hpp"
#include "crypto/drel/hashkeyderivator.hpp"
#include "crypto/drel/aesexception.hpp"
#include "crypto/dsrp/conversion.hpp"
#include "constants.hpp"
#include "protoconst.hpp"


class ClientSession
{
public:
	ClientSession(StopWaitTx &swtx);
	int run(std::string strUsername, std::string strPassword);
private:
	int getShellServerResponse(DragonSRP::DatagramEncryptor& encryptor, DragonSRP::DatagramDecryptor& decryptor, const San2::Utils::bytes& shellRequest, San2::Utils::bytes& shellResponse);

	StopWaitTx &m_swtx;
	
	DragonSRP::OsslSha1 hash;
	DragonSRP::OsslRandom random;
	DragonSRP::Ng ng;
	DragonSRP::Ossl::OsslMathImpl math;
	DragonSRP::SrpClient srpclient;
	DragonSRP::SrpClientAuthenticator sca;
};
