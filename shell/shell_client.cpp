


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>
#include <list>

#include "api/node/connector/cnodeconnector.hpp"
#include "network/ccapsule.hpp"
#include "utils/log.h"
#include "utils/platform/basictypes.hpp"
#include "utils/address.hpp"
#include "stopwaittx.hpp"

#include "messageconstructor.hpp"

#include "clientsession.hpp"

#define PIPENAME "/tmp/sanode1api"
#define SH_SRV_PORT 2201
#define SH_SRV_ADDRESS "000000000000000000000000000000000000000000000000000000000000FF21"
#define SH_CLI_ARQ_REPETITIONS 3
#define SH_CLI_ARQ_TIMEOUTMSEC 3000

int main(int argc, char *argv[])
{
	/*
	if (enc_testA()) printf("enc_testA FAIL\n");
	else printf("enc_testA success\n");
	
	if (enc_testP()) printf("enc_testP FAIL\n");
	else printf("enc_testP success\n");
	
	if (enc_testB()) printf("enc_testB FAIL\n");
	else printf("enc_testB success\n");
	
	if (enc_testQ()) printf("enc_testQ FAIL\n");
	else printf("enc_testQ success\n");
	
	if (enc_testC()) printf("enc_testC FAIL\n");
	else printf("enc_testC success\n");
	
	if (enc_testR()) printf("enc_testR FAIL\n");
	else printf("enc_testR success\n");
	
	if (enc_testD()) printf("enc_testD FAIL\n");
	else printf("enc_testD success\n");
	*/
	
	FILELog::ReportingLevel() = logDEBUG4;
	
	San2::Utils::bytes payload;
	San2::Utils::bytes serializedCapsule;
	San2::Network::CCapsule capsule;
	San2::Api::CNodeConnector connector(PIPENAME, 5000, 5000, 5000, 5000);
	
	if (connector.open() != San2::Cppl::ErrorCode::SUCCESS)
	{
		printf("connection failed\n");
		return -1;
	}

	if (connector.connect() == false)
	{
		printf("Failed to connect to local node API listener. (Is the server running?)\n");
		return -1;
	}
	
	SAN_UINT16 port = connector.getEphemeralPort();
	
	FILE_LOG(logDEBUG4) << "ephemeral port (client port): " << port;
	
	if (port == 0) return -1;
	
	San2::Network::SanAddress serverAddress;
	
	if (San2::Utils::string2address(SH_SRV_ADDRESS, serverAddress) != true)
	{
		FILE_LOG(logDEBUG4) << "failed to parse destination address";
		return -1;
	}
	
	San2::Network::SanAddress clientAddress;
	std::list<San2::Network::SanAddress> adrs;
	unsigned int addressCount = connector.getInterfaceAddresses(adrs);
	
	if (addressCount < 1)
	{
		printf("Could not determine source address. No interfaces found.\n");
		return -3;
	}
	
	clientAddress = *(adrs.cbegin());
	std::cout << "Using source address: " << San2::Utils::address2string(clientAddress) << std::endl;
	
	StopWaitTx swtx(SH_CLI_ARQ_REPETITIONS, SH_CLI_ARQ_TIMEOUTMSEC, connector, serverAddress, SH_SRV_PORT, clientAddress, port);
	
	ClientSession cses(swtx);
	cses.run("san", "smartactivenode");
	
	// go
	
	/*
	for (int k = 0; k < 10; k++)
	{
		payload.append("Ahoj, jak se mas?");
		San2::Utils::bytes response;
		swtx.sendReliableMessage(payload, response);
		
		// print result
		std::cout << "response:";
		San2::Utils::bytes::printBytesAsString(response);
		std::cout << std::endl;
	}
	*/
	 
	return 0;
}

