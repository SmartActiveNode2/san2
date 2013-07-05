/* ===============================================================  @
*  DragonSRP, C++ library implementing Secure Remote Protocol       *
*  Pavel Slechta <slechta@email.cz>                                 *
@  =============================================================== */

#pragma once

#include <vector>
#include "../dsrp/common.hpp"
#include "../dsrp/dsrpexception.hpp"
#include "../dsrp/hashinterface.hpp"

#define HASHKEYDERIVATOR_MIN_SESSIONKEY_SIZE 16
#define HASHKEYDERIVATOR_MIN_DERIVSTR_SIZE 8
#define HASHKEYDERIVATOR_MIN_HASHOUT_SIZE 8

namespace DragonSRP
{
	class HashKeyDerivator
	{
		public:
			// MacKey length must have at lest hash.blockLen() bytes !!!!!!
			HashKeyDerivator(const bytes &sessionSrpKey, unsigned int blockCipherKeyBytes, unsigned int blockCipherIVBytes, unsigned int macKeyBytes);
			
			bytes &getClientEncryptionKey(){return mClientEncryptionKey;}
			bytes &getClientIV(){return mClientIV;}
			bytes &getClientMacKey(){return mClientMacKey;}
			
			bytes &getServerEncryptionKey(){return mServerEncryptionKey;}
			bytes &getServerIV(){return mServerIV;}
			bytes &getServerMacKey(){return mServerMacKey;}
			
		private:
			bytes deriveBytes(unsigned int length, bytes derivationString, const bytes &sessionSrpKey);
		
			bytes mClientEncryptionKey;
			bytes mClientIV;
			bytes mClientMacKey;
			
			bytes mServerEncryptionKey;
			bytes mServerIV;
			bytes mServerMacKey;
			
			
	};
	
}
