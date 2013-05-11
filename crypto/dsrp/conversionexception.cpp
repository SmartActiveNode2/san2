/* ===============================================================  @
*  DragonSRP, C++ library implementing Secure Remote Protocol       *
*  Pavel Slechta <slechta@email.cz>                                 *
@  =============================================================== */

#include "conversionexception.hpp"

namespace DragonSRP
{
	ConversionException::ConversionException(const char *message) :
		DsrpException(message)
	{
		
	}
	
	ConversionException::ConversionException(std::string &message) :
		DsrpException(message)
	{
		
	}
}
