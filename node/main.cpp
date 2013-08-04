
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <memory>
#include <algorithm>

#include "cnode.hpp"
#include "cipcchannel.hpp"
#include "interfaces/tcp/ctcpinterface.hpp"
#include "utils/platform/sleep.hpp"
#include "utils/config.hpp"
#include "utils/log.h"
#include "utils/hex.hpp"
#include "utils/address.hpp"

#include "network/ccapsule.hpp"
#include "network/nettypedef.hpp"
#include "cppl/pipeserver.hpp"
#include "cppl/pipechannel.hpp"

#define TIME_CON 2000
#define TIME_RX  2000
#define TIME_TX  2000
#define TIME_POP 2000
#define TIME_POP_NODE 2000
#define TCP_QUEUESIZE 50
#define INPUT_QUEUE_SIZE 50

std::string int2string(int x)
{
    std::stringstream stream;
	stream << x;
	return stream.str();
}

std::string getKeyTextString(int x, const std::string &key)
{
	std::string s;
	s.append("peer."); 
	s.append(int2string(x));
	s.append(".");
	s.append(key);
	return s;
}

std::string getPeerConfig(San2::Utils::Config::ConfigFile &cfg, int x, const std::string &key)
{
	return cfg.getValue(getKeyTextString(x, key));
}




int main(int argc, char *argv[])
{
	//ps.start();
	//ps.join();
	
	FILELog::ReportingLevel() = logWARNING;
	FILE_LOG(logDEBUG3) << "logger working";
	
	if (argc != 2)
	{
		printf("Missing parameter: configuration file\nUsage: sanode <configFile>\n");
		return -1;
	}
	
	
	
	San2::Utils::Config::ConfigFile cfg;
	
	if (!cfg.load(argv[1]))
	{
		printf("failed to load config file: %s\nexiting\n", argv[1]);
		return -1;
	}
	
    // Start WSA

    #ifdef WIN32
		WSADATA wsaData;
		int ret = WSAStartup(0x202, &wsaData);
		if (ret)
		{
			printf("WSAStartup failed: %d", ret);
			return -1;
		}
	#endif

	// ---- setup InterProcessComunnication to terminal application
	
	std::string ipcAddress = cfg.getValue("ipcAddress");
	std::string apiAddress = cfg.getValue("apiAddress");
	
	if (!ipcAddress.compare(std::string("")))
	{
		printf("failed to parse ipcAddress from configuration file\n");
		return -2;
	}
	

	San2::Node::CNode node(INPUT_QUEUE_SIZE, cfg.getValue("nodeName"), TIME_POP_NODE, apiAddress);

	San2::Cppl::PipeServer ps(ipcAddress.c_str(), [&node] (CPPL_PIPETYPE handle, unsigned int timRX, unsigned int timTX) {return new San2::Node::CIpcChannel(handle, timRX, timTX, node);}, TIME_CON, TIME_RX, TIME_TX);
	
	std::string localIp, localPort, remoteIp, remotePort, ifAddress;
    
    San2::Utils::bytes ba;
    San2::Network::SanAddress sanaddr;
		
	int i;
	for (i = 1; ; i++)
	{
		ifAddress = getPeerConfig(cfg, i, "ifAddress"); 
		localIp = getPeerConfig(cfg, i, "localIp");
		localPort = getPeerConfig(cfg, i, "localPort");
		remoteIp = getPeerConfig(cfg, i, "remoteIp");
		remotePort = getPeerConfig(cfg, i, "remotePort");
		
		if (!ifAddress.compare(std::string("")) || !localIp.compare(std::string("")) || !localPort.compare(std::string("")) || !remoteIp.compare(std::string("")) || !remotePort.compare(std::string("")))
		{
			FILE_LOG(logDEBUG4) << "failed to parse interface" << i;
			break;
		}
		
		if (San2::Utils::string2address(ifAddress, sanaddr) != true)
		{
			FILE_LOG(logDEBUG4) << "failed to parse peer." << i << ".ifAddress from config";
			continue; //skip
		}
		
		std::shared_ptr<San2::Interfaces::CTcpInterface> tcpif(new San2::Interfaces::CTcpInterface(sanaddr, localIp, localPort, remoteIp, remotePort, TIME_CON, TIME_RX, TIME_TX, TIME_POP, node.getInputQueue(), TCP_QUEUESIZE));
		FILE_LOG(logDEBUG4) << "adding peer #" << i;
		node.addInterface(tcpif);
	}
	
	node.start();
	ps.start(); // ipc start
    printf("running\n");
    node.join();
	
   	#ifdef WIN32
		// TODO: Error checking
		WSACleanup();
	#endif

	return 0;
}
