
#pragma once

#include "cppl/pipeclient.hpp"
#include "comm/streamrpcchannel.hpp"
#include "rpc/crpcinvoker.hpp"

namespace San2 { namespace Api {
	
	class CNodeConnector : public San2::Cppl::PipeClient
	{
	public:
		CNodeConnector(const char *pipeName, unsigned int timCON, unsigned int timRX, unsigned int timTX, unsigned int timRPC_RX);
		virtual ~CNodeConnector();
		San2::Cppl::ErrorCode receive(); // leave empty?
		
		bool connect();
		bool sendCapsule(San2::Utils::bytes &capsuleData);
	protected:	
		
	private:
		unsigned int m_timRPC_RX;
	
		// Sender part
		San2::Comm::StreamRpcChannel *m_rpcChannel;
		San2::Rpc::CRpcInvoker *m_rpci;
	};
	
	
}} // ns
