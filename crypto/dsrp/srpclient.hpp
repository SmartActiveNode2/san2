/* ===============================================================  @
*  DragonSRP, C++ library implementing Secure Remote Protocol       *
*  Pavel Slechta <slechta@email.cz>                                 *
@  =============================================================== */

#ifndef DSRP_SRPCLIENT_HPP
#define DSRP_SRPCLIENT_HPP

#include "common.hpp"

#include "mathinterface.hpp"
#include "randominterface.hpp"

#include "srpclientauthenticator.hpp"

namespace DragonSRP
{	
	class SrpClient
	{		
		public:
			SrpClient(MathInterface &mathInterface, RandomInterface &randomInterface, bool interleaveKey = false);
			SrpClientAuthenticator getAuthenticator(bytes username, bytes password);
			bytes getM1(bytes salt, bytes B, SrpClientAuthenticator &sca);
	
			#ifdef DSRP_DANGEROUS_TESTING
				// injects custom a; used for testing rfc vectors
				SrpClientAuthenticator getAuthenticator(bytes username, bytes password, bytes a);
			#endif
			
		private:
			MathInterface &math;
			RandomInterface &random;	
			bool interleave;
	};
}

#endif
