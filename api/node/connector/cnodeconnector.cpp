

#include "cnodeconnector.hpp"
#include "comm/cpplstreamrw.hpp"
#include "cppl/pipeclient.hpp"
#include "utils/platform/sigignore.hpp"
#include "network/nettypedef.hpp"
#include "utils/address.hpp"
#include "interfaces/sendcapsulefuncout.hpp"
#include "utils/log.h"

#define SAN2_CNODECONNECTOR_MAXSINGLEREADSIZE 2048

namespace San2 { namespace Api {
	
CNodeConnector::CNodeConnector(const char *pipeName, unsigned int timCON, unsigned int timRX, unsigned int timTX, unsigned int timRPC_RX) :
	San2::Cppl::PipeClient(pipeName, timCON, timRX, timTX),
	m_timRPC_RX(timRPC_RX)
{
	
}

CNodeConnector::~CNodeConnector()
{
	
}
	
San2::Cppl::ErrorCode CNodeConnector::receive()
{
	return San2::Cppl::ErrorCode::SUCCESS;
}
	
// returns true on success
bool CNodeConnector::connect()
{
	#ifdef LINUX
		San2::Utils::san_ignore_sigpipe();
	#endif
	
	San2::Comm::CpplStreamRW stream(SAN2_CNODECONNECTOR_MAXSINGLEREADSIZE, this);
	m_rpcChannel = new San2::Comm::StreamRpcChannel(stream);
	m_rpci = new San2::Rpc::CRpcInvoker(*m_rpcChannel, m_timRPC_RX);

    
	bool ret = m_rpci->registerFunction([](){return new San2::Interfaces::SendCapsuleFuncOut;});
	if (!ret)
	{
		FILE_LOG(logERROR) << "CNodeConnector::run(): registrer function FAILED";
		return false;
	}	

	return true;
}

bool CNodeConnector::sendCapsule(San2::Utils::bytes &capsuleData)
{
	San2::Interfaces::SendCapsuleFuncOut func;
	func.setCapsuleToSend(capsuleData);
	return m_rpci->invokeFunction(func);
}
	
}} // ns
