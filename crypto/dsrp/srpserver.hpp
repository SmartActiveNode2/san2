/* ===============================================================  @
*  DragonSRP, C++ library implementing Secure Remote Protocol       *
*  Pavel Slechta <slechta@email.cz>                                 *
@  =============================================================== */

#ifndef DSRP_SRPSERVER_HPP
#define DSRP_SRPSERVER_HPP

#include "common.hpp"

#include "user.hpp"

#include "lookupinterface.hpp"
#include "hashinterface.hpp"
#include "mathinterface.hpp"
#include "randominterface.hpp"
#include "srpverificator.hpp"

#include "ng.hpp"

namespace DragonSRP
{
	class SrpServer
	{	
		public:
			SrpServer(LookupInterface &lookupInterface, MathInterface &mathInterface, RandomInterface &randomInterface, bool interleaveKey = false);
			SrpVerificator getVerificator(const bytes &username, const bytes &AA);
			
			#ifdef DSRP_DANGEROUS_TESTING
				// injects custom b; used for testing rfc vectors
				SrpVerificator getVerificator(const bytes &username, const bytes &AA, const bytes &b);
			#endif
			
		private:
			LookupInterface &lookup;
			MathInterface &math;
			RandomInterface &random;
			bool interleave;
	};
}


#endif
