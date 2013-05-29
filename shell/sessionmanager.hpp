#pragma once

#include <map>
#include <memory>
#include <utils/cvector.hpp>

#include "session.hpp"
#include "utils/platform/basictypes.hpp"
#include "network/nettypedef.hpp"

class SessionManager
{
public:
	SessionManager();
	std::shared_ptr<Session> getSession(const San2::Network::SanAddress &sourceAddress, SAN_UINT16 sourcePort);
	std::size_t sessionCount();
protected:

private:
	std::map<std::pair<San2::Network::SanAddress, SAN_UINT16>, std::shared_ptr<Session> > sessionStorage;
	
};
