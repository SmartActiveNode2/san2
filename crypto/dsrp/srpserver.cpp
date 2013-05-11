/* ===============================================================  @
*  DragonSRP, C++ library implementing Secure Remote Protocol       *
*  Pavel Slechta <slechta@email.cz>                                 *
@  =============================================================== */

#include "srpserver.hpp"

namespace DragonSRP
{
	
	SrpServer::SrpServer(LookupInterface &lookupInterface, MathInterface &mathInterface, RandomInterface &randomInterface, bool interleaveKey) :
		lookup(lookupInterface),
		math(mathInterface),
		random(randomInterface),
		interleave(interleaveKey)
	{
			
	}	
	
	SrpVerificator SrpServer::getVerificator(const bytes &username, const bytes &AA)
	{
		// check status of authentification
		User usr = lookup.getByName(username); // throws if not found
		bytes M1, M2, K, B;
		bytes b = random.getRandom(32); // length of salt, needs to be parametrized!
		math.serverChallange(username, usr.getSalt(), usr.getVerificator(), AA, b, B, M1, M2, K, interleave); // throws on error
		return SrpVerificator(username, usr.getSalt(), B, M1, M2, K);
	}
			
	#ifdef DSRP_DANGEROUS_TESTING
	    // injects custom b; used for testing rfc vectors
		SrpVerificator SrpServer::getVerificator(const bytes &username, const bytes &AA, const bytes &b)
		{
			// check status of authentification
			User usr = lookup.getByName(username); // throws if not found
			bytes M1, M2, K, B;
			
			math.serverChallange(username, usr.getSalt(), usr.getVerificator(), AA, b, B, M1, M2, K, interleave); // throws on error
			return SrpVerificator(username, usr.getSalt(), B, M1, M2, K);
		}
	#endif
	
}
