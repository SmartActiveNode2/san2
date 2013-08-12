


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
#include "passwordloader.hpp"

#define SH_SRV_PASSWORD_FILE "pass.pwd"

#define APPLICATION_PORT 2201
#define SH_SRV_SESSION_TIMEOUT 3600

int main(int argc, char *argv[])
{
	if (argc < 2 || argc > 3)
	{
		printf("Usage:   ./shell_server <SanNodeApiAddress> [port=2201]\n");
		printf("Example: ./shell_server /tmp/sanode2api 2201\n");
		return -1;
	}
	
	
	SAN_UINT16 srvIntPort = APPLICATION_PORT;
	unsigned int tmpPort;
	
	
	char *srvApiAddress = argv[1]; 
	char *srvPort       = argv[2];
	
	if (argc == 3) 
	{
		if (sscanf(srvPort, "%u", &tmpPort) != 1)
		{
			printf("Invalid parameter provided: <port>");
			return -1;
		}
		
		if (tmpPort > 65535)
		{
			printf("Port parameter must be from 1 to 65535\n");
			return -1;
		}
		
		srvIntPort = tmpPort;
		
		if (srvIntPort == 0)
		{
			printf("Port parameter must be a non zero value.\n");
			return -1;
		}
	}
	
	FILELog::ReportingLevel() = logWARNING;
	San2::Utils::bytes serializedCapsule;
	
	SAN_UINT16 fromPort, toPort;
	San2::Utils::bytes payload;
	San2::Network::CCapsule capsule;
	San2::Api::CNodeConnector connector(srvApiAddress, 5000, 5000, 5000, 5000);
	
	PasswordLoader pload;
	
	int userCount = pload.loadFromFile(SH_SRV_PASSWORD_FILE);
	
	printf("userCount: %d\n", userCount);
	
	if (userCount <= 0)
	{
		printf("No users found, please check \"pass.pwd\" file contents.\n");
		return -1;
	}
	
	
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
	
	if (connector.registerPort(srvIntPort) == true)
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
	
	SessionManager<Session> sman([serverAddress, &connector, &pload, srvIntPort](const San2::Network::SanAddress& clientAddress, SAN_UINT16 clientPort){return new Session(connector, serverAddress, srvIntPort, clientAddress, clientPort, pload);}, SH_SRV_SESSION_TIMEOUT);
	
	while(1)
	{
		bool portsok;
		
		San2::Network::CCapsule rxcapsule;
		San2::Network::CCapsule txcapsule;
		std::shared_ptr<Session> ses;
		
		SAN_INT32 rval = connector.waitForCapsule(rxcapsule, 50000);
		
		San2::Utils::bytes data;
		
		switch(rval)
		{
			case SAN2_WAITFORCAPSULE_SUCCESS:
				portsok = rxcapsule.getPortsDS(toPort, fromPort);
				ses = sman.getSession(rxcapsule.getSourceAddress(), fromPort);
				
				if (portsok)
				{
					/*
					printf("got: rxcapsule, from port %ud", fromPort);
					std::cout << " from address " << San2::Utils::address2string(rxcapsule.getSourceAddress()) << std::endl;
					std::cout << "   to address " << San2::Utils::address2string(rxcapsule.getDestinationAddress()) << std::endl;
					*/
				}
				
				if (ses->incommingCapsule(rxcapsule) == false)
				{
					printf("Failed to generate response\n");
					return -6;
				}
				break;
			case SAN2_WAITFORCAPSULE_TIMEOUT:
				//printf("got: timeout\n");
				break;
			case SAN2_WAITFORCAPSULE_ERROR_INVALID_RESPONSE:
				printf("got: error invalid response\n");
				break;	
			case SAN2_WAITFORCAPSULE_ERROR_CAPSULE_UNPACK:
				printf("got: error capsule unpack\n");
				break;	
			case SAN2_WAITFORCAPSULE_ERROR_INVOKE_FAILED:	
				printf("Connection to San node was lost\n");
				return -1;
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
