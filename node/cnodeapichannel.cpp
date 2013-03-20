
#include <string>
#include <string.h>

#include "cnodeapichannel.hpp"
#include "utils/log.h"
#include "utils/address.hpp"
#include "utils/cstringutils.hpp"
#include "utils/hex.hpp"
#include "interfaces/sendcapsulefuncin.hpp"
#include "interfaces/sendcapsulefuncout.hpp"
#include "interfaces/alivefuncout.hpp"
#include "comm/cpplstreamrw.hpp"

#include "utils/platform/sleep.hpp"
#include "utils/cvector.hpp"
#include "utils/log.h"
#include "utils/platform/sigignore.hpp"

#include "rpc/crpcexecutor.hpp"
#include "rpc/cirpcchannel.hpp"

#define SAN2_CIPCCHANNEL_MAX_HOPCOUNT 65535
#define SAN2_CIPCCHANNEL_INJECTTIMEOUT_MSEC 2000

/* Provides RPC functions for applications
 * used on server (channel)
 * 
 * 
 * 
*/

namespace San2 { namespace Node {

CNodeApiChannel::CNodeApiChannel(CPPL_PIPETYPE handle, unsigned int timRX, unsigned int timTX, CNode &node) :
	San2::Cppl::PipeChannel(handle, timRX, timTX),
	m_node(node)
{

}

CNodeApiChannel::~CNodeApiChannel()
{
	// empty
}


San2::Cppl::ErrorCode CNodeApiChannel::receive()
{	
	San2::Comm::CpplStreamRW stream(2000, this);
	m_rpcChannel = new San2::Comm::StreamRpcChannel(stream);
	m_rpcexec = new San2::Rpc::CRpcExecutor(*m_rpcChannel, 5000);
	
	San2::Utils::CProducerConsumer<std::shared_ptr<San2::Network::CCapsule> >& inputQueue = m_node.getInputQueue();
	
	//San2::Network::SanAddress ifaceAddress = San2::Network::sanDefaultAddress;
	
	bool ret = m_rpcexec->registerFunction([&inputQueue, this](){return new San2::Interfaces::SendCapsuleFuncIn(San2::Network::sanDefaultAddress, &inputQueue, this);});

	if (!ret)
	{
		FILE_LOG(logERROR) << "CNodeApiChannel::receive(): registrer function *FAILED*";
		return San2::Cppl::ErrorCode::FAILURE;
	}	
	
	return San2::Cppl::ErrorCode::SUCCESS;
}


}} // ns
