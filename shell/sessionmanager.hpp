#pragma once

#include <map>
#include <memory>
#include <utils/cvector.hpp>

#include "session.hpp"

class SessionManager
{
public:
	SessionManager();
	std::shared_ptr<Session> getSession(San2::Utils::bytes sessionId);
	std::size_t sessionCount();
protected:

private:
	std::map<San2::Utils::bytes, std::shared_ptr<Session> > sessionStorage;
	
};
