


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
	
	San2::Api::CNodeConnector connector(PIPENAME, 5000, 5000, 5000, 5000);
	
	if (connector.open() != San2::Cppl::ErrorCode::SUCCESS)
	{
		printf("connection failed\n");
		return -1;
	}

	connector.connect();
	
	if (connector.registerPort(2200) == true)
	{
		printf("port register: OK\n");
	}
	else
	{
		printf("port register: FAILURE\n");
	}
	
	San2::Network::SanAddress dstaddr;
	
	if (San2::Utils::string2address("000000000000000000000000000000000000000000000000000000000000FF11", dstaddr) != true)
	{
		FILE_LOG(logDEBUG4) << "failed to parse destination address";
		return -1;
	}
	
	
	San2::Utils::bytes payload;
	payload.append("Ahoj, jak se mas");
	
	San2::Network::CCapsule capsule;
	capsule.setDSdata(2201, 2200, payload);
	capsule.setDestinationAddress(dstaddr);
	
	San2::Utils::bytes serial;
	capsule.pack(serial);

	connector.sendCapsule(serial);
	
	/*
	San2::Network::CCapsule rxcapsule;
	
	printf("awaiting capsule\n");
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
	*/
	
	return 0;
}
