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

#ifndef DSRP_OSSL_MD5_HPP
#define DSRP_OSSL_MD5_HPP

#include "../dsrp/common.hpp"
#include "../dsrp/hashinterface.hpp"

namespace DragonSRP
{
	class OsslMd5 : public HashInterface
	{
		public:
			~OsslMd5();
			void hash(const unsigned char *in, unsigned int inLen, unsigned char *out);
			unsigned int outputLen();
			unsigned int blockSize();
	};
}


#endif
