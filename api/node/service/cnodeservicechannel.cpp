
#include <string>
#include <string.h>

#include "cnodeservicechannel.hpp"

// utils
#include "utils/log.h"
#include "utils/address.hpp"
#include "utils/cstringutils.hpp"
#include "utils/hex.hpp"
#include "utils/platform/sleep.hpp"
#include "utils/cvector.hpp"
#include "utils/platform/sigignore.hpp"

// functions
#include "interfaces/sendcapsulefuncin.hpp"
#include "interfaces/sendcapsulefuncout.hpp"
#include "interfaces/alivefuncout.hpp"
#include "interfaces/waitforcapsulein.hpp"
#include "interfaces/registerin.hpp"
#include "interfaces/registerephemeralin.hpp"
#include "interfaces/getaddressesin.hpp"

// rpc
#include "rpc/crpcexecutor.hpp"
#include "rpc/cirpcchannel.hpp"

// rpc adapters
#include "comm/cpplstreamrw.hpp"

#define SAN2_CIPCCHANNEL_MAX_HOPCOUNT 65535
#define SAN2_CIPCCHANNEL_INJECTTIMEOUT_MSEC 2000

#define SAN2_CNODESERVICECHANNEL_APPQUEMAXLEN 20

namespace San2 { namespace Api {

CNodeServiceChannel::CNodeServiceChannel(CPPL_PIPETYPE handle, unsigned int timRX, unsigned int timTX, San2::Utils::CProducerConsumer<std::shared_ptr<San2::Network::CCapsule> >& nodeQueue, San2::Node::CPortmap& portmap, San2::Node::CNode &node) :
	San2::Cppl::PipeChannel(handle, timRX, timTX),
	m_nodeQueue(nodeQueue),
	m_portmap(portmap),
	m_applicationQueue(SAN2_CNODESERVICECHANNEL_APPQUEMAXLEN),
	m_node(node)
{

}

CNodeServiceChannel::~CNodeServiceChannel()
{
	// empty
}


San2::Cppl::ErrorCode CNodeServiceChannel::receive() // required
{	
	// TODO: add sigignore SIGPIPE
	
	San2::Comm::CpplStreamRW stream(2000, this);
	m_rpcChannel = new San2::Comm::StreamRpcChannel(stream);
	m_rpcexec = new San2::Rpc::CRpcExecutor(*m_rpcChannel, 5000);

	bool ret = m_rpcexec->registerFunction([&, this](){return new San2::Interfaces::SendCapsuleFuncIn(San2::Network::sanDefaultAddress, &m_nodeQueue, this);});

	if (!ret)
	{
		FILE_LOG(logERROR) << "CNodeApiChannel::receive(): registrer function *FAILED*";
		return San2::Cppl::ErrorCode::FAILURE;
	}	
	
	CThread *thr = this;
	ret = m_rpcexec->registerSyncFunction([&, thr](){return new San2::Interfaces::WaitForCapsuleIn(&m_applicationQueue, thr);});

	if (!ret)
	{
		FILE_LOG(logERROR) << "CNodeApiChannel::receive(): registrer function *FAILED*";
		return San2::Cppl::ErrorCode::FAILURE;
	}	
	
	ret = m_rpcexec->registerSyncFunction([this](){return new San2::Interfaces::RegisterIn(m_portmap, m_applicationQueue);});

	if (!ret)
	{
		FILE_LOG(logERROR) << "CNodeApiChannel::receive(): registrer function *FAILED*";
		return San2::Cppl::ErrorCode::FAILURE;
	}	
	
	ret = m_rpcexec->registerSyncFunction([this](){return new San2::Interfaces::RegisterEphemeralIn(m_portmap, m_applicationQueue);});

	if (!ret)
	{
		FILE_LOG(logERROR) << "CNodeApiChannel::receive(): registrer function *FAILED*";
		return San2::Cppl::ErrorCode::FAILURE;
	}	
	
	ret = m_rpcexec->registerSyncFunction([this](){return new San2::Interfaces::GetAddressesIn(m_node);});

	if (!ret)
	{
		FILE_LOG(logERROR) << "CNodeApiChannel::receive(): registrer function *FAILED*";
		return San2::Cppl::ErrorCode::FAILURE;
	}	
	
	m_rpcexec->run();
	
	// Unregister all ports, that application registred
	m_portmap.freePorts(&m_applicationQueue);
	return San2::Cppl::ErrorCode::SUCCESS;
}


}} // ns
