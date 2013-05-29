

#include "cnodeconnector.hpp"
#include "cppl/pipeclient.hpp"
#include "utils/platform/sigignore.hpp"
#include "network/nettypedef.hpp"
#include "utils/address.hpp"
#include "utils/log.h"

#include "interfaces/sendcapsulefuncout.hpp"
#include "interfaces/registerout.hpp"
#include "interfaces/registerephemeralout.hpp"
#include "interfaces/waitforcapsuleout.hpp"
#include "interfaces/getaddressesout.hpp"

#define SAN2_CNODECONNECTOR_MAXSINGLEREADSIZE 2048

namespace San2 { namespace Api {
	
CNodeConnector::CNodeConnector(const char *pipeName, unsigned int timCON, unsigned int timRX, unsigned int timTX, unsigned int timRPC_RX) :
	San2::Cppl::PipeClient(pipeName, timCON, timRX, timTX),
	m_timRPC_RX(timRPC_RX),
	m_stream(NULL),
	m_rpcChannel(NULL),
	m_rpci(NULL)
{
	
}

void CNodeConnector::cleanup()
{
	if (m_stream != NULL) free(m_stream);
	if (m_rpcChannel != NULL) free(m_rpcChannel);
	if (m_rpci != NULL) free(m_rpci);
}

CNodeConnector::~CNodeConnector()
{
	cleanup();
}
	
San2::Cppl::ErrorCode CNodeConnector::receive()
{
	return San2::Cppl::ErrorCode::SUCCESS;
}
	
// returns true on success
bool CNodeConnector::connect()
{
	cleanup();
	
	#ifdef LINUX
		San2::Utils::san_ignore_sigpipe();
	#endif
	
	m_stream = new San2::Comm::CpplStreamRW(SAN2_CNODECONNECTOR_MAXSINGLEREADSIZE, this);
	m_rpcChannel = new San2::Comm::StreamRpcChannel(*m_stream);
	m_rpci = new San2::Rpc::CRpcInvoker(*m_rpcChannel, m_timRPC_RX);

    
	bool ret = m_rpci->registerFunction([](){return new San2::Interfaces::SendCapsuleFuncOut;});
	if (!ret)
	{
		FILE_LOG(logERROR) << "CNodeConnector::run(): registrer function FAILED";
		return false;
	}	
	
	ret = m_rpci->registerSyncFunction([](){return new San2::Interfaces::RegisterOut;});
	if (!ret)
	{
		FILE_LOG(logERROR) << "CNodeConnector::run(): registrer function FAILED";
		return false;
	}	

	ret = m_rpci->registerSyncFunction([](){return new San2::Interfaces::WaitForCapsuleOut;});
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
	
bool CNodeConnector::registerPort(SAN_UINT16 port)
{
	San2::Interfaces::RegisterOut func;
	func.setPort(port);
	if (m_rpci->invokeSyncFunction(func) == false) return false;
	return func.wasRegistered();
}
	
SAN_INT32 CNodeConnector::waitForCapsule(San2::Network::CCapsule &capsule, SAN_UINT32 timeout)
{
	San2::Interfaces::WaitForCapsuleOut func;
	func.setTimeout(timeout);
	if (m_rpci->invokeSyncFunction(func) == false) return SAN2_WAITFORCAPSULE_ERROR_INVOKE_FAILED;
	return func.getCapsule(capsule);
}

SAN_UINT16 CNodeConnector::getEphemeralPort()
{
	San2::Interfaces::RegisterEphemeralOut func;
	if (m_rpci->invokeSyncFunction(func) == false) return 0;
	return func.getPort();
}	


unsigned int CNodeConnector::getInterfaceAddresses(std::list<San2::Network::SanAddress> &adrs)
{
	San2::Interfaces::GetAddressesOut func;
	if (m_rpci->invokeSyncFunction(func) == false) return -2;
	return func.getAddresses(adrs);
}
	
}} // ns
