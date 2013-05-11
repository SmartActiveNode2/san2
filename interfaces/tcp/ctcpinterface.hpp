#pragma once

#include <string>
#include <memory>
#include <atomic>
#include <mutex>

#include "ccapsulereceiver.hpp"
#include "network/nettypedef.hpp"
#include "network/cnetinterface.hpp"
#include "network/ccapsule.hpp"
#include "tcp/ctcpclient.hpp"
#include "comm/streamrpcchannel.hpp"
#include "utils/cproducerconsumer.hpp"
#include "rpc/crpcinvoker.hpp"
#include "utils/atomic.hpp"

// This is very confusing class as it combines both - client and server.
// Server is an instance of CCapsuleReceiver
// while client is this (inherited from San2::Tcp::CTcpClient)
// this actually uses 2 threads, one for client=sender and
// one for server=receiver

// There are 2 completely separate TCP connections (over RPC connections)
// one only receives (CCapsuleReceiver=TcpServer)
// the other only sends (San2::Tcp::CTcpClient)

// This inputQueue is global as it is reference
// ((outputQueue is per interface))

// Server can only make one single TCP connection (e.g. accept())
// to predefined ip and tcp port. If it does not match than it
// closes connection and waits again for another one.
// It is INTENTIONAL and SHOULD be that way. As one interface connects
// only one neighbour node.

// Both client and server are (i hope) designed as fault-tolerant
// so if the remote and disconnects it should reconnect successfully later



namespace San2
{
	namespace Interfaces
	{	
		class CTcpInterface : public San2::Network::CNetInterface, public San2::Tcp::CTcpClient
		{
		public:
			CTcpInterface(San2::Network::SanAddress sanaddr, const std::string &localIp, const std::string &localPort, const std::string &remoteIp, const std::string &remotePort, unsigned int timeCON, unsigned int timeRX, unsigned int timeTX, unsigned int timePOP, San2::Utils::CProducerConsumer<std::shared_ptr<San2::Network::CCapsule> >& inputQueue, unsigned long maxOutputQueueSize);
			
			San2::Utils::bytes firstMessage(const San2::Network::SanAddress& addr);
			
			San2::Utils::CThread* getThread();
			
			// No idea how to fix this mess 
			void up(); // CTcpInterface
			void run(); // CThread
			San2::Tcp::TcpErrorCode receive(); // CTcpClient
			
			bool sendCapsule(std::shared_ptr<San2::Network::CCapsule> capsule, San2::Utils::CThread *thr);
			
			
			San2::Network::SanAddress getPeerAddress(); // mutexed
			
			San2::Network::SanAddress getInterfaceAddress();

            San2::Network::InterfaceLineStatus getRXstate();
            San2::Network::InterfaceLineStatus getTXstate();
		protected:
					
		private:
			San2::Network::SanAddress m_sanaddr; // addres of this (local) interface
			
			// remote interface address is obtained automatically when remote node TcpClients
			// connectes to TcpSingleServer. At the beginning the remote end sends
			// "SAN2" ascii + 16B of his SanAddress
			San2::Network::SanAddress m_peeraddr;
			
			std::string m_localIp, m_localPort;
			std::string m_remoteIp, m_remotePort;
			
			unsigned int m_timeCON, m_timeRX, m_timeTX, m_timePOP;
			
			// Receiver part
			San2::Utils::CProducerConsumer<std::shared_ptr<San2::Network::CCapsule> >& m_inputQueue;
			San2::Utils::CProducerConsumer<std::shared_ptr<San2::Network::CCapsule> > m_outputQueue;
			
            San2::Utils::Atomic<San2::Network::InterfaceLineStatus> m_RXstatus, m_TXstatus;

			CCapsuleReceiver srv;
			
			// Sender part
			San2::Comm::StreamRpcChannel *m_rpcChannel;
			San2::Rpc::CRpcInvoker *m_rpci;
			
			std::mutex m_mutexPeerAddress;
			std::mutex m_mutexInterfaceAddress;
			
			CTcpInterface& self(){return *this;}
			
			friend class CCapsuleReceiver;
			void setPeerAddress(const San2::Network::SanAddress &address); // mutexed
		};
	}
}
