#pragma once

#include <string>
#include <vector>

#include "network/ccapsule.hpp"


#include "cppl/pipechannel.hpp"
#include "cppl/pipeserver.hpp"

#include "cnode.hpp"
#include "comm/streamrpcchannel.hpp"
#include "rpc/crpcexecutor.hpp"
#include "comm/streamrpcchannel.hpp"

namespace San2 
{
	namespace Node
	{
		class CNodeApiChannel : public San2::Cppl::PipeChannel
		{
		public:
			CNodeApiChannel(CPPL_PIPETYPE handle, unsigned int timRX, unsigned int timTX, CNode &node);
			virtual ~CNodeApiChannel();
			San2::Cppl::ErrorCode receive();
            
        protected:
    		San2::Comm::StreamRpcChannel *m_rpcChannel;
			San2::Rpc::CRpcExecutor *m_rpcexec;

		private:
			 // another msvc fix
			#ifdef LINUX
				CNodeApiChannel(const CNodeApiChannel& copyFromMe)=delete;
				CNodeApiChannel& operator=(const CNodeApiChannel& copyFromMe)=delete;
			#endif
			
			CNode &m_node;

		};
	}
}
