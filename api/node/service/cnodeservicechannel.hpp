#pragma once

#include <string>
#include <vector>

#include "network/ccapsule.hpp"

#include "cppl/pipechannel.hpp"
#include "cppl/pipeserver.hpp"

#include "rpc/crpcexecutor.hpp"

#include "comm/streamrpcchannel.hpp"
#include "comm/streamrpcchannel.hpp"

#include "node/cnode.hpp"
#include "node/cportmap.hpp"

namespace San2 
{
	namespace Node 
	{
	  class CNode;
	  class CPortmap;
	}
}

namespace San2 
{
	namespace Api
	{	
		class CNodeServiceChannel : public San2::Cppl::PipeChannel
		{
		public:
			CNodeServiceChannel(CPPL_PIPETYPE handle, unsigned int timRX, unsigned int timTX, San2::Utils::CProducerConsumer<std::shared_ptr<San2::Network::CCapsule> >& inputQueue, San2::Node::CPortmap& portmap);
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

			San2::Utils::CProducerConsumer<std::shared_ptr<San2::Network::CCapsule> >& m_nodeQueue;
			
			San2::Utils::CProducerConsumer<std::shared_ptr<San2::Network::CCapsule> > m_applicationQueue;
			
			San2::Node::CPortmap& m_portmap;
		};
	}
}
