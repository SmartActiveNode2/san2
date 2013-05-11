/* ===============================================================  @
*  DragonSRP, C++ library implementing Secure Remote Protocol       *
*  Pavel Slechta <slechta@email.cz>                                 *
@  =============================================================== */

#ifndef DSRP_SIMPLEKEYDERIVATOR_HPP
#define DSRP_SIMPLEKEYDERIVATOR_HPP

#include <vector>
#include "../dsrp/common.hpp"
#include "../dsrp/dsrpexception.hpp"
#include "../dsrp/hashinterface.hpp"

namespace DragonSRP
{
	class SimpleKeyDerivator
	{
		public:
			// MacKey length must have at lest hash.blockLen() bytes !!!!!!
			SimpleKeyDerivator(const bytes &sessionSrpKey, unsigned int blockCipherKeyBytes, unsigned int blockCipherIVBytes, unsigned int macKeyBytes);
			
			bytes &getClientEncryptionKey(){return mClientEncryptionKey;}
			bytes &getClientIV(){return mClientIV;}
			bytes &getClientMacKey(){return mClientMacKey;}
			
			bytes &getServerEncryptionKey(){return mServerEncryptionKey;}
			bytes &getServerIV(){return mServerIV;}
			bytes &getServerMacKey(){return mServerMacKey;}
			
		private:
			bytes mClientEncryptionKey;
			bytes mClientIV;
			bytes mClientMacKey;
			
			bytes mServerEncryptionKey;
			bytes mServerIV;
			bytes mServerMacKey;
			
			
	};
	
}

#endif
