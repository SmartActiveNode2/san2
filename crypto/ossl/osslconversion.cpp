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

#include <iostream>
#include "osslconversion.hpp"

namespace DragonSRP
{
	void OsslConversion::bytes2bignum(const bytes &in, BIGNUM *out)
	{
		BN_bin2bn(&in[0], in.size(), out);
	}
	
	void OsslConversion::bignum2bytes(BIGNUM *in, bytes& out)
	{
		out.resize(BN_num_bytes(in));
		BN_bn2bin(in, &out[0]); 
	}
	
	void OsslConversion::printBignum(const BIGNUM *in)
	{
		char *str = BN_bn2hex(in);
		std::cout << str << std::endl;
		//OPENSSL_free(str);
		free(str); // ugly!
	}
}
