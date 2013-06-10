#pragma once

#include <map>
#include <memory>
#include <utility>

#include "utils/cvector.hpp"
#include "utils/platform/basictypes.hpp"
#include "network/nettypedef.hpp"

template <class T> class SessionManager
{
private:
	std::map<std::pair<San2::Network::SanAddress, SAN_UINT16>, std::shared_ptr<T> > sessionStorage;
	std::function<T* (const San2::Network::SanAddress&, SAN_UINT16)> m_createT;
	unsigned int m_sessionTimeoutSeconds;
protected:

public:
	SessionManager(std::function<T* (const San2::Network::SanAddress&, SAN_UINT16)> createT, unsigned int sessionTimeoutSeconds) :
		m_createT(createT),
		m_sessionTimeoutSeconds(sessionTimeoutSeconds)
	{
		
	}
	
	std::shared_ptr<T> getSession(San2::Network::SanAddress sourceAddress, SAN_UINT16 sourcePort)
	{	
		typename std::map<std::pair<San2::Network::SanAddress, SAN_UINT16>, std::shared_ptr<T> >::iterator iter;
		iter = sessionStorage.find(std::make_pair(sourceAddress, sourcePort));
		if (iter != sessionStorage.end()) return iter->second;
		std::shared_ptr<T> createdSession(m_createT(sourceAddress, sourcePort));
		// std::pair<std::map<std::pair<San2::Network::SanAddress, SAN_UINT16>, std::shared_ptr<T> >::iterator, bool> ret;
		auto ret = sessionStorage.insert(std::make_pair(std::make_pair(sourceAddress, sourcePort), createdSession));
		if (ret.second == false) return iter->second;
		return createdSession;
		
	}
	
	std::size_t sessionCount()
	{
		return sessionStorage.size();
	}
};
