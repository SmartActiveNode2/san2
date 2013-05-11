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


#ifndef DSRP_OSSL_CONVERSION_HPP
#define DSRP_OSSL_CONVERSION_HPP

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <string>
#include <openssl/bn.h>
#include "../dsrp/common.hpp"
#include "../dsrp/conversionexception.hpp"

namespace DragonSRP
{
	class OsslConversion
	{
		public:
			static void bytes2bignum(const bytes &in, BIGNUM *out);
			static void bignum2bytes(BIGNUM *in, bytes& out);
			
			static void printBignum(const BIGNUM *in);
	};
}

#endif
