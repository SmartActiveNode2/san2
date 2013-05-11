/* ===============================================================  @
*  DragonSRP, C++ library implementing Secure Remote Protocol       *
*  Pavel Slechta <slechta@email.cz>                                 *
@  =============================================================== */


#include "usernotfoundexception.hpp"

namespace DragonSRP
{
	UserNotFoundException::UserNotFoundException(const char *message, const bytes &userName) :
		DsrpException(message),
		username(userName)
	{
		
	}
	
	UserNotFoundException::UserNotFoundException(std::string &message, const bytes &userName) :
		DsrpException(message),
		username(userName)
	{
		
	}
	
	bytes UserNotFoundException::getUsername()
	{
		return username;
	}
}
