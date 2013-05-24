
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
		class RegisterEphemeralOut : public San2::Rpc::CIRpcSyncFunctionOut
		{
		public:
			static const int timeoutMilisec;
		
			RegisterEphemeralOut();
			unsigned int getUniqueId()const;
			bool operator()(void);
			bool parseResponse(const San2::Utils::bytes &in);
			
			unsigned short int getPort();
			
			bool pack(San2::Utils::bytes &out);
		
		private:
			SAN_UINT16 m_port;
		};
	}
}
