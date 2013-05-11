/* ===============================================================  @
*  DragonSRP, C++ library implementing Secure Remote Protocol       *
*  Pavel Slechta <slechta@email.cz>                                 *
@  =============================================================== */

#include <stdio.h>
#include <iterator>
#include "memorylookup.hpp"
#include "user.hpp"
#include "usernotfoundexception.hpp"

namespace DragonSRP
{
	class User;
	
	User MemoryLookup::getByName(bytes username)
	{
		std::map<bytes, User>::iterator iter;
		iter = db.find(username);
		if (iter == db.end()) throw UserNotFoundException("User not found.", username);
		return iter->second;
	}
	
	bool MemoryLookup::userAdd(User user)
	{
		std::pair<std::map<bytes, User>::iterator, bool> ret = db.insert(make_pair(user.getUsername(), user));
		return ret.second;
	}
}
