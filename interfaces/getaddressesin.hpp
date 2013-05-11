
#include <memory>

#include "utils/cproducerconsumer.hpp"
#include "network/ccapsule.hpp"
#include "network/nettypedef.hpp"
#include "utils/cvector.hpp"
#include "rpc/cirpcsyncfunctionin.hpp"
#include "utils/platform/basictypes.hpp"
#include "node/cportmap.hpp"
#include "utils/address.hpp"
#include "node/cnode.hpp"


namespace San2 {
	
	// namespace Node { class CNode; }
	
	namespace Interfaces
	{
		class GetAddressesIn : public San2::Rpc::CIRpcSyncFunctionIn
		{
		public:
			static const int timeoutMilisec;
		
			GetAddressesIn(San2::Node::CNode &node);
			unsigned int getUniqueId()const;
			bool operator()(void);
			bool unpack(const San2::Utils::bytes &in);
			San2::Utils::bytes getResponse();
			
		private:
			San2::Node::CNode &m_node;
			SAN_UINT16 m_port;
			San2::Utils::bytes m_response;
		};
	}
}
