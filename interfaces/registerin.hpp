
#include <memory>

#include "utils/cproducerconsumer.hpp"
#include "network/ccapsule.hpp"
#include "network/nettypedef.hpp"
#include "utils/cvector.hpp"
#include "rpc/cirpcsyncfunctionin.hpp"
#include "utils/platform/basictypes.hpp"
#include "node/cportmap.hpp"

namespace San2 {
	namespace Interfaces
	{
		class RegisterIn : public San2::Rpc::CIRpcSyncFunctionIn
		{
		public:
			static const int timeoutMilisec;
		
			RegisterIn(San2::Node::CPortmap& portmapper); // must be mutexed inside portmapper!!!
			unsigned int getUniqueId()const;
			bool operator()(void);
			bool unpack(const San2::Utils::bytes &in);
			San2::Utils::bytes getResponse();
			
		private:
			SAN_UINT16 m_port;
			San2::Node::CPortmap& m_portmapper;
			San2::Utils::bytes m_response;
		};
	}
}
