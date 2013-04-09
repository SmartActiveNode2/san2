
#pragma once

#include "nettypedef.hpp"
#include "utils/platform/basictypes.hpp"

#define SAN_FLAGPOS_DX_POSITION 0
#define SAN_FLAGPOS_DS_POSITION 1

namespace San2
{
	namespace Network
	{
		// const std::size_t minimumCapsuleHeaderSize = sanCapsuleIdSize + (2 * sanAddressSize) + sanHashSize + sanFlagSize + sanHopSize;
		
		// NOTE: This class is NOT thread-safe
		//       only one thread should access instance
		//       of this class at given time
		//       HOWEVER this class MAY be SAFELY passed
		//       across threads using CProducentConsumer
		//       class. When passing this class to
		//       CProducentConsumer it is better to use
		//       a pointer to this class or smart-pointer
		//       (such as std::auto_ptr) as it is much
		//		 faster than copy by value.
		class CCapsule
		{
		public:
			void setFromInterfaceAddress(SanAddress interfaceAddress); // used on localhost only for determining source interface
			void setSourceAddress(SanAddress srcAddress);
			void setDestinationAddress(SanAddress dstAddress);
			void setHop(SAN_UINT16 hop);
			void setDX(bool dx); // Destination eXecute
			void setDS(bool ds); // Destination Send
			void setApplicationId(SanApplicationId appId);
			
			SanAddress getFromInterfaceAddress();
			SanAddress getSourceAddress();
			SanAddress getDestinationAddress();
			SAN_UINT16 getHop();
			bool getDX();
			bool getDS();
			SanApplicationId getAppId();
			
			// NOTE: this is qutie safe as the only
			// possibility is provide data by value
			// and that is what we want in producer-consumer
			// multi-thread environment
			void setData(San2::Utils::bytes data);
			
			// NOTE: Only one thread must access the
			// const data reference only as there is
			// possibility of nasty race conditions
			// when one thread writes when other thread reads
			// the only reason why I use pass by reference and
			// not pass by value is performance!
			const San2::Utils::bytes& getData();
			
			// Pack the capsule as byte array
			void pack(San2::Utils::bytes &out);
			
			// Assigns the instance of the capsule values from byte array
			// TRUE = success
			bool unpack(const San2::Utils::bytes &capsule);
			
			
		protected:
		
		private:
			static San2::Utils::bytes hop2bytes(SAN_UINT16 hop);
			unsigned char constructFlags();
			void parseFlags(unsigned char flags);
		
			bool m_flagDX;
			bool m_flagDS;
		
			SanAddress m_interfaceAddress; // used on localhost only for determining source interface
			SanAddress m_srcAddress;
			SanAddress m_dstAddress;
			SanApplicationId m_appId;
			SAN_UINT16 m_hop;
			
			// The payload of capsule
			San2::Utils::bytes m_data;
		};
	
		
	}
}
