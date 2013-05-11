/* ===============================================================  @
*  DragonSRP, C++ library implementing Secure Remote Protocol       *
*  Pavel Slechta <slechta@email.cz>                                 *
@  =============================================================== */

#ifndef DSRP_HASHINTERFACE_HPP
#define DSRP_HASHINTERFACE_HPP

#include "common.hpp"

namespace DragonSRP
{
	class HashInterface
	{
		public:
			virtual ~HashInterface() = 0;
			virtual void hash(const unsigned char *in, unsigned int inLen, unsigned char *out)=0;
			virtual unsigned int outputLen()=0;
			virtual unsigned int blockSize()=0;
	};
}

#endif
