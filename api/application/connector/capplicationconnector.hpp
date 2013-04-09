

#pragma once

#include "network/nettypedef.hpp"
#include "network/ccapsule.hpp"
#include "cppl/pipeclient.hpp"
#include "comm/streamrpcchannel.hpp"
#include "utils/cproducerconsumer.hpp"
#include "rpc/crpcinvoker.hpp"

namespace San2 { namespace Api {
	
	class CApplicationConnector : public San2::Cppl::PipeClient
	{
	public:
		CApplicationConnector(const char *pipeName, unsigned int timCON, unsigned int timRX, unsigned int timTX, unsigned int timRPC_RX);
		virtual ~CApplicationConnector();
		
		San2::Cppl::ErrorCode receive();
		San2::Cppl::ErrorCode connect();
		bool sendCapsule(San2::Utils::bytes &capsuleData);
	protected:
		// Sender part
		San2::Comm::StreamRpcChannel *m_rpcChannel;
		San2::Rpc::CRpcInvoker *m_rpci;

		unsigned int m_timRPC_RX;
	private:
		
	};
	
	
	
}} //ns
