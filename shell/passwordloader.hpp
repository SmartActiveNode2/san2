
//#pragma once

#include <string>
#include "../crypto/dsrp/lookupinterface.hpp"
#include "../crypto/dsrp/user.hpp"
#include "../crypto/dsrp/memorylookup.hpp"

class PasswordLoader : public DragonSRP::MemoryLookup
{		
public:	
	virtual ~PasswordLoader();
	int loadFromFile(std::string filename);
	
	
};
