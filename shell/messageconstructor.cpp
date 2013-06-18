
#include <iterator>
#include <string>
#include <stdio.h>

#include "messageconstructor.hpp"
#include "utils/cvector.hpp"
#include "utils/platform/basictypes.hpp"

// Protocol header constants
#define ENC_PROTOCOL_ID 0x45
#define ENC_PROTOCOL_INDEX 0
#define ENC_MSGTYPE_INDEX  1
#define ENC_ERROR_INDEX    2

// SOF + MSGTYPE
#define ENC_MIN_CLIENT_HEADER 2
#define ENC_MIN_SERVER_HEADER 3

// Minimum and maximum parameter sizes
#define ENC_MAX_USERNAME 32
#define ENC_MIN_USERNAME 1

#define ENC_MAX_SRPA 1024
#define ENC_MIN_SRPA 64

#define ENC_MAX_SRPB 1024
#define ENC_MIN_SRPB 64

#define ENC_MAX_SALT 64
#define ENC_MIN_SALT 16

#define ENC_MAX_M1 512
#define ENC_MIN_M1 16

#define ENC_MAX_M2 512
#define ENC_MIN_M2 16

// Message Type ID
#define ENC_MSGTYPE_A 0x41
#define ENC_MSGTYPE_B 0x42
#define ENC_MSGTYPE_C 0x43

#define ENC_MSGTYPE_P 0x50
#define ENC_MSGTYPE_Q 0x51
#define ENC_MSGTYPE_R 0x52

// E + username + 0x00 + srpA
int enc_construct_A_message(const std::string &username, const San2::Utils::bytes &srpA, San2::Utils::bytes &out)
{
	if (username.size() < ENC_MIN_USERNAME) return -1;
	if (username.size() > ENC_MAX_USERNAME) return -2;
	
	if (srpA.size() < ENC_MIN_SRPA) return -3;
	if (srpA.size() > ENC_MAX_SRPA) return -4;
	
	out.clear();
	out += ENC_PROTOCOL_ID;
	out += ENC_MSGTYPE_A;
	out += username;
	out += 0x00;
	out += srpA;

	return 0;
}

int enc_parse_A_message(const San2::Utils::bytes &messageIn, San2::Utils::bytes &username, San2::Utils::bytes &srpA)
{
	if (messageIn.size() < ENC_MIN_CLIENT_HEADER + ENC_MIN_USERNAME + ENC_MIN_SRPA) return -1; // SOF + usr + 0x00 + srpA .. min is 4
	if (messageIn[ENC_PROTOCOL_INDEX] != ENC_PROTOCOL_ID) return -2;
	if (messageIn[ENC_MSGTYPE_INDEX] != ENC_MSGTYPE_A) return -3;
	
	San2::Utils::bytes::const_iterator iter = messageIn.cbegin();
	iter += ENC_MIN_CLIENT_HEADER;
	
	for (; iter <= messageIn.end(); iter++)
	{
		if (*iter == 0x00) break;
	}
	
	if (iter == messageIn.end()) return -4;
	
	username.clear();
	std::copy(messageIn.cbegin() + ENC_MIN_CLIENT_HEADER, iter, std::back_inserter(username));
	
	srpA.clear();
	std::copy(++iter, messageIn.end(), std::back_inserter(srpA));
	
	return 0;
}

