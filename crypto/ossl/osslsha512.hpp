/* ===============================================================  @
*  DragonSRP, C++ library implementing Secure Remote Protocol       *
*  Pavel Slechta <slechta@email.cz>                                 *
@  =============================================================== */

/* ===============================================================  @
*  This product includes software developed by the OpenSSL Project  *
*  for use in the OpenSSL Toolkit. (http://www.openssl.org/)        *
*                                                                   *
*  This product includes cryptographic software                     *
*  written by Eric Young (eay@cryptsoft.com)                        *
*                                                                   *
*  This product includes software                                   *
*  written by Tim Hudson (tjh@cryptsoft.com)                        *
@  =============================================================== */


#ifndef DSRP_OSSL_SHA512_HPP
#define DSRP_OSSL_SHA512_HPP

#include "../dsrp/common.hpp"
#include "../dsrp/hashinterface.hpp"

namespace DragonSRP
{
	class OsslSha512 : public HashInterface
	{
		public:
			~OsslSha512();
			void hash(const unsigned char *in, unsigned int inLen, unsigned char *out);
			unsigned int outputLen();
			unsigned int blockSize();
	};
}


#endif
