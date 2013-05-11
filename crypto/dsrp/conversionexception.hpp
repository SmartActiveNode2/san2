/* ===============================================================  @
*  DragonSRP, C++ library implementing Secure Remote Protocol       *
*  Pavel Slechta <slechta@email.cz>                                 *
@  =============================================================== */

#ifndef DSRP_CONVERSIONEXCEPTION_HPP
#define DSRP_CONVERSIONEXCEPTION_HPP

#include "common.hpp"
#include "dsrpexception.hpp"

namespace DragonSRP
{
	class ConversionException : public DsrpException
	{
		public:
			ConversionException(const char *message);
			ConversionException(std::string &message);
	};
	
}

#endif
