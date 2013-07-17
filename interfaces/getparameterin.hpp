
#include <memory>

#include "utils/cproducerconsumer.hpp"
#include "network/ccapsule.hpp"
#include "network/nettypedef.hpp"
#include "utils/cvector.hpp"
#include "rpc/cirpcsyncfunctionin.hpp"
#include "utils/platform/basictypes.hpp"
#include "node/cportmap.hpp"
#include "node/cnode.hpp"
#include "utils/address.hpp"
#include "utils/cstringutils.hpp"
#include "utils/hex.hpp"

namespace San2 {
	namespace Interfaces
	{
		class GetParameterIn : public San2::Rpc::CIRpcSyncFunctionIn
		{
		public:
			static const int timeoutMilisec;
		
			GetParameterIn(San2::Node::CNode &node, San2::Node::CPortmap& portmapper); // must be mutexed inside portmapper!!!
			unsigned int getUniqueId()const;
			bool operator()(void);
			bool unpack(const San2::Utils::bytes &in);
			San2::Utils::bytes getResponse();
			
		private:
			San2::Node::CNode &m_node;
			San2::Node::CPortmap& m_portmapper;
			std::string m_parameter;
			San2::Utils::bytes m_response;
			
			const char* statusToString(San2::Network::InterfaceLineStatus status);
		};
	}
}
