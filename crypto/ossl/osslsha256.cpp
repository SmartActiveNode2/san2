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

#include "osslsha256.hpp"
#include "../dsrp/conversion.hpp"

namespace DragonSRP
{
	OsslSha256::~OsslSha256()
	{
		// empty
	}
	
	void OsslSha256::hash(const unsigned char *in, unsigned int inLen, unsigned char *out)
	{
		SHA256_CTX context;
		SHA256_Init(&context);
		SHA256_Update(&context, in, inLen);
		SHA256_Final(out, &context);
	}
	
	unsigned int OsslSha256::outputLen()
	{
		return SHA256_DIGEST_LENGTH;
	}
	
	unsigned int OsslSha256::blockSize()
	{
		return 64;
	}
}
