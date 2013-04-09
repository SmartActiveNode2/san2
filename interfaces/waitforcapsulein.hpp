
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
		class WaitForCapsuleIn : public San2::Rpc::CIRpcSyncFunctionIn
		{
		public:
			static const int timeoutMilisec;
		
			// Parametrem je producent konzument
			WaitForCapsuleIn(San2::Utils::CProducerConsumer<std::shared_ptr<San2::Network::CCapsule> > *queue, San2::Utils::CThread* thr); // must be mutexed inside portmapper!!!
			unsigned int getUniqueId()const;
			bool operator()(void);
			bool unpack(const San2::Utils::bytes &in);
			San2::Utils::bytes getResponse();
			
		private:
			San2::Utils::bytes m_response;
			
			SAN_UINT32 m_requestedTimeout;
			San2::Utils::CProducerConsumer<std::shared_ptr<San2::Network::CCapsule> > *m_queue;
			San2::Utils::CThread* m_thr;
		};
	}
}
