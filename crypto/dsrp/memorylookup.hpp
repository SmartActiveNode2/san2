/* ===============================================================  @
*  DragonSRP, C++ library implementing Secure Remote Protocol       *
*  Pavel Slechta <slechta@email.cz>                                 *
@  =============================================================== */

#ifndef DSRP_MEMORYLOOKUP_HPP
#define DSRP_MEMORYLOOKUP_HPP

#include <map>
#include "common.hpp"
#include "lookupinterface.hpp"
#include "user.hpp"

namespace DragonSRP
{
	class MemoryLookup : public LookupInterface
	{
		public:
			User getByName(bytes username); // throws DsrpException if user not found
			bool userAdd(User user); // returns true on success
			void clearDb();
		private:
			std::map<bytes, User> db;
	};
}

#endif
