

#include <iostream>
#include "session.hpp"

Session::Session(const San2::Network::SanAddress& srcAddress, SAN_UINT16 srcPort) :
	m_srcAddress(srcAddress),
	m_srcPort(srcPort)
{
	
}

bool Session::processDatagram(SAN_UINT64 sequenceNummber, const San2::Utils::bytes& request, San2::Utils::bytes& response)
{
	std::cout << "Session::processDatagram()" << std::endl;
	response = "This is some response";
	return true;
}


int Session::getState()
{
	return m_state;
}
