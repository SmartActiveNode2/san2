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


#ifndef DSRP_OSSLRANDOM_HPP
#define DSRP_OSSLRANDOM_HPP

#include "../dsrp/common.hpp"
#include "../dsrp/randominterface.hpp"
#include "../dsrp/dsrpexception.hpp"

namespace DragonSRP
{
	class OsslRandom : public RandomInterface
	{
		public:
			OsslRandom();
			bytes getRandom(unsigned int lenBytes);
		private:
			FILE *fp;
			bool initOk;
	};
}

#endif
