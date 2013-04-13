


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>

#include "api/node/connector/cnodeconnector.hpp"
#include "network/ccapsule.hpp"
#include "utils/log.h"

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
	
	San2::Utils::bytes payload;
	payload.append("Ahoj, jak se mas");
	
	San2::Network::CCapsule capsule;
	capsule.setData(payload);
	
	San2::Utils::bytes serial;
	capsule.pack(serial);

	connector.sendCapsule(serial);
	connector.sendCapsule(serial);
	connector.sendCapsule(serial);
	
	if (connector.registerPort(2345) == true)
	{
		printf("port register: OK\n");
	}
	else
	{
		printf("port register: FAILURE\n");
	}
	
	return 0;
}
