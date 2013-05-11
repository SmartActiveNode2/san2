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
#include <openssl/sha.h>

#include "osslsha1.hpp"
#include "../dsrp/conversion.hpp"

namespace DragonSRP
{
	OsslSha1::~OsslSha1()
	{
		// empty
	}
	
	void OsslSha1::hash(const unsigned char *in, unsigned int inLen, unsigned char *out)
	{
		SHA_CTX context;
		SHA1_Init(&context);
		SHA1_Update(&context, in, inLen);
		SHA1_Final(out, &context);
	}
	
	unsigned int OsslSha1::outputLen()
	{
		return SHA_DIGEST_LENGTH;
	}
	
	unsigned int OsslSha1::blockSize()
	{
		return 64;
	}
}
