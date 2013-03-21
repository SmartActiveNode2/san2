
#include <memory>
#include "rpc/cirpcfunctionout.hpp"
#include "utils/cproducerconsumer.hpp"
#include "network/ccapsule.hpp"
#include "network/nettypedef.hpp"
#include "utils/cvector.hpp"

namespace San2 {
	namespace Interfaces
	{
		class SendCapsuleFuncOut : public San2::Rpc::CIRpcFunctionOut
		{
		public:
			static const int timeoutMilisec;
		
			SendCapsuleFuncOut();
			bool setCapsuleToSend(std::shared_ptr<San2::Network::CCapsule> capsule);
			void setCapsuleToSend(San2::Utils::bytes &packedCaplsule);
			unsigned int getUniqueId()const;
			bool operator()(void);
			
			bool pack(San2::Utils::bytes &out);
			
		private:
			San2::Utils::bytes m_SerializedCapsule;
		};
	}
}
