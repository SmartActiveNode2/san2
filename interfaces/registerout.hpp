
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
		class RegisterOut : San2::Rpc::CIRpcSyncFunctionOut
		{
		public:
			static const int timeoutMilisec;
		
			RegisterOut();
			bool setPort(SAN_UINT16 port);			
			unsigned int getUniqueId()const;
			bool operator()(void);
			
			bool parseResponse(const San2::Utils::bytes &in);
			bool getResult();
			
			bool pack(San2::Utils::bytes &out);
		
		private:
			SAN_UINT16 m_port;
			SAN_INT32 m_result;
		};
	}
}
