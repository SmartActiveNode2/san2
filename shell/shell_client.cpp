


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

#define SH_CLI_ARQ_REPETITIONS 3
#define SH_CLI_ARQ_TIMEOUTMSEC 3000

int main(int argc, char *argv[])
{
	if (argc != 6)
	{
		printf("Usage:  ./shell_client <SanNodeApiAddress> <ShellServerSanAddress> <port> <user> <passwrod>\n");
		printf("Example:./shell_client /tmp/sanode1api FF21 2201 san smartactivenode\n");
		return -1;
	}
	
	char *apiAddress = argv[1];
	char *srvAddress = argv[2];
	char *srvPort    = argv[3];
	char *srpUser    = argv[4];
	char *srpPass    = argv[5];
	
	// Parse and check srvAddress format
	// we will prepend the address with zeroes to the correct address size length
	San2::Network::SanAddress srvSanAddress;
	if (San2::Utils::getExpandedAddress(std::string(srvAddress), srvSanAddress) == false)
	{
		printf("Invalid parameter provided: <ShellServerSanAddress>\n");
		return -1;
	}
		
	// Parse and check srvPort format
	SAN_UINT16 srvIntPort;
	if (sscanf(srvPort, "%u", &srvIntPort) != 1)
	{
		printf("Invalid parameter provided: <port>");
		return -1;
	}
	
	if (srvIntPort == 0)
	{
		printf("Port parameter must be a non zero value.\n");
		return -1;
	}
	
	
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
	San2::Api::CNodeConnector connector(apiAddress, 5000, 5000, 5000, 5000);
	
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
	
	StopWaitTx swtx(SH_CLI_ARQ_REPETITIONS, SH_CLI_ARQ_TIMEOUTMSEC, connector, srvSanAddress, srvIntPort, clientAddress, port);
	ClientSession cses(swtx);
	cses.run(srpUser, srpPass); // no threadining involved, just one thread in the whole program
	return 0;
}

