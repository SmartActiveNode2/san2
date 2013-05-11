
#include <memory>
#include <list>

#include "rpc/cirpcsyncfunctionout.hpp"
#include "utils/cproducerconsumer.hpp"
#include "network/ccapsule.hpp"
#include "network/nettypedef.hpp"
#include "utils/cvector.hpp"
#include "utils/platform/basictypes.hpp"
#include "utils/address.hpp"

namespace San2 {
	namespace Interfaces
	{
		class GetAddressesOut : public San2::Rpc::CIRpcSyncFunctionOut
		{
		public:
			static const int timeoutMilisec;
		
			GetAddressesOut();
			
			// internal
			unsigned int getUniqueId()const;
			bool operator()(void);
			bool parseResponse(const San2::Utils::bytes &in);
			bool pack(San2::Utils::bytes &out);
			
			// external
			unsigned int getAddresses(std::list<San2::Network::SanAddress> &adrs);
		private:
			San2::Utils::bytes m_response;
		
			SAN_UINT16 m_port;
			SAN_INT32 m_result;
		};
	}
}
