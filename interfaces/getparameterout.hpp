
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
		class GetParameterOut : public San2::Rpc::CIRpcSyncFunctionOut
		{
		public:
			GetParameterOut();
			bool setParameter(std::string parameter);			
			unsigned int getUniqueId()const;
			bool operator()(void);
			
			bool parseResponse(const San2::Utils::bytes &in);
			
			std::string getResult();
			
			bool pack(San2::Utils::bytes &out);
		
		private:
			std::string m_parameter;
			std::string m_result;
		};
	}
}
