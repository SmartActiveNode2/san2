

#include "capiclient.hpp"
#include "interfaces/sendcapsulefuncout.hpp"
#include "comm/cpplstreamrw.hpp"
#include "utils/platform/sleep.hpp"
#include "utils/cvector.hpp"
#include "utils/log.h"
#include "utils/platform/sigignore.hpp"
#include "comm/streamrpcchannel.hpp"
#include "rpc/crpcinvoker.hpp"
#include "interfaces/alivefuncout.hpp"

#define CAPI_MAX_SINGLE_READ_SIZE 2048


/*
 * Used by node to send capsules form node to application
 * 
 * 
 * 
 */	

namespace San2 { namespace ClientApi {
	

CApiClient::CApiClient(const char *pipeName, unsigned int timCON, unsigned int timRX, unsigned int timTX, unigned timRPC_RX) :
	San2::Cppl::PipeClient(pipeName, timCON, timRX, timTX),
	m_timRPC_RX(timRPC_RX)
{
	

}


San2::Tcp::TcpErrorCode CTcpInterface::receive()
{
	// intentionally empty as the client only sends
	// we dont call client.start() so this is NEVER called (because we have overridden run())
	// but must be implemented, becouse it is a pure virtual function in CTcpClient
	// (again very confusing, sorry for that)
	return San2::Tcp::TcpErrorCode::SUCCESS;
}

San2::Cppl::ErrorCode CApiClient::receive()
{
	#ifdef LINUX
		San2::Utils::san_ignore_sigpipe();
	#endif
	
	// Now the client=sender stuff
	
	San2::Comm::CpplStreamRW stream(CAPI_MAX_SINGLE_READ_SIZE, this);
	m_rpcChannel = new San2::Comm::StreamRpcChannel(stream);
	m_rpci = new San2::Rpc::CRpcInvoker(*m_rpcChannel, timRPC_RX);
	
	// Another MSVC fix :)
    // error C3480: a lambda capture variable must be from an enclosing function scope
    San2::Network::SanAddress msvc_fix_sanaddr = m_sanaddr;

	bool ret = m_rpci->registerFunction([this, &msvc_fix_sanaddr](){return new SendCapsuleFuncOut;});
	if (!ret)
	{
		FILE_LOG(logERROR) << "CApiClient::run(): registrer function FAILED";
		return San2::Cppl::ErrorCode::FAILURE;
	}	
	
	
	
	return San2::Cppl::ErrorCode::SUCCESS;
}

	
	
}} //ns
