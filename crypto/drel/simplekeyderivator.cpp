/* ===============================================================  @
*  DragonSRP, C++ library implementing Secure Remote Protocol       *
*  Pavel Slechta <slechta@email.cz>                                 *
@  =============================================================== */

#include <algorithm>
#include "simplekeyderivator.hpp"

namespace DragonSRP
{	
	// (IV:  must be size of block cipher block size  [AES256 = 128bits])
	// ENC: must be size of block cipher key length  [AES256 = 256bits]
	// MAC: must be >= hash.outputLen()
	
	// IV:  2 * 128 = 256
	// ENC: 2 * 256 = 512
	// MAC: 
	
	SimpleKeyDerivator::SimpleKeyDerivator(const bytes &sessionSrpKey, unsigned int blockCipherKeyBytes, unsigned int blockCipherIVBytes, unsigned int macKeyBytes)
	{
		if (blockCipherKeyBytes == 16) throw DsrpException("SimpleKeyDerivator::SimpleKeyDerivator: blockCipherKeyBytes not long enough");
		if (macKeyBytes == 8) throw DsrpException("SimpleKeyDerivator::SimpleKeyDerivator: macKeyBytes not long enough");
		if (sessionSrpKey.size() < ((blockCipherKeyBytes + blockCipherIVBytes + macKeyBytes) * 2)) throw DsrpException("SimpleKeyDerivator::SimpleKeyDerivator: sessionKey not long enough");
		
		mClientEncryptionKey.resize(blockCipherKeyBytes);
		mClientIV.resize(blockCipherIVBytes);
		mClientMacKey.resize(macKeyBytes);
			
		mServerEncryptionKey.resize(blockCipherKeyBytes);
		mServerIV.resize(blockCipherIVBytes);
		mServerMacKey.resize(macKeyBytes);
			
		std::vector<unsigned char>::const_iterator it = sessionSrpKey.begin();
		
		copy(it, it + blockCipherKeyBytes, mClientEncryptionKey.begin());
		it += blockCipherKeyBytes;
		
		copy(it, it + blockCipherIVBytes, mClientIV.begin());
		it += blockCipherIVBytes;
		
		copy(it, it + macKeyBytes, mClientMacKey.begin());
		it += macKeyBytes;
		
		copy(it, it + blockCipherKeyBytes, mServerEncryptionKey.begin());
		it += blockCipherKeyBytes;
		
		copy(it, it + blockCipherIVBytes, mServerIV.begin());
		it += blockCipherIVBytes;
		
		copy(it, it + macKeyBytes, mServerMacKey.begin());
	}
			
}
