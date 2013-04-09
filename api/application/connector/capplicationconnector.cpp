

#include "capplicationconnector.hpp"
#include "interfaces/sendcapsulefuncout.hpp"
#include "comm/cpplstreamrw.hpp"
#include "utils/platform/sleep.hpp"
#include "utils/cvector.hpp"
#include "utils/log.h"
#include "utils/platform/sigignore.hpp"
#include "comm/streamrpcchannel.hpp"
#include "rpc/crpcinvoker.hpp"
#include "interfaces/alivefuncout.hpp"

#define SAN2_CAPPLICATIONCONNECTOR_SINGLE_READ_SIZE 2048


/*
 * Used by node to send capsules form node to application
 * 
 * 
 * 
 */	

namespace San2 { namespace Api {
	

CApplicationConnector::CApplicationConnector(const char *pipeName, unsigned int timCON, unsigned int timRX, unsigned int timTX, unsigned timRPC_RX) :
	San2::Cppl::PipeClient(pipeName, timCON, timRX, timTX),
	m_timRPC_RX(timRPC_RX)
{
	

}


San2::Cppl::ErrorCode CApplicationConnector::receive()
{
	return San2::Cppl::ErrorCode::SUCCESS;
}

San2::Cppl::ErrorCode CApplicationConnector::connect()
{	
	#ifdef LINUX
		San2::Utils::san_ignore_sigpipe();
	#endif
	
	San2::Comm::CpplStreamRW stream(SAN2_CAPPLICATIONCONNECTOR_SINGLE_READ_SIZE, this);
	m_rpcChannel = new San2::Comm::StreamRpcChannel(stream);
	m_rpci = new San2::Rpc::CRpcInvoker(*m_rpcChannel, m_timRPC_RX);

	bool ret = m_rpci->registerFunction([](){return new San2::Interfaces::SendCapsuleFuncOut;});
	if (!ret)
	{
		FILE_LOG(logERROR) << "CApiClient::run(): registrer function FAILED";
		return San2::Cppl::ErrorCode::FAILURE;
	}	
	
	return San2::Cppl::ErrorCode::SUCCESS;
}

bool CApplicationConnector::sendCapsule(San2::Utils::bytes &capsuleData)
{
	San2::Interfaces::SendCapsuleFuncOut func;
	func.setCapsuleToSend(capsuleData);
	return m_rpci->invokeFunction(func);
}	
	
}} //ns
