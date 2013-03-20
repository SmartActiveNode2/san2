#pragma once

#include <string>
#include <vector>

#include "network/ccapsule.hpp"

#include "cppl/pipechannel.hpp"
#include "cppl/pipeserver.hpp"

#include "node/cnode.hpp"

#include "rpc/crpcexecutor.hpp"

#include "comm/streamrpcchannel.hpp"
#include "comm/streamrpcchannel.hpp"

namespace San2 
{
	namespace Api
	{
		class CNodeServiceChannel : public San2::Cppl::PipeChannel
		{
		public:
			CNodeServiceChannel(CPPL_PIPETYPE handle, unsigned int timRX, unsigned int timTX, San2::Node::CNode &node);
			virtual ~CNodeServiceChannel();
			San2::Cppl::ErrorCode receive();
            
        protected:
    		San2::Comm::StreamRpcChannel *m_rpcChannel;
			San2::Rpc::CRpcExecutor *m_rpcexec;

		private:
			 // another msvc fix
			#ifdef LINUX
				CNodeServiceChannel(const CNodeServiceChannel& copyFromMe)=delete;
				CNodeServiceChannel& operator=(const CNodeServiceChannel& copyFromMe)=delete;
			#endif
			
			San2::Node::CNode &m_node;

		};
	}
}
