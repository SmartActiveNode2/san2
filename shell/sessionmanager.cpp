
#include <utility>
#include <memory>
#include "sessionmanager.hpp"

std::shared_ptr<Session> SessionManager::getSession(San2::Utils::bytes sessionId)
{	
	std::map<San2::Utils::bytes, std::shared_ptr<Session> >::iterator iter = sessionStorage.find(sessionId);
	if (iter != sessionStorage.end()) return iter->second;
	std::shared_ptr<Session> createdSession(new Session(3600));
	std::pair<std::map<San2::Utils::bytes, std::shared_ptr<Session> >::iterator, bool> ret = sessionStorage.insert(std::make_pair(sessionId, createdSession));
	if (ret.second == false) throw "Fatal error, inserted element is a duplicate.";
	return createdSession;
}

std::size_t SessionManager::sessionCount()
{
	return sessionStorage.size();
}
