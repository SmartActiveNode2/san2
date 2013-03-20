

#pragma once

#include "network/nettypedef.hpp"
#include "network/ccapsule.hpp"
#include "cppl/pipeclient.hpp"
#include "comm/streamrpcchannel.hpp"
#include "utils/cproducerconsumer.hpp"
#include "rpc/crpcinvoker.hpp"

namespace San2 { namespace ClientApi {
	
	class CApiClient : public San2::Cppl::PipeClient
	{
	public:
		CApiClient(const char *pipeName, unsigned int timCON, unsigned int timRX, unsigned int timTX, unsigned int timRPC_RX);
		virtual ~CApiClient();
		
		San2::Cppl::ErrorCode receive();
		
	protected:
		// Sender part
		San2::Comm::StreamRpcChannel *m_rpcChannel;
		San2::Rpc::CRpcInvoker *m_rpci;

		unsigned int m_timRPC_RX;
	private:
		
	};
	
	
	
}} //ns
