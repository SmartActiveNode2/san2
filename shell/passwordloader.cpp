
#include <fstream>
#include <string>
#include <stdio.h>
#include "passwordloader.hpp"
#include "utils/cvector.hpp"
#include "utils/cstringutils.hpp"
#include "utils/hex.hpp"

PasswordLoader::~PasswordLoader()
{
	
}

int PasswordLoader::loadFromFile(std::string filename)
{
	int userCount = 0;
	clearDb();
	std::ifstream infile(filename);

	if (!infile.good()) return -1;
	
	std::string line;
	
	char delim = 10;
	while (std::getline(infile, line, delim)) // LF
	{	
		line = San2::Utils::CStringUtils::trim(line);
		
		if (line.length() == 0) continue;
		if (line.length() >= 1)
		{
			if (line[0] == '#')
			{
				continue; // skip comment line
			}
		}
			
		std::istringstream iss(line);
		
		std::string srpUser, srpSalt, srpVerificator;
		
		if (!(iss >> srpUser >> srpSalt >> srpVerificator))
		{
			return -2;  // error, wrong file format
		}

		San2::Utils::bytes bSrpUser, bSrpSalt, bSrpVerificator;
		
		bSrpUser = srpUser; // Doed that include \0 or not? \0 must not be at the end!
	
		if (San2::Utils::hexToBytes(srpSalt, bSrpSalt) == false)
		{
			return -3;
		}
		
		if (San2::Utils::hexToBytes(srpVerificator, bSrpVerificator) == false)
		{
			return -4;
		}
		
		DragonSRP::User usr(bSrpUser, bSrpVerificator, bSrpSalt);
		if (userAdd(usr)) // duplicit usernames will not be inserted, only first user inserted
		{
			userCount++;
		}
	}
	
	return userCount;
}

