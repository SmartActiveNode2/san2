/* ===============================================================  @
*  DragonSRP, C++ library implementing Secure Remote Protocol       *
*  Pavel Slechta <slechta@email.cz>                                 *
@  =============================================================== */

#include "srpclient.hpp"

namespace DragonSRP
{
	SrpClient::SrpClient(MathInterface &mathInterface, RandomInterface &randomInterface, bool interleaveKey) :
		math(mathInterface),
		random(randomInterface),
		interleave(interleaveKey)
	{
			
	}
	
	SrpClientAuthenticator SrpClient::getAuthenticator(bytes username, bytes password)
	{
		bytes a = random.getRandom(32); // length of salt, needs to be parametrized!
		bytes A = math.calculateA(a);
		return SrpClientAuthenticator(username, password, a, A);
	}
	
	#ifdef DSRP_DANGEROUS_TESTING
		// injects custom a; used for testing rfc vectors
		SrpClientAuthenticator SrpClient::getAuthenticator(bytes username, bytes password, bytes a)
		{
			
			bytes A = math.calculateA(a);
			return SrpClientAuthenticator(username, password, a, A);
		}
	#endif
	
	bytes SrpClient::getM1(bytes salt, bytes B, SrpClientAuthenticator &sca)
	{
		bytes M1, M2, K;
		math.clientChallange(salt, sca.a, sca.A, B, sca.username, sca.password, M1, M2, K, interleave);
		sca.M2_calculated = M2;
		sca.K = K;
		return M1;
	}
}
