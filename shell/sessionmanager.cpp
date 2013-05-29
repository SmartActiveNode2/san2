
#include <utility>
#include <memory>
#include "sessionmanager.hpp"

#define SESSION_TIMEOUT 3600

std::shared_ptr<Session> SessionManager::getSession(const San2::Network::SanAddress &sourceAddress, SAN_UINT16 sourcePort)
{	
	std::map<std::pair<San2::Network::SanAddress, SAN_UINT16>, std::shared_ptr<Session> >::iterator iter = sessionStorage.find(std::make_pair(sourceAddress, sourcePort));
	if (iter != sessionStorage.end()) return iter->second;
	std::shared_ptr<Session> createdSession(new Session(SESSION_TIMEOUT));
	std::pair<std::map<std::pair<San2::Network::SanAddress, SAN_UINT16>, std::shared_ptr<Session> >::iterator, bool> ret = sessionStorage.insert(std::make_pair(std::make_pair(sourceAddress, sourcePort), createdSession));
	if (ret.second == false) return iter->second;
	return createdSession;
}

std::size_t SessionManager::sessionCount()
{
	return sessionStorage.size();
}
