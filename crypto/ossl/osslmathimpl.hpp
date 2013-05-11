/* ===============================================================  @
*  DragonSRP, C++ library implementing Secure Remote Protocol       *
*  Pavel Slechta <slechta@email.cz>                                 *
@  =============================================================== */

/* ===============================================================  @
*  This product includes software developed by the OpenSSL Project  *
*  for use in the OpenSSL Toolkit. (http://www.openssl.org/)        *
*                                                                   *
*  This product includes cryptographic software                     *
*  written by Eric Young (eay@cryptsoft.com)                        *
*                                                                   *
*  This product includes software                                   *
*  written by Tim Hudson (tjh@cryptsoft.com)                        *
@  =============================================================== */


#ifndef DSRP_OSSLMATHIMPL_HPP
#define DSRP_OSSLMATHIMPL_HPP

#include <openssl/bn.h>
#include <openssl/sha.h>
#include <openssl/crypto.h>
#include <openssl/rand.h>

#include "../dsrp/common.hpp"
#include "../dsrp/mathinterface.hpp"
#include "../dsrp/ng.hpp"
#include "../dsrp/conversion.hpp"

#include "../dsrp/dsrpexception.hpp"

namespace DragonSRP
{
namespace Ossl
{
	
	class OsslMathImpl : public MathInterface
	{
		public:
			OsslMathImpl(HashInterface &hashInterface, Ng ngVal);
			~OsslMathImpl();
			bytes calculateA(const bytes &aa);
			void clientChallange(const bytes &salt, const bytes &aa, const bytes &AA, const bytes &BB, const bytes &username, const bytes &password, bytes &M1_out, bytes &M2_out, bytes &K_out, bool interleave = false);
			void serverChallange(const bytes &username, const bytes &salt, const bytes &verificator, const bytes &AA, const bytes &bb, bytes &B_out, bytes &M1_out, bytes &M2_out, bytes &K_out, bool interleave = false);
			bytes calculateVerificator(const bytes &username, const bytes &password, const bytes &salt);
			
			#ifdef DSRP_DANGEROUS_TESTING
				bytes S_client_premaster_secret;
				bytes S_server_premaster_secret;
				bytes u_server;
				bytes k_math;
				bytes x_client;
				bytes x_vgen;
			#endif
			
		private:
			bytes calculateM1(const bytes &username, const bytes &s, const bytes &A, const bytes &B, const bytes &K);
			void checkNg();
			
			void interleaveS(const bytes &S, bytes &K); // input S, returns K
			
			BIGNUM *N;
			BIGNUM *g;
			BIGNUM *k;
			BN_CTX *ctx;
	};
	
// Namespace endings
}
}

#endif
