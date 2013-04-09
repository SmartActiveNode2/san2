#pragma once

#include <string>
#include <vector>

#include "network/ccapsule.hpp"
#include "cppl/pipechannel.hpp"
#include "cppl/pipeserver.hpp"
#include "comm/streamrpcchannel.hpp"
#include "rpc/crpcexecutor.hpp"
#include "comm/streamrpcchannel.hpp"
#include "utils/cproducerconsumer.hpp"

namespace San2 
{
	namespace Api
	{
		class CApiServiceChannel : public San2::Cppl::PipeChannel
		{
		public:
			CApiServiceChannel(CPPL_PIPETYPE handle, unsigned int timRX, unsigned int timTX, San2::Utils::CProducerConsumer<std::shared_ptr<San2::Network::CCapsule> >& inputQueue);
			virtual ~CApiServiceChannel();
			San2::Cppl::ErrorCode receive();
            
        protected:
    		San2::Comm::StreamRpcChannel *m_rpcChannel;
			San2::Rpc::CRpcExecutor *m_rpcexec;

		private:
			 // another msvc fix
			#ifdef LINUX
				CApiServiceChannel(const CApiServiceChannel& copyFromMe)=delete;
				CApiServiceChannel& operator=(const CApiServiceChannel& copyFromMe)=delete;
			#endif

			San2::Utils::CProducerConsumer<std::shared_ptr<San2::Network::CCapsule> >& m_inputQueue;
		};
	}
}
