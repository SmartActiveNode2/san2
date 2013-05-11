
#include "ctcpinterface.hpp"
#include "interfaces/sendcapsulefuncout.hpp"
#include "interfaces/alivefuncout.hpp"
#include "comm/tcpstreamrw.hpp"
#include "utils/platform/sleep.hpp"
#include "utils/cvector.hpp"
#include "utils/log.h"
#include "utils/platform/sigignore.hpp"
#include "ccapsulereceiver.hpp"

#define CTCP_INTERFACE_MAX_SINGLE_READ_SIZE 2000

namespace San2 { namespace Interfaces {
		

CTcpInterface::CTcpInterface(San2::Network::SanAddress sanaddr, const std::string &localIp, const std::string &localPort, const std::string &remoteIp, const std::string &remotePort, unsigned int timeCON, unsigned int timeRX, unsigned int timeTX, unsigned int timePOP, San2::Utils::CProducerConsumer<std::shared_ptr<San2::Network::CCapsule> >& inputQueue, unsigned long maxOutputQueueSize) :
	San2::Tcp::CTcpClient(remoteIp, remotePort, timeCON, timeRX, timeTX),
	m_sanaddr(sanaddr),
	m_peeraddr(San2::Network::sanDefaultAddress),
	m_localIp(localIp),
	m_localPort(localPort),
	m_remoteIp(remoteIp),
	m_remotePort(remotePort),
	m_timeCON(timeCON),
	m_timeRX(timeRX),
	m_timeTX(timeTX),
    m_timePOP(timePOP),
	m_inputQueue(inputQueue),
	m_outputQueue(maxOutputQueueSize),
    srv(self() ,localIp, localPort, timeCON, timeRX, timeTX, inputQueue, m_RXstatus),
	m_rpcChannel(NULL),
	m_rpci(NULL)
{
    m_TXstatus = San2::Network::InterfaceLineStatus::DISCONNECTED;
}

San2::Utils::bytes CTcpInterface::firstMessage(const San2::Network::SanAddress& addr)
{
	// Todo: fix 4
	San2::Utils::bytes firstMessage;
	firstMessage.resize(4 + San2::Network::sanAddressSize);
	firstMessage[0] = 0x53; // S
	firstMessage[1] = 0x41; // A
	firstMessage[2] = 0x4E; // N
	firstMessage[3] = 0x32; // 2
	std::copy(addr.begin(), addr.end(), firstMessage.begin() + 4);	
	return firstMessage;
}

San2::Utils::CThread* CTcpInterface::getThread()
{
	return this;
}

void CTcpInterface::up()
{
	start();
}

void CTcpInterface::run()
{
	#ifdef LINUX
		San2::Utils::san_ignore_sigpipe();
	#endif
	
	srv.start(); // start sender thread, it will put received capsules in inputQueue automatically
	
	// Now the client=sender stuff
	
	San2::Comm::TcpStreamRW stream(CTCP_INTERFACE_MAX_SINGLE_READ_SIZE, this);
	m_rpcChannel = new San2::Comm::StreamRpcChannel(stream);
	m_rpci = new San2::Rpc::CRpcInvoker(*m_rpcChannel, m_timeRX);

    // Another MSVC fix :)
    // error C3480: a lambda capture variable must be from an enclosing function scope
    San2::Network::SanAddress msvc_fix_sanaddr = m_sanaddr;

	bool ret = m_rpci->registerFunction([this, &msvc_fix_sanaddr](){return new SendCapsuleFuncOut;});
	if (!ret)
	{
		FILE_LOG(logERROR) << "CTcpInterface::run(): registrer function FAILED";
        m_TXstatus = San2::Network::InterfaceLineStatus::FAILURE;
		return;
	}	
	
    ret = m_rpci->registerFunction([](){return new AliveFuncOut;});
	if (!ret)
	{
		FILE_LOG(logERROR) << "CTcpInterface::run(): registrer AliveFunc function FAILED";
        m_TXstatus = San2::Network::InterfaceLineStatus::FAILURE;
		return;
	}	

	SendCapsuleFuncOut func;
	AliveFuncOut aliveFunc;

	// get item from outputQueue and send it
	
	// Fault-tolerant client
	while(!isTerminated()) // When node fails and reconnectes, this ensures connection will be ok
	{
		if (open() != San2::Tcp::TcpErrorCode::SUCCESS)
		{
			unsigned int sleepSec = m_timeCON / 1000;
			// Safety feature
			if (sleepSec <= 0) sleepSec = 1;
			
			San2::Utils::SanSleep(sleepSec);
			continue;
		}

		// TODO: This could block indefinitely. Fix.
		if (stream.writeAll(firstMessage(m_sanaddr)) != true)
		{
			// conection failure, retry
			// (again, we want to have it fault-tolerant)
			continue;
		}
		
        FILE_LOG(logDEBUG4) << "IFACE-TCP: connected TX";
        m_TXstatus = San2::Network::InterfaceLineStatus::CONNECTED;

		while(!isTerminated())
		{
			std::shared_ptr<San2::Network::CCapsule> capsule;

            bool popWasSuccessfull = true;

            while (m_outputQueue.try_pop(&capsule, this, m_timePOP))
            {
                if (isTerminated())
                {
                    m_TXstatus = San2::Network::InterfaceLineStatus::FAILURE;
                    return;
                }

			    if (!m_rpci->invokeFunction(aliveFunc)) // test if connection ok (dummy function that sends, but does nothing)
			    {
				    FILE_LOG(logDEBUG4) << "TCP-IFACE: connection lost TX";
                    m_TXstatus = San2::Network::InterfaceLineStatus::DISCONNECTED;
                    popWasSuccessfull = false;
                    break;
			    }   
            }

            if (popWasSuccessfull == false)
            {
                break; // important
            }

			if (func.setCapsuleToSend(capsule) != true)
			{
				FILE_LOG(logERROR) << "TCP-IFACE: capsule packing failed() ***FAILED*** ";	
			}
			
			bool rval = m_rpci->invokeFunction(func);
			if (!rval)
			{
				FILE_LOG(logDEBUG4) << "TCP-IFACE: connection lost TX";
                m_TXstatus = San2::Network::InterfaceLineStatus::DISCONNECTED;
				break;
			}
		}
	}
	
	//FILE_LOG(logDEBUG4) << "CTcpInterface::run(): returned";
}

San2::Tcp::TcpErrorCode CTcpInterface::receive()
{
	// intentionally empty as the client only sends
	// we dont call client.start() so this is NEVER called (because we have overridden run())
	// but must be implemented, becouse it is a pure virtual function in CTcpClient
	// (again very confusing, sorry for that)
	return San2::Tcp::TcpErrorCode::SUCCESS;
}

bool CTcpInterface::sendCapsule(std::shared_ptr<San2::Network::CCapsule> capsule, San2::Utils::CThread *thr)
{
	// push to output queue
	// parameter must be calling thread, not the clientClass itself!
	// MUST NOT block too long, otherwise it will block all other niterfaces in CNode
    // therfore TAIL DROP is implemented, linke in IPv4 routers
    // there is nothing we can do about this if we have limited queue space
    return !(m_outputQueue.try_push(capsule, thr, 0));
}

void CTcpInterface::setPeerAddress(const San2::Network::SanAddress &address)
{
	// FILE_LOG(logDEBUG3) << "CTcpInterface::setPeerAddress()";
	std::lock_guard<std::mutex> lock(m_mutexPeerAddress);
	m_peeraddr = address;
}

San2::Network::SanAddress CTcpInterface::getPeerAddress()
{
	std::lock_guard<std::mutex> lock(m_mutexPeerAddress);
	return m_peeraddr; // TODO: fix
}

San2::Network::SanAddress CTcpInterface::getInterfaceAddress()
{
	std::lock_guard<std::mutex> lock(m_mutexInterfaceAddress);
	return m_sanaddr;
}

San2::Network::InterfaceLineStatus CTcpInterface::getRXstate()
{
    return m_RXstatus;
}

San2::Network::InterfaceLineStatus CTcpInterface::getTXstate()
{
    return m_TXstatus;
}

}} // ns
