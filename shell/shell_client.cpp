


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

#define PIPENAME "/tmp/sanode1api"

int main(int argc, char *argv[])
{
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

	connector.connect();
		
	SAN_UINT16 port = connector.getEphemeralPort();
	
	FILE_LOG(logDEBUG4) << "ephemeral port: " << port;
	
	if (port == 0) return -1;
	
	San2::Network::SanAddress dstaddr;
	
	if (San2::Utils::string2address("000000000000000000000000000000000000000000000000000000000000FF21", dstaddr) != true)
	{
		FILE_LOG(logDEBUG4) << "failed to parse destination address";
		return -1;
	}
	
	San2::Network::SanAddress srcaddr;
	
	if (San2::Utils::string2address("000000000000000000000000000000000000000000000000000000000000FF12", srcaddr) != true)
	{
		FILE_LOG(logDEBUG4) << "failed to parse source address";
		return -2;
	}
	
	
	
	
	
	payload.append("Ahoj, jak se mas?");

	capsule.setDSdata(2201, port, payload);
	capsule.setDestinationAddress(dstaddr);
	capsule.setSourceAddress(srcaddr);
	capsule.pack(serializedCapsule);


	San2::Network::CCapsule testcap;
	if (testcap.unpack(serializedCapsule) == false)
	{
		printf("UNPACKING CAPSULE FAILED\n");
		return -5;
	}
	std::cout << "srcaddr: " << San2::Utils::address2string(testcap.getSourceAddress()) << std::endl;

	connector.sendCapsule(serializedCapsule);
	
	
	San2::Network::CCapsule rxcapsule;
	
	printf("awaiting response\n");
	SAN_INT32 rval = connector.waitForCapsule(rxcapsule, 5000);
	
	switch(rval)
	{
		case SAN2_WAITFORCAPSULE_SUCCESS:
			printf("got: rxcapsule\n");
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
	
	
	return 0;
}
