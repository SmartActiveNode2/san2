
#include "cdatapack.hpp"

namespace San2 { namespace Utils {

San2::Utils::bytes CDataPack::pack(SAN_INT16 num)
{
	San2::Utils::bytes b;
	char tmp[sizeof(SAN_INT16)];
	num = San2::Utils::Endian::san_s_htobe16(num);
	memcpy(tmp, &num, sizeof(SAN_INT16));
	copy(tmp, tmp + sizeof(SAN_INT16), std::back_inserter(b));
	return b;
}

San2::Utils::bytes CDataPack::pack(SAN_INT32 num)
{
	San2::Utils::bytes b;
	char tmp[sizeof(SAN_INT32)];
	num = San2::Utils::Endian::san_s_htobe32(num);
	memcpy(tmp, &num, sizeof(SAN_INT32));
	copy(tmp, tmp + sizeof(SAN_INT32), std::back_inserter(b));
	return b;
}

San2::Utils::bytes CDataPack::pack(SAN_INT64 num)
{
	San2::Utils::bytes b;
	char tmp[sizeof(SAN_INT64)];
	num = San2::Utils::Endian::san_s_htobe64(num);
	memcpy(tmp, &num, sizeof(SAN_INT64));
	copy(tmp, tmp + sizeof(SAN_INT64), std::back_inserter(b));
	return b;
}

San2::Utils::bytes CDataPack::pack(SAN_UINT16 num)
{
	San2::Utils::bytes b;
	char tmp[sizeof(SAN_INT16)];
	num = San2::Utils::Endian::san_u_htobe16(num);
	memcpy(tmp, &num, sizeof(SAN_INT16));
	copy(tmp, tmp + sizeof(SAN_INT16), std::back_inserter(b));
	return b;
}

San2::Utils::bytes CDataPack::pack(SAN_UINT32 num)
{
	San2::Utils::bytes b;
	char tmp[sizeof(SAN_INT32)];
	num = San2::Utils::Endian::san_u_htobe32(num);
	memcpy(tmp, &num, sizeof(SAN_INT32));
	copy(tmp, tmp + sizeof(SAN_INT32), std::back_inserter(b));
	return b;
}

San2::Utils::bytes CDataPack::pack(SAN_UINT64 num)
{
	San2::Utils::bytes b;
	char tmp[sizeof(SAN_INT64)];
	num = San2::Utils::Endian::san_u_htobe64(num);
	memcpy(tmp, &num, sizeof(SAN_INT64));
	copy(tmp, tmp + sizeof(SAN_INT64), std::back_inserter(b));
	return b;	
}
	
SAN_INT16 CDataPack::unpackInt16(const San2::Utils::bytes &data, unsigned int position)
{
	if (data.size() < position + sizeof(SAN_INT16)) return 0; // error
	SAN_INT16 num;
	memcpy(&num, &data[position], sizeof(SAN_INT16));
	return San2::Utils::Endian::san_s_be16toh(num);
}

SAN_INT32 CDataPack::unpackInt32(const San2::Utils::bytes &data, unsigned int position)
{
	if (data.size() < position + sizeof(SAN_INT32)) return 0; // error
	SAN_INT32 num;
	memcpy(&num, &data[position], sizeof(SAN_INT32));
	return San2::Utils::Endian::san_s_be32toh(num);
}

SAN_INT64 CDataPack::unpackInt64(const San2::Utils::bytes &data, unsigned int position)
{
	if (data.size() < position + sizeof(SAN_INT64)) return 0; // error
	SAN_INT64 num;
	memcpy(&num, &data[position], sizeof(SAN_INT64));
	return San2::Utils::Endian::san_s_be64toh(num);
}

//-------

SAN_UINT8 CDataPack::unpackUint8(const San2::Utils::bytes &data, unsigned int position)
{
	if (data.size() < position + sizeof(SAN_UINT8)) return 0; // error
	SAN_UINT8 num;
	memcpy(&num, &data[position], sizeof(SAN_UINT8));
	return num;
}


SAN_UINT16 CDataPack::unpackUint16(const San2::Utils::bytes &data, unsigned int position)
{
	if (data.size() < position + sizeof(SAN_UINT16)) return 0; // error
	SAN_UINT16 num;
	memcpy(&num, &data[position], sizeof(SAN_UINT16));
	return San2::Utils::Endian::san_u_be16toh(num);
}

SAN_UINT32 CDataPack::unpackUint32(const San2::Utils::bytes &data, unsigned int position)
{
	if (data.size() < position + sizeof(SAN_UINT32)) return 0; // error
	SAN_UINT32 num;
	memcpy(&num, &data[position], sizeof(SAN_UINT32));
	return San2::Utils::Endian::san_u_be32toh(num);
}

SAN_UINT64 CDataPack::unpackUint64(const San2::Utils::bytes &data, unsigned int position)
{
	if (data.size() < position + sizeof(SAN_UINT64)) return 0; // error
	SAN_UINT64 num;
	memcpy(&num, &data[position], sizeof(SAN_UINT64));
	return San2::Utils::Endian::san_u_be64toh(num);
}

	
}} // ns