bool enc_testA()
{
	std::string username = "a";
	San2::Utils::bytes srpA;
	srpA += 0x00;
	srpA += 0x00;
	srpA += 0x00;
	srpA += 0x33;
	srpA.resize(ENC_MIN_SRPA, 0x00);
	
	San2::Utils::bytes out;
	int ret;
	ret = enc_construct_A_message(username, srpA, out);
	
	if (ret)
	{
		printf("enc_testA_failed (1): %d\n", ret);
		return false;
	}
	
	San2::Utils::bytes username2b;
	San2::Utils::bytes srpA2;

	ret = enc_parse_A_message(out, username2b, srpA2);
	if (ret)
	{
		printf("enc_testA_failed (2): %d\n", ret);
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

int enc_construct_P_message(unsigned char errorCode, const San2::Utils::bytes& salt, const San2::Utils::bytes& srpB, San2::Utils::bytes &out)
{
	if (salt.size() < ENC_MIN_SALT) return -1;
	if (salt.size() > ENC_MAX_SALT) return -2;
	
	if (srpB.size() < ENC_MIN_SRPB) return -3;
	if (srpB.size() > ENC_MAX_SRPB) return -4;
	
	if (salt.size() > 255) return -5;
	SAN_UINT8 saltSize = (SAN_UINT8) salt.size();
		
	out.clear();
	
	out.resize(ENC_MIN_SERVER_HEADER + sizeof(SAN_UINT8) + salt.size() + srpB.size());
	
	out[ENC_PROTOCOL_INDEX] = ENC_PROTOCOL_ID;
	out[ENC_MSGTYPE_INDEX] = ENC_MSGTYPE_P;
	out[ENC_ERROR_INDEX] = errorCode;
	out[ENC_ERROR_INDEX + 1] = (unsigned char) saltSize;
	
	San2::Utils::bytes::iterator iter = out.begin() + ENC_MIN_SERVER_HEADER + sizeof(SAN_UINT8);
	std::copy(salt.cbegin(), salt.cend(), iter);
	iter += saltSize;
	std::copy(srpB.cbegin(), srpB.cend(), iter);
	
	return 0;
}

int enc_parse_P_message(const San2::Utils::bytes &messageIn, San2::Utils::bytes &salt, San2::Utils::bytes &srpB, unsigned char& errorCode)
{
	if (messageIn.size() < ENC_MIN_SERVER_HEADER + sizeof(SAN_UINT8) + ENC_MIN_SALT + ENC_MIN_SRPB) return -1;
	if (messageIn[ENC_PROTOCOL_INDEX] != ENC_PROTOCOL_ID) return -2;
	if (messageIn[ENC_MSGTYPE_INDEX] != ENC_MSGTYPE_P) return -3;
	errorCode = messageIn[ENC_ERROR_INDEX];
	unsigned char saltSize = messageIn[ENC_ERROR_INDEX+1];
	
	if (saltSize < ENC_MIN_SALT || saltSize > ENC_MAX_SALT) return -4;
	
	San2::Utils::bytes::const_iterator iterSaltBegin = messageIn.cbegin() + ENC_MIN_SERVER_HEADER + sizeof(SAN_UINT8);
	San2::Utils::bytes::const_iterator iterSaltEnd = messageIn.cbegin() + ENC_MIN_SERVER_HEADER + sizeof(SAN_UINT8) + saltSize;
	
	salt.resize(saltSize);
	std::copy(iterSaltBegin, iterSaltEnd, salt.begin());
	
	std::size_t srpBSize = messageIn.size() - ENC_MIN_SERVER_HEADER - sizeof(SAN_UINT8) - saltSize;
	srpB.resize(srpBSize);
	std::copy(iterSaltEnd, messageIn.cend(), srpB.begin());
	
	if (salt.size() < ENC_MIN_SALT) return -5;
	if (salt.size() > ENC_MAX_SALT) return -6;
	
	if (srpB.size() < ENC_MIN_SRPB) return -7;
	if (srpB.size() > ENC_MAX_SRPB) return -8;
	
	return 0;	
}

int enc_testP()
{
	unsigned char errorCode = 23;
	unsigned char errorCode2;
	San2::Utils::bytes salt, salt2;
	San2::Utils::bytes srpB, srpB2;
	San2::Utils::bytes out;
	
	salt.resize(ENC_MIN_SALT, 0x45);
	srpB.resize(ENC_MIN_SRPB, 0x78);
	
	int ret;
	
	ret = enc_construct_P_message(errorCode, salt, srpB, out);
	
	if (ret)
	{
		printf("enc_testP(): enc_constuct_P_message_failed FAILED: %d\n", ret);
		return -1;
	}
	
	ret = enc_parse_P_message(out, salt2, srpB2, errorCode2);
	
	if (ret)
	{
		printf("enc_testP(): enc_parse_P_message_failed FAILED: %d\n", ret);
		return -2;
	}
	
	if (errorCode != errorCode2)
	{
		printf("enc_testP(): errorCode does not match FAILED\n");
		return -3;
	}
	
	if (salt != salt2)
	{
		printf("enc_testP(): salt does not match FAILED\n");
		return -4;
	}
	
	if (srpB != srpB2)	
	{
		printf("enc_testP(): srpB does not match FAILED\n");
		return -5;
	}
	
	printf("enc_test_P() OK\n");
	return 0;
}

int enc_construct_B_message(const San2::Utils::bytes& srpM1, San2::Utils::bytes &out)
{
	if (srpM1.size() < ENC_MIN_M1) return -1;
	if (srpM1.size() > ENC_MAX_M1) return -2;
	
	out.resize(ENC_MIN_CLIENT_HEADER + srpM1.size());
	
	out[ENC_PROTOCOL_INDEX] = ENC_PROTOCOL_ID;
	out[ENC_MSGTYPE_INDEX] = ENC_MSGTYPE_B;
	
	std::copy(srpM1.cbegin(), srpM1.cend(), out.begin() + ENC_MIN_CLIENT_HEADER);
	
	return 0;
}

int enc_parse_B_message(const San2::Utils::bytes &messageIn, San2::Utils::bytes &srpM1)
{
	if (messageIn.size() < ENC_MIN_CLIENT_HEADER + ENC_MIN_M1) return -1;
	if (messageIn[ENC_PROTOCOL_INDEX] != ENC_PROTOCOL_ID) return -2;
	if (messageIn[ENC_MSGTYPE_INDEX] != ENC_MSGTYPE_B) return -3;
	
	std::size_t m1size = messageIn.size() - ENC_MIN_CLIENT_HEADER;
	if (m1size < ENC_MIN_M1 || m1size > ENC_MAX_M1) return -4;
	
	srpM1.resize(m1size);
	std::copy(messageIn.cend() - m1size, messageIn.cend(), srpM1.begin());
	
	return 0;
}

int enc_testB()
{
	San2::Utils::bytes srpM1_1, srpM1_2;
	San2::Utils::bytes out;
	
	srpM1_1.resize(ENC_MIN_M1, 0x45);

	int ret;
	
	ret = enc_construct_B_message(srpM1_1, out);
	
	if (ret)
	{
		printf("enc_testB(): enc_constuct_B_message_failed FAILED: %d\n", ret);
		return -1;
	}
	
	ret = enc_parse_B_message(out, srpM1_2);
	
	if (ret)
	{
		printf("enc_testB(): enc_parse_B_message_failed FAILED: %d\n", ret);
		return -2;
	}
	
	if (srpM1_1 != srpM1_2)	
	{
		printf("enc_testB(): srpM1 does not match FAILED\n");
		return -3;
	}
	
	printf("enc_test_B() OK\n");
	return 0;
}

int enc_construct_Q_message_fail(San2::Utils::bytes &out, unsigned char errorCode)
{
	out.resize(ENC_MIN_SERVER_HEADER);	
	out[ENC_PROTOCOL_INDEX] = ENC_PROTOCOL_ID;
	out[ENC_MSGTYPE_INDEX] = ENC_MSGTYPE_Q;
	out[ENC_ERROR_INDEX] = errorCode;
	return 0;
}

int enc_construct_Q_message_success(const San2::Utils::bytes& srpM2, San2::Utils::bytes &out)
{
	if (srpM2.size() < ENC_MIN_M2) return -1;
	if (srpM2.size() > ENC_MAX_M2) return -2;
	out.resize(ENC_MIN_SERVER_HEADER + srpM2.size());
	
	out[ENC_PROTOCOL_INDEX] = ENC_PROTOCOL_ID;
	out[ENC_MSGTYPE_INDEX] = ENC_MSGTYPE_Q;
	out[ENC_ERROR_INDEX] = 0; // success
	
	std::copy(srpM2.cbegin(), srpM2.cend(), out.begin() + ENC_MIN_SERVER_HEADER);
	
	return 0;
}

int enc_parse_Q_message(const San2::Utils::bytes &messageIn, San2::Utils::bytes &srpM2, unsigned char& errorCode)
{
	if (messageIn.size() < ENC_MIN_SERVER_HEADER) return -1;
	if (messageIn[ENC_PROTOCOL_INDEX] != ENC_PROTOCOL_ID) return -2;
	if (messageIn[ENC_MSGTYPE_INDEX] != ENC_MSGTYPE_Q) return -3;
	errorCode = messageIn[ENC_ERROR_INDEX];
	
	if (errorCode != 0)
	{ 
		srpM2.clear();
		return 0;
	}
	
	if (messageIn.size() < ENC_MIN_SERVER_HEADER + ENC_MIN_M2) return -4;
	
	std::size_t m2size = messageIn.size() - ENC_MIN_SERVER_HEADER;
	
	srpM2.resize(m2size);
	std::copy(messageIn.cbegin() + ENC_MIN_SERVER_HEADER, messageIn.cend(), srpM2.begin());
	
	return 0;
}

int enc_testQ()
{
	San2::Utils::bytes srpM2_1, srpM2_2;
	San2::Utils::bytes out;
	
	srpM2_1.resize(ENC_MIN_M2, 0x45);

	int ret;
	
	ret = enc_construct_Q_message_success(srpM2_1, out);
	
	if (ret)
	{
		printf("enc_testQ(): enc_construct_Q_message_success FAILED: %d\n", ret);
		return -1;
	}
	
	unsigned char errorCodeSuccess;
	ret = enc_parse_Q_message(out, srpM2_2, errorCodeSuccess);
	
	if (ret)
	{
		printf("enc_testQ(): enc_parse_Q_message FAILED: %d\n", ret);
		return -2;
	}
	
	if (errorCodeSuccess != 0)
	{
		printf("enc_testQ(): errorCodeSuccess is non zero FAILED\n");
		return -3;
	}
	
	if (srpM2_1 != srpM2_2)	
	{
		printf("enc_testQ(): srpM2 does not match FAILED\n");
		return -4;
	}
	
	// second part
	unsigned char errorCode1 = 87;
	unsigned char errorCode2;
	ret = enc_construct_Q_message_fail(out, errorCode1);
	
	if (ret)
	{
		printf("enc_testQ(): enc_construct_Q_message_fail FAILED: %d\n", ret);
		return -5;
	}
	
	ret = enc_parse_Q_message(out, srpM2_2, errorCode2);
	
	if (ret)
	{
		printf("enc_testQ(): enc_parse_Q_message FAILED: %d\n", ret);
		return -5;
	}
	
	if (errorCode1 != errorCode2)
	{
		printf("errorCode1: %d\nerrorCode2: %d\n", errorCode1, errorCode2);
		printf("enc_testQ(): errorCode does not match FAILED\n");
		return -6;
	}
	
	printf("enc_test_Q() OK\n");
	return 0;
}

