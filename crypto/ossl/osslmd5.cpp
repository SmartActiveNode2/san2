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

#include <stdio.h>
#include <stdlib.h>
#include <openssl/md5.h>

#include "osslmd5.hpp"
#include "../dsrp/conversion.hpp"

namespace DragonSRP
{
	OsslMd5::~OsslMd5()
	{
		// empty
	}
	
	void OsslMd5::hash(const unsigned char *in, unsigned int inLen, unsigned char *out)
	{
		MD5_CTX context;
		MD5_Init(&context);
		MD5_Update(&context, in, inLen);
		MD5_Final(out, &context);
	}
	
	unsigned int OsslMd5::outputLen()
	{
		return MD5_DIGEST_LENGTH;
	}
	
	unsigned int OsslMd5::blockSize()
	{
		return 64;
	}
}
