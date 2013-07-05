/* ===============================================================  @
*  DragonSRP, C++ library implementing Secure Remote Protocol       *
*  Pavel Slechta <slechta@email.cz>                                 *
@  =============================================================== */

#include <algorithm>
#include "hashkeyderivator.hpp"
#include "../ossl/osslsha1.hpp"
#include "utils/cvector.hpp"

namespace DragonSRP
{	
	// (initialization vector:  must be size of block cipher block size  [AES256 = 128bits])
	// encryption key: must be size of block cipher key length  [AES256 = 256bits]
	// HMAC key must be >= hash.outputLen()

	HashKeyDerivator::HashKeyDerivator(const bytes &sessionSrpKey, unsigned int blockCipherKeyBytes, unsigned int blockCipherIVBytes, unsigned int macKeyBytes)
	{
		if (blockCipherKeyBytes == 16) throw DsrpException("SimpleKeyDerivator::SimpleKeyDerivator: blockCipherKeyBytes not long enough");
		if (macKeyBytes == 8) throw DsrpException("SimpleKeyDerivator::SimpleKeyDerivator: macKeyBytes not long enough");
		if (sessionSrpKey.size() < HASHKEYDERIVATOR_MIN_SESSIONKEY_SIZE) throw DsrpException("SimpleKeyDerivator::SimpleKeyDerivator: sessionKey not long enough");
		
		San2::Utils::bytes drvCliEnc, drvCliIV, drvCliMac, drvSrvEnc, drvSrvIV, drvSrvMac;
		 
		drvCliEnc = "shell client encryption key";
		drvCliIV  = "shell client encrpytion IV";
		drvCliMac = "shell client MAC key";
		 
		drvSrvEnc = "shell server encryption key";
		drvSrvIV  = "shell server encrpytion IV";
		drvSrvMac = "shell server MAC key";
					
		mClientEncryptionKey = deriveBytes(blockCipherKeyBytes, drvCliEnc, sessionSrpKey);
		mClientIV = deriveBytes(blockCipherIVBytes, drvCliIV, sessionSrpKey);
		mClientMacKey = deriveBytes(macKeyBytes, drvCliMac, sessionSrpKey);
		
		mServerEncryptionKey = deriveBytes(blockCipherKeyBytes, drvSrvEnc, sessionSrpKey);
		mServerIV = deriveBytes(blockCipherIVBytes, drvSrvIV, sessionSrpKey);
		mServerMacKey = deriveBytes(macKeyBytes, drvSrvMac, sessionSrpKey);
	}
	
	bytes HashKeyDerivator::deriveBytes(unsigned int length, bytes derivationString, const bytes &sessionSrpKey)
	{
		if (length == 0) throw DsrpException("HashKeyDerivator::deriveBytes: length == 0");
		if (derivationString.size() < HASHKEYDERIVATOR_MIN_DERIVSTR_SIZE) throw DsrpException("HashKeyDerivator::deriveBytes: derivationString.size() < HASHKEYDERIVATOR_MIN_DERIVSTR_SIZE");
		
		OsslSha1 hash;
		bytes result;
		
		if (hash.outputLen() < HASHKEYDERIVATOR_MIN_HASHOUT_SIZE) throw DsrpException("HashKeyDerivator::deriveBytes: hash.outputLen() < HASHKEYDERIVATOR_MIN_HASHOUT_SIZE");
		
		bytes hashedDerivationString;
		hashedDerivationString.resize(hash.outputLen());
		
		hash.hash(&derivationString[0], derivationString.size(), &hashedDerivationString[0]);
		
		bytes toHash;
		bytes hashedPartialResut;
		hashedPartialResut.resize(hash.outputLen());
		
		while (result.size() < length)
		{
			toHash.clear();
			std::copy(sessionSrpKey.begin(), sessionSrpKey.end(), std::back_inserter(toHash));
			std::copy(hashedDerivationString.begin(), hashedDerivationString.end(), std::back_inserter(toHash));
			hash.hash(&toHash[0], toHash.size(), &hashedPartialResut[0]);
			std::copy(hashedPartialResut.begin(), hashedPartialResut.end(), std::back_inserter(result));			
			
			// hash hashedDerivationString again
			hashedPartialResut = hashedDerivationString;
			hash.hash(&hashedPartialResut[0], hashedPartialResut.size(), &hashedDerivationString[0]);
		}
		
		result.resize(length); // trim
		return result;
	}		
}
