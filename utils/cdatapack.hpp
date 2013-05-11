
#pragma once

#include "utils/cvector.hpp"
#include "utils/platform/basictypes.hpp"

namespace San2
{
	namespace Utils
	{
		class CDataPack
		{
		public:
			static San2::Utils::bytes pack(SAN_INT16 num);
			static San2::Utils::bytes pack(SAN_INT32 num);
			static San2::Utils::bytes pack(SAN_INT64 num);
			
			static San2::Utils::bytes pack(SAN_UINT16 num);
			static San2::Utils::bytes pack(SAN_UINT32 num);
			static San2::Utils::bytes pack(SAN_UINT64 num);
			
			// Ugly
			static SAN_INT16 unpackInt16(const San2::Utils::bytes &data, unsigned int position);
			static SAN_INT32 unpackInt32(const San2::Utils::bytes &data, unsigned int position);
			static SAN_INT64 unpackInt64(const San2::Utils::bytes &data, unsigned int position);
			
			static SAN_UINT8  unpackUint8 (const San2::Utils::bytes &data, unsigned int position);
			static SAN_UINT16 unpackUint16(const San2::Utils::bytes &data, unsigned int position);
			static SAN_UINT32 unpackUint32(const San2::Utils::bytes &data, unsigned int position);
			static SAN_UINT64 unpackUint64(const San2::Utils::bytes &data, unsigned int position);
		};

	}
}
