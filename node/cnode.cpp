#include <algorithm>
#include <assert.h>
#include <stdlib.h>

#include "cppl/pipeserver.hpp"
#include "cnode.hpp"
#include "utils/log.h"

#define TIME_CON 2000
#define TIME_RX  2000
#define TIME_TX  2000

namespace San2 
{
	namespace Node
	{
		class CPortmap; // forward decalaration
	}
}

namespace San2 { namespace Node {

CNode::CNode(unsigned int inputQueueMaxSize, std::string nodeName, unsigned int timePOP, std::string apiAddress) :
	m_apiAddress(apiAddress),
	m_inputQueue(inputQueueMaxSize),
	m_timePOP(timePOP),
	m_nodeName(nodeName)
{

}

void CNode::run()
{
	// here should be capsule executor/worker implementation
	// here should be a router implementation
	
	// Node api
	CNode &me = self();
	San2::Cppl::PipeServer apiServer(m_apiAddress.c_str(), [&] (CPPL_PIPETYPE handle, unsigned int timRX, unsigned int timTX) {return new San2::Api::CNodeServiceChannel(handle, timRX, timTX, m_inputQueue, m_portmap);}, TIME_CON, TIME_RX, TIME_TX);
	apiServer.start();
	
	
	std::shared_ptr<San2::Network::CCapsule> capsule;
	
	while(1)
	{
		
		
		m_inputQueue.pop(&capsule, this, m_timePOP);
		FILE_LOG(logDEBUG4) << "CNode::run()::pop(): gotCapsule ######";
		
	
		// check if we reached destination
		bool destinationReached = false;
		{
			std::lock_guard<std::mutex> lock(m_mutexInterfaces);
			
			std::for_each(m_interfaces.begin(), m_interfaces.end(), 
				[&destinationReached, capsule](std::shared_ptr<San2::Network::CNetInterface> iface) 
					{
						/*
						FILE_LOG(logDEBUG4) << "KK4";
						
						printf("capsule->getDestinationAddress(): ");
						San2::Network::SanAddress tmp = capsule->getDestinationAddress();
						for (unsigned int v = 0 ; v < San2::Network::sanAddressSize ; ++v ) printf("%02X", tmp[v]);
						printf("\n");
						
						printf("iface->getInterfaceAddress():");
						tmp = iface->getInterfaceAddress();
						for (unsigned int v = 0 ; v < San2::Network::sanAddressSize ; ++v ) printf("%02X", tmp[v]);
						printf("\n");
						printf("\n");
						*/
						
						if (capsule->getDestinationAddress() == iface->getInterfaceAddress())
						{
							// mark that we have reached destination
							destinationReached = true;
						}
					});
		}
		
		
		// If destination was reached
		if (destinationReached == true)
		{
			// FILE_LOG(logDEBUG4) << "CNode::run():: capsule reached its final destination ######";
			
			// Check the DS Flag
			SAN_UINT16 dstport, srcport;
			if (capsule->getPortsDS(dstport, srcport)) // if DS was set
			{
				// Check if dstport is registered
				San2::Utils::CProducerConsumer<std::shared_ptr<San2::Network::CCapsule> >* dsque = m_portmap.getPortQueue(dstport);
				
				if (dsque != NULL) // if port was registered
				{
					// put capsule in the que
					// tail-drop !!!!, you should always tail-drop here!!!
					// so always use try_push() here with zero timeout!
					
					if (dsque->try_push(capsule, this, 0) == -1)
					{
						 FILE_LOG(logDEBUG2) << "CNode:run(): TERMINATED";
						 break;
					}
					
					FILE_LOG(logDEBUG4) << "CNode:run(): send DS capsule to application queue OK";	
					
					continue; // ok, nothing to do now, wait for another capsule to come
					
				}
				else
				{
					FILE_LOG(logDEBUG4) << "CNode:run(): got ds capsule, but unregistered port";	
					continue; // ok, nothing to do now, wait for another capsule to come
				}
			
			
				continue; // ok, nothing to do now, wait for another capsule to come
			}
			
            if (capsule->getAppId() == San2::Network::sanTestMessageApplicationId)
            {
                San2::Utils::bytes message = capsule->getData();
                if (message[message.size()-1] != 0) message.push_back(0);
                FILE_LOG(logDEBUG1) << "incoming msg:" << message.toArray();
            }

			continue; // ok, nothing to do now, wait for another capsule to come
		}
	
		// very simple routing algorithm which checks
		// if the destination SanAddress matches one of our 
		// direct peers SanAddress and sends the capsule that way.	
		destinationReached = false;
		{
			std::lock_guard<std::mutex> lock(m_mutexInterfaces);
			
			// FILE_LOG(logDEBUG4) << "KK3";
			std::for_each(m_interfaces.begin(), m_interfaces.end(), [&destinationReached, capsule, this](std::shared_ptr<San2::Network::CNetInterface> iface)
			{
				
				/*
				FILE_LOG(logDEBUG4) << "KK4";
				
				printf("capsule->getDestinationAddress(): ");
				San2::Network::SanAddress tmp = capsule->getDestinationAddress();
				for (unsigned int v = 0 ; v < San2::Network::sanAddressSize ; ++v ) printf("%02X", tmp[v]);
				printf("\n");
				
				printf("iface->getPeerAddress(): ");
				tmp = iface->getPeerAddress();
				for (unsigned int v = 0 ; v < San2::Network::sanAddressSize ; ++v ) printf("%02X", tmp[v]);
				printf("\n");
				printf("\n");
				*/
				
				if (capsule->getDestinationAddress() == iface->getPeerAddress()) 
				{
					destinationReached = iface->sendCapsule(capsule, this); 
					if (!destinationReached) 
					{
						FILE_LOG(logDEBUG4) << "CNode::run(): iface->sendCapsule failed";
					}
				}
			});
		}
		
		if (destinationReached == true) 
		{
			FILE_LOG(logDEBUG4) << "CNode::run():: rerouting capsule";
		}
		
	}
}

int CNode::addInterface(std::shared_ptr<San2::Network::CNetInterface> iface)
{
	std::lock_guard<std::mutex> lock(m_mutexInterfaces);
	iface->up();
	m_interfaces.insert(iface);
	return 0;
}


San2::Utils::CProducerConsumer<std::shared_ptr<San2::Network::CCapsule> >& CNode::getInputQueue()
{
	return m_inputQueue;
}

// do not confuse with routing, just puts capsule into the inputQueue
bool CNode::injectCapsule(std::shared_ptr<San2::Network::CCapsule> capsule)
{
	assert(capsule != NULL);
	return m_inputQueue.push(capsule);
}

bool CNode::injectCapsule(std::shared_ptr<San2::Network::CCapsule> capsule, San2::Utils::CThread *thr, unsigned int timeoutMsec)
{
	assert(capsule != NULL);
	return m_inputQueue.push(capsule, this, timeoutMsec);
}

bool CNode::tryInjectCapsule(std::shared_ptr<San2::Network::CCapsule> capsule, San2::Utils::CThread *thr, unsigned int timeoutMsec)
{
	assert(capsule != NULL);
	return m_inputQueue.try_push(capsule, this, timeoutMsec);
}

std::string CNode::getNodeName() const
{
	return m_nodeName;
}

std::set<std::shared_ptr<San2::Network::CNetInterface> > CNode::getInterafces()
{
    std::lock_guard<std::mutex> lock(m_mutexInterfaces);
    return m_interfaces;
}

}} // ns
