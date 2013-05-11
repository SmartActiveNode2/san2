/* ===============================================================  @
*  DragonSRP, C++ library implementing Secure Remote Protocol       *
*  Pavel Slechta <slechta@email.cz>                                 *
@  =============================================================== */

#include "macexception.hpp"

namespace DragonSRP
{
	MacException::MacException(const char *message) :
		DsrpException(message)
	{
		
	}
	
	MacException::MacException(std::string &message) :
		DsrpException(message)
	{
		
	}
}
