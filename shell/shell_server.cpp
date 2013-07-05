


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>
#include <memory>

#include "api/node/connector/cnodeconnector.hpp"
#include "network/ccapsule.hpp"
#include "utils/log.h"
#include "utils/platform/basictypes.hpp"
#include "utils/address.hpp"
#include "utils/cvector.hpp"
#include "session.hpp"
#include "sessionmanager.hpp"


#define PIPENAME "/tmp/sanode2api"

#define APPLICATION_PORT 2201
#define SH_SRV_SESSION_TIMEOUT 3600

int main(int argc, char *argv[])
{
	FILELog::ReportingLevel() = logDEBUG4;
	San2::Utils::bytes serializedCapsule;
	
	SAN_UINT16 fromPort, toPort;
	San2::Utils::bytes payload;
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
	
	if (connector.registerPort(APPLICATION_PORT) == true)
	{
		printf("port register: OK\n");
	}
	else
	{
		printf("port register: FAILURE\n");
		return -1;
	}
	
	San2::Network::SanAddress serverAddress;
	std::list<San2::Network::SanAddress> adrs;
	
	unsigned int addressCount = connector.getInterfaceAddresses(adrs);	
	
	if (addressCount < 1)
	{
		printf("Could not determine source address. No interfaces found.\n");
		return -3;
	}
	
	serverAddress = *(adrs.cbegin());
	std::cout << "Using source address (serverAddress): " << San2::Utils::address2string(serverAddress) << std::endl;
	
	SessionManager<Session> sman([serverAddress, &connector](const San2::Network::SanAddress& clientAddress, SAN_UINT16 clientPort){return new Session(connector, serverAddress, APPLICATION_PORT, clientAddress, clientPort);}, SH_SRV_SESSION_TIMEOUT);
	
	while(1)
	{
		bool portsok;
		
		San2::Network::CCapsule rxcapsule;
		San2::Network::CCapsule txcapsule;
		std::shared_ptr<Session> ses;
		
		printf("awaiting capsule\n");
		SAN_INT32 rval = connector.waitForCapsule(rxcapsule, 50000);
		printf("i have got a capsule\n");
		
		San2::Utils::bytes data;
		
		switch(rval)
		{
			case SAN2_WAITFORCAPSULE_SUCCESS:
				portsok = rxcapsule.getPortsDS(toPort, fromPort);
				printf("SRV #1\n");
				ses = sman.getSession(rxcapsule.getSourceAddress(), fromPort);
				
				printf("SRV #2\n");
				if (portsok)
				{
					printf("got: rxcapsule, from port %ud", fromPort);
					std::cout << " from address " << San2::Utils::address2string(rxcapsule.getSourceAddress()) << std::endl;
					std::cout << "   to address " << San2::Utils::address2string(rxcapsule.getDestinationAddress()) << std::endl;
				}
				
				printf("SRV #3\n");
				
				if (ses->incommingCapsule(rxcapsule) == false)
				{
					printf("Failed to generate response\n");
					return -6;
				}
				
				printf("SRV #4\n");
				break;
			case SAN2_WAITFORCAPSULE_TIMEOUT:
				printf("got: timeout\n");
				break;
			case SAN2_WAITFORCAPSULE_ERROR_INVALID_RESPONSE:
				printf("got: error invalid response\n");
				break;	
			case SAN2_WAITFORCAPSULE_ERROR_CAPSULE_UNPACK:
				printf("got: error capsule unpack\n");
				break;	
			case SAN2_WAITFORCAPSULE_ERROR_INVOKE_FAILED:	
				printf("got: error INVOKE FAILED\n");
				break;	
			case SAN2_WAITFORCAPSULE_ERROR:
				printf("got: GENERAL ERROR\n");
				break;	
			default:
				printf("got: UNKNOWN ERROR\n");
				break;
		}
	}
	
	return 0;
}
