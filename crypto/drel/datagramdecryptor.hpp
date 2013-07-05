
#ifndef DREL_DATAGRAMDECRYPTOR_HPP
#define DREL_DATAGRAMDECRYPTOR_HPP

#ifdef LINUX
	#include <sys/types.h>
	#include <inttypes.h>
#endif

#include "../mac/hmac.hpp"
#include "../dsrp/common.hpp"
#include "../dsrp/dsrpexception.hpp"
#include "../dsrp/hashinterface.hpp"
#include "../ossl/osslsha1.hpp"
#include "aescounter.hpp"
#include "encparams.hpp"
#include "simplekeyderivator.hpp"

// *--2-----8------------------12---*
// | LEN | SEQ | ENC(DATA) | DIGEST |
// *--------------------------------*
//             <--ENCRYPT-->
// <-----AUTHENTICATE------>

// LEN = sizeof(ENCRYPT(DATA))
// First encrypt and then the encrypted with associated data authenticate

namespace DragonSRP
{	
	class DatagramDecryptor
	{	
		public:
			DatagramDecryptor(const bytes &encryptionKey, const bytes &IV, const bytes &macKey);
			void decryptAndVerifyMac(const unsigned char *dataIn, unsigned int dataInLen, unsigned char *dataOut, unsigned int *dataOutLen, std::uint64_t expectedSequenceNumber); // throws
			unsigned int getOverheadLen();

		private:	
			AesCounter aesCtr;
			OsslSha1 sha1;
			Hmac hmac;
	};
}
#endif

