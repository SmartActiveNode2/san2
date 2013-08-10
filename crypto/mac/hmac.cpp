#include <string>
#include <iostream>
#include <vector>

#include "hmac.hpp"
#include "../dsrp/conversion.hpp"
#include "macexception.hpp"

/***********************************************************************
 * RFC 2014 Page 3
   Keys need to be chosen at random (or using a cryptographically strong
   pseudo-random generator seeded with a random seed), and periodically
   refreshed.  (Current attacks do not indicate a specific recommended
   frequency for key changes as these attacks are practically
   infeasible.  However, periodic key refreshment is a fundamental
   security practice that helps against potential weaknesses of the
   function and keys, and limits the damage of an exposed key.)
 * *********************************************************************/

namespace DragonSRP
{
	Hmac::Hmac(HashInterface &hashInterface, const bytes &key): hash(hashInterface), mKey(key)
	{
		unsigned int padSize = hash.blockSize();
		
		if (mKey.size() == 0) throw MacException("Mac - empty key provided.");
		//if (mKey.size() < hash.outputLen()) throw MacException("Mac - short key provided.");
		if (mKey.size() > padSize) throw MacException("Mac - too long key provided. Hash it first.");
		
		mKey.reserve(padSize);
		// expand key to match hash block size with zeros
		mKey.resize(padSize, 0x00);
		
		ikeypad.reserve(padSize);
		okeypad.reserve(padSize);

		for (unsigned int i = 0; i < padSize; i++)
		{
			ikeypad.push_back(mKey[i] ^ 0x36);
			okeypad.push_back(mKey[i] ^ 0x5c);
		}
	}
	
	void Hmac::hmac(const bytes &data, bytes &mac)
	{
		bytes toHash, res;

		toHash.reserve(ikeypad.size() + data.size());
		Conversion::append(toHash, ikeypad);
		Conversion::append(toHash, data);
		
		res.resize(hash.outputLen());
		hash.hash(&toHash[0], hash.outputLen(), &res[0]);
	
		mac.resize(hash.outputLen());
		hmac(&data[0], data.size(), &mac[0]);
	}
	
	
	void Hmac::hmac(const unsigned char *in, unsigned int inLen, unsigned char *mac)
	{
		DragonSRP::bytes toHash;
		toHash.resize(ikeypad.size() + inLen);
		// unsigned char toHash[ikeypad.size() + inLen];
		memcpy(&toHash[0], &ikeypad[0], ikeypad.size());
		memcpy(&toHash[0] + ikeypad.size(), &in[0], inLen);
		
		DragonSRP::bytes res;
		res.resize(hash.outputLen());

		hash.hash(&toHash[0], ikeypad.size() + inLen, &res[0]);
		memcpy(&toHash[0], &okeypad[0], okeypad.size());
		memcpy(&toHash[0] + okeypad.size(), &res[0], hash.outputLen());
		hash.hash(&toHash[0], okeypad.size() + hash.outputLen(), mac);
	}
	
	unsigned int Hmac::outputLen()
	{
		return hash.outputLen();
	}
}
