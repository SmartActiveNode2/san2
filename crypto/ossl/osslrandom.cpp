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
#include <openssl/rand.h>

#include "../dsrp/conversion.hpp"
#include "osslrandom.hpp"

namespace DragonSRP
{
	OsslRandom::OsslRandom() :
		initOk(false)
	{
		FILE *fp = 0;
		unsigned char buff[128];

		fp = fopen("/dev/urandom", "r");
        
		if (fp)
		{
			size_t count = fread(buff, sizeof(unsigned char), sizeof(buff), fp);
			if (count != sizeof(buff)) throw DsrpException("Could not initialize random number generator - small seed");
			fclose(fp);
		}
		else throw DsrpException("Could not initialize random number generator");
	
		RAND_seed(buff, sizeof(buff));
		initOk = true;
	}
	
	bytes OsslRandom::getRandom(unsigned int lenBytes)
	{
		if (!initOk) throw DsrpException("Could not get random number - PRNG not initialized");
		if (lenBytes <= 0) throw DsrpException("Could not get random number - size is zero or negative");
		unsigned char *r = (unsigned char *) malloc(lenBytes);
		if (r == NULL) throw DsrpException("Could not get random number - malloc() failed");
		int rval = RAND_bytes(r, lenBytes);
		
		if (rval != 1)
		{
			free(r);
			throw DsrpException("Could not get random number");
		}
		
		bytes out = Conversion::array2bytes(r, lenBytes);
		free(r);
		return out;
	}
	
}
