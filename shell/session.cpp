

#include <iostream>
#include "session.hpp"

Session::Session(San2::Api::CNodeConnector &connector, const San2::Network::SanAddress& serverAddress, SAN_UINT16 serverPort, const San2::Network::SanAddress& clientAddress, SAN_UINT16 clientPort) :
	StopWaitRx(connector, serverAddress, serverPort, clientAddress, clientPort)
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
