/* ===============================================================  @
*  DragonSRP, C++ library implementing Secure Remote Protocol       *
*  Pavel Slechta <slechta@email.cz>                                 *
@  =============================================================== */

#ifndef DSRP_MACEXCEPTION_HPP
#define DSRP_MACEXCEPTION_HPP

#include "../dsrp/common.hpp"
#include "../dsrp/dsrpexception.hpp"

namespace DragonSRP
{
	class MacException : public DsrpException
	{
		public:
			MacException(const char *message);
			MacException(std::string &message);
	};
	
}

#endif
