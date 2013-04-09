
#include <memory>

#include "rpc/cirpcsyncfunctionout.hpp"
#include "utils/cproducerconsumer.hpp"
#include "network/ccapsule.hpp"
#include "network/nettypedef.hpp"
#include "utils/cvector.hpp"
#include "utils/platform/basictypes.hpp"

namespace San2 {
	namespace Interfaces
	{
		class WaitForCapsuleOut : public San2::Rpc::CIRpcSyncFunctionOut
		{
		public:
			static const int timeoutMilisec;
		
			WaitForCapsuleOut();
			void setTimeout(SAN_UINT32 timeout);
			unsigned int getUniqueId()const;
			bool operator()(void);
			
			bool parseResponse(const San2::Utils::bytes &in);
			bool getCapsule(San2::Network::CCapsule &capsule);
			
			bool pack(San2::Utils::bytes &out);
		
		private:
			SAN_UINT32 m_timeout;
			San2::Utils::bytes m_response;
		};
	}
}
