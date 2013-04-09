			
#include "ccapsule.hpp"
#include "utils/flag.hpp"												
			
namespace San2 { namespace Network {			
						
void CCapsule::setSourceAddress(SanAddress srcAddress)
{
	m_srcAddress = srcAddress;
}

void CCapsule::setDestinationAddress(SanAddress dstAddress)
{
	m_dstAddress = dstAddress;
}

void CCapsule::setHop(SAN_UINT16 hop)
{
	m_hop = hop;
}
						
void CCapsule::setDX(bool dx)
{
	m_flagDX = dx;
}		

void CCapsule::setDS(bool ds)
{
	m_flagDX = ds;
}					
	
bool CCapsule::getDX()
{
	return m_flagDX;
}		

bool CCapsule::getDS()
{
	return m_flagDS;
}		

SanAddress CCapsule::getSourceAddress()
{
	return m_srcAddress;
}

SanAddress CCapsule::getDestinationAddress()
{
	return m_dstAddress;
}

SAN_UINT16 CCapsule::getHop()
{
	return m_hop;
}

void CCapsule::setData(San2::Utils::bytes data)
{
	m_data = data; // so nice
}

const San2::Utils::bytes& CCapsule::getData()
{
	return m_data;
}

void CCapsule::pack(San2::Utils::bytes &out)
{
	out = San2::Utils::arr2vec(m_dstAddress) + San2::Utils::arr2vec(m_srcAddress) + hop2bytes(m_hop) + constructFlags() + San2::Utils::arr2vec(m_appId) + m_data;
}

bool CCapsule::unpack(const San2::Utils::bytes &capsule)
{
    //                        dstAddr  srcAddr       hops                  flags                 appId
    unsigned int headerSize = 2 * sanAddressSize + sizeof(SAN_UINT16) + sizeof(unsigned char) + sanHashSize;
	
	if (capsule.size() < headerSize) return false;
	
	San2::Utils::bytes::const_iterator from = capsule.begin();
	// dstAddr
	std::copy(from, capsule.begin() + sanAddressSize, m_dstAddress.begin());
	from += sanAddressSize;
	
	// srcAddr
	std::copy(from, capsule.begin() + sanAddressSize, m_srcAddress.begin());
	from += sanAddressSize;
	
	// hops
	memcpy(&m_hop, &capsule[0] + 2 * sanAddressSize, sizeof(SAN_UINT16));
	m_hop = San2::Utils::Endian::san_u_be16toh(m_hop);
	from += sizeof(SAN_UINT16);
	
	// flags
	parseFlags(capsule[2 * sanAddressSize + sizeof(SAN_UINT16)]);
	from += sizeof(unsigned char);
	
	// hash
	std::copy(from, from + sanHashSize, m_appId.begin());
	from += sanHashSize;
	
	// data
	m_data.resize(capsule.size() - headerSize);
	std::copy(from, capsule.end(), m_data.begin());
	return true;
}

San2::Utils::bytes CCapsule::hop2bytes(SAN_UINT16 hop)
{
	// Ugly but i dont know, how to do it better
	SAN_UINT16 netorder;
	unsigned char tmp[sizeof(SAN_UINT16)];
	netorder = San2::Utils::Endian::san_u_htobe16(hop);
	memcpy(tmp, &netorder, sizeof(SAN_UINT16));
	San2::Utils::bytes b(tmp, tmp + sizeof(SAN_UINT16));
	return b;
}

void San2::Network::CCapsule::setApplicationId(San2::Network::SanApplicationId appId)
{
	m_appId = appId;
}

SanApplicationId CCapsule::getAppId()
{
	return m_appId;
}

unsigned char CCapsule::constructFlags()
{
	unsigned char flags = 0; // 0 - null all flags
	San2::Utils::Flag::setFlag(flags, SAN_FLAGPOS_DX_POSITION, m_flagDX);
	San2::Utils::Flag::setFlag(flags, SAN_FLAGPOS_DS_POSITION, m_flagDS);
	return flags;
}

void CCapsule::parseFlags(unsigned char flags)
{
	m_flagDX = San2::Utils::Flag::getFlag(flags, SAN_FLAGPOS_DX_POSITION);
	m_flagDS = San2::Utils::Flag::getFlag(flags, SAN_FLAGPOS_DS_POSITION);
}

void CCapsule::setFromInterfaceAddress(SanAddress interfaceAddress)
{
	m_interfaceAddress = interfaceAddress;
}

SanAddress CCapsule::getFromInterfaceAddress()
{
	return m_interfaceAddress;
}

}} // ns
