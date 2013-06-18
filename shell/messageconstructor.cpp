
#include <iterator>
#include <string>
#include <stdio.h>

#include "messageconstructor.hpp"
#include "utils/cvector.hpp"

#define ENC_PROTOCOL_ID 0x45
#define ENC_MAX_USERNAME 32
#define ENC_MAX_A 8192

// E + username + 0x00 + srpA
bool enc_construct_A_message(const std::string &username, const San2::Utils::bytes &srpA, San2::Utils::bytes &out)
{
	if (username.size() == 0) return false;
	if (username.size() > ENC_MAX_USERNAME) return false;
	
	if (srpA.size() == 0) return false;
	if (srpA.size() > 8192) return false;
	
	out.clear();
	out += ENC_PROTOCOL_ID;
	out += username;
	out += 0x00;
	out += srpA;

	return true;
}

bool enc_parse_A_message(const San2::Utils::bytes &messageIn, San2::Utils::bytes &username, San2::Utils::bytes &srpA)
{
	if (messageIn.size() < 4) return false; // SOF + usr + 0x00 + srpA .. min is 4
	if (messageIn[0] != ENC_PROTOCOL_ID) return false;
	
	San2::Utils::bytes::const_iterator iter = messageIn.cbegin();
	iter++;
	
	for (; iter <= messageIn.end(); iter++)
	{
		if (*iter == 0x00) break;
	}
	
	if (iter == messageIn.end()) return false;
	
	username.clear();
	std::copy(messageIn.cbegin() + 1 ,iter, std::back_inserter(username));
	
	srpA.clear();
	std::copy(++iter, messageIn.end(), std::back_inserter(srpA));
	
	return true;
}

bool enc_testA()
{
	std::string username = "a";
	San2::Utils::bytes srpA;
	srpA += 0x00;
	srpA += 0x00;
	srpA += 0x00;
	srpA += 0x33;
	srpA += 0x00;
	
	San2::Utils::bytes out;
	if (enc_construct_A_message(username, srpA, out) == false)
	{
		printf("enc_testA_failed (1)\n");
		return false;
	}
	
	San2::Utils::bytes username2b;
	San2::Utils::bytes srpA2;

	if (enc_parse_A_message(out, username2b, srpA2) == false)
	{
		printf("enc_testA_failed (2)\n");
		return false;
	}
	
	std::string username2;
	std::copy(username2b.begin(), username2b.end(), std::back_inserter(username2));
	
	std::cout << "username:" << username << "*" << std::endl;
	std::cout << "username2:" << username2 << "*" << std::endl;
	
	if (username != username2)
	{
		printf("enc_testA_failed (3) - username comparison failed\n");
		return false;
	}
	
	std::cout << "srpA: ";
	San2::Utils::bytes::printBytes(srpA);
	
	std::cout << std::endl << "srpA2:";
	San2::Utils::bytes::printBytes(srpA);	
	std::cout << std::endl;
	
	std::cout << "srpA.size():" << srpA.size() << std::endl;
	std::cout << "srpA2.size():" << srpA2.size() << std::endl;
	
	if (srpA != srpA2)
	{
		printf("enc_testA_failed (4) - A comparison failed\n");
		return false;
	}
	
	return true;
}
