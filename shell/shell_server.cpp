


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>

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

int main(int argc, char *argv[])
{
	FILELog::ReportingLevel() = logDEBUG4;
	San2::Utils::bytes serializedCapsule;
	
	SAN_UINT16 fromPort, toPort;
	San2::Utils::bytes payload;
	San2::Network::CCapsule capsule;
	San2::Api::CNodeConnector connector(PIPENAME, 5000, 5000, 5000, 5000);
	
	SessionManager<Session> sman([](const San2::Network::SanAddress& srcAddr, SAN_UINT16 srcPort){return new Session(srcAddr, srcPort);}, 3600);
	
	if (connector.open() != San2::Cppl::ErrorCode::SUCCESS)
	{
		printf("connection failed\n");
		return -1;
	}

	connector.connect();
	
	
	// payload.append("Ahoj, jak se mas");
	/*
	payload = "Ahoj, jak se mas";
	capsule.setData(payload);
	
	capsule.pack(serializedCapsule);
	connector.sendCapsule(serializedCapsule);
	* */
	
	if (connector.registerPort(APPLICATION_PORT) == true)
	{
		printf("port register: OK\n");
	}
	else
	{
		printf("port register: FAILURE\n");
		return -1;
	}
	
	
	while(1)
	{
	
		San2::Network::CCapsule rxcapsule;
		
		printf("awaiting capsule\n");
		SAN_INT32 rval = connector.waitForCapsule(rxcapsule, 50000);
		
		switch(rval)
		{
			case SAN2_WAITFORCAPSULE_SUCCESS:
				if (rxcapsule.getPortsDS(toPort, fromPort))
				{
					printf("got: rxcapsule, from port %ud", fromPort);
					
					std::cout << " from address " << San2::Utils::address2string(rxcapsule.getSourceAddress()) << std::endl;
					std::cout << "   to address " << San2::Utils::address2string(rxcapsule.getDestinationAddress()) << std::endl;
					
					
					San2::Utils::bytes data = rxcapsule.getData();
					char *message = (char*) malloc(sizeof(char) * (data.size() - 4));
					memcpy(message, &data[4], data.size() - 4);
					message[data.size() - 5] = 0;
					printf("message:%s\n", message);
					
					// echo
					capsule.setSourceAddress(rxcapsule.getDestinationAddress());
					capsule.setDestinationAddress(rxcapsule.getSourceAddress());
					capsule.setDSdata(fromPort, APPLICATION_PORT, rxcapsule.getData());
					capsule.pack(serializedCapsule);
					
					std::cout << "DSTADDR: " << San2::Utils::address2string(capsule.getDestinationAddress()) << std::endl;
					
					connector.sendCapsule(serializedCapsule);
				}
				else
				{
					printf("got: rxcapsule, from port ?\n");	
				}
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
