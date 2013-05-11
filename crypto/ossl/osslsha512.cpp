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

#include "osslsha512.hpp"
#include "../dsrp/conversion.hpp"

namespace DragonSRP
{
	OsslSha512::~OsslSha512()
	{
		// empty
	}
	
	void OsslSha512::hash(const unsigned char *in, unsigned int inLen, unsigned char *out)
	{
		SHA512_CTX context;
		SHA512_Init(&context);
		SHA512_Update(&context, in, inLen);
		SHA512_Final(out, &context);
	}
	
	unsigned int OsslSha512::outputLen()
	{
		return SHA512_DIGEST_LENGTH;
	}
	
	unsigned int OsslSha512::blockSize()
	{
		return 128;
	}
}
