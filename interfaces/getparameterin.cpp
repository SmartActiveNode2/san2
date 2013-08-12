
#include <algorithm>
#include <assert.h>
#include <set>
#include <memory>
#include "getparameterin.hpp"
#include "utils/cdatapack.hpp"
#include "network/ccapsule.hpp"
#include "network/nettypedef.hpp"
#include "utils/log.h"

namespace San2 { namespace Interfaces {

namespace Node {class CNode;} // forward declaration

const int GetParameterIn::timeoutMilisec = 3000;

GetParameterIn::GetParameterIn(San2::Node::CNode& node, San2::Node::CPortmap& portmapper):
	m_node(node),
	m_portmapper(portmapper)
{
	
}

unsigned int GetParameterIn::getUniqueId() const
{
	return 4333; // TODO: replace by global define file of func ids
}

bool GetParameterIn::operator()(void)
{	
	if (m_parameter.compare("hello") == 0)
	{
		m_response = "Welcome to SAN node terminal";
		return true;
	}
	
	if (m_parameter.compare("hostname") == 0)
	{
		m_response = m_node.getNodeName();
		return true;
	}
	
	if (m_parameter.compare("uptime") == 0)
	{
		unsigned long uptimeSec = m_node.getUptimeSec();
		
		unsigned long uptimeDays = (uptimeSec - (uptimeSec % 86400)) / 86400;
		uptimeSec %= 86400; 
		unsigned long uptimeHours = (uptimeSec - (uptimeSec % 3600)) / 3600;
		uptimeSec %= 3600; 
		unsigned long uptimeMin = (uptimeSec - (uptimeSec % 60)) / 60;
		uptimeSec %= 60; 
		
		std::string tmp;
		
		tmp = San2::Utils::CStringUtils::ulongToString(uptimeDays) + "d " + San2::Utils::CStringUtils::ulongToString(uptimeHours) + "h " + San2::Utils::CStringUtils::ulongToString(uptimeMin) + "m " + San2::Utils::CStringUtils::ulongToString(uptimeSec) + "s";
		m_response = tmp;
		return true;
	}
	
	if (m_parameter.compare("peers") == 0)
	{
	    std::set<std::shared_ptr<San2::Network::CNetInterface> > ifaces = m_node.getInterfaces();
       
        m_response = "";
       
        for(auto s: ifaces)
        {
            
            m_response += "IFACE status RX:";
            m_response += statusToString(s->getRXstate());
            m_response += "   TX:";
            m_response += statusToString(s->getTXstate());
            m_response += "\n>> iadr: ";
            m_response += San2::Utils::address2string(s->getInterfaceAddress()).c_str();
            m_response += "\n>> peer: ";
            m_response += San2::Utils::address2string(s->getPeerAddress()).c_str();
            m_response += "\n\n";            
         }

		return true;
	}
	
	
	m_response = "Unknown parameter " + m_parameter;
	
	return true;
}

bool GetParameterIn::unpack(const San2::Utils::bytes &in)
{	
	m_parameter = San2::Utils::bytes::bytes2string(in);
	return true;
}

San2::Utils::bytes GetParameterIn::getResponse()
{
	return m_response;
}

const char* GetParameterIn::statusToString(San2::Network::InterfaceLineStatus status)
{
    switch(status)
    {
    case San2::Network::InterfaceLineStatus::CONNECTED:
        return "conn";
    case San2::Network::InterfaceLineStatus::DISCONNECTED:
        return "diss";
    case San2::Network::InterfaceLineStatus::FAILURE:
        return "fail";
    default: // should never happen
        return "????";
    }
}

}} // ns
