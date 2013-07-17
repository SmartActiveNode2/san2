
#include <algorithm>
#include <assert.h>
#include "getparameterout.hpp"
#include "utils/cdatapack.hpp"
#include "network/ccapsule.hpp"
#include "network/nettypedef.hpp"
#include "utils/log.h"

namespace San2 { namespace Interfaces {

GetParameterOut::GetParameterOut()
{
	
}

bool GetParameterOut::setParameter(std::string parameter)
{
	m_parameter = parameter;
	return true;
}

unsigned int GetParameterOut::getUniqueId()const
{
    // FuncIn must match FunOut ID
	return 4333; // TODO: replace by global define file of func ids
}

bool GetParameterOut::pack(San2::Utils::bytes &out)
{
	out = m_parameter;
	return true;
}

bool GetParameterOut::parseResponse(const San2::Utils::bytes &in)
{
	m_result = San2::Utils::bytes::bytes2string(in);
	return true;
}

std::string GetParameterOut::getResult()
{		
	return m_result;
}

}} // ns
