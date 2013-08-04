
#include <utility>
#include "crpcexecutor.hpp"
#include "utils/log.h"

namespace San2
{
	namespace Rpc
	{
		CRpcExecutor::CRpcExecutor(CIRpcChannel& channel, unsigned int timRX) :
			m_channel(channel),
			m_timRX(timRX)
		{
		
		}
		
		void CRpcExecutor::run()
		{
			San2::Utils::bytes b;
			unsigned int functionId;
			
			// needs some kind of return value
			while(!isTerminated())
			{
				if (!(m_channel.recvData(functionId, b, m_timRX)))
				{
					 //printf("FAIL: CRpcExecutor::run()::recvData()\n");
					 break;
				}
				
				if (executeFunction(functionId, b) != RpcError::SUCCESS)
				{
					FILE_LOG(logDEBUG3) << "FAIL:CRpcExecutor::run()::executeFunction()";	
				}
			}
		}
		
		bool CRpcExecutor::registerFunction(std::function<CIRpcFunctionIn* (void)> createFunction)
		{
			std::shared_ptr<CIRpcFunctionIn> sharedFuncPtr(createFunction()); // must be on separate line!!! otherwise mem leak
			std::pair<std::map<unsigned int, std::shared_ptr<CIRpcFunctionIn> >::iterator, bool> ret;
			ret = m_functions.insert(std::pair<unsigned int, std::shared_ptr<CIRpcFunctionIn> > (sharedFuncPtr->getUniqueId(), sharedFuncPtr));
			return ret.second;	
		}

        bool CRpcExecutor::registerSyncFunction(std::function<CIRpcSyncFunctionIn* (void)> createSyncFunction)
		{
			std::shared_ptr<CIRpcSyncFunctionIn> sharedSyncFuncPtr(createSyncFunction()); // must be on separate line!!! otherwise mem leak
			std::pair<std::map<unsigned int, std::shared_ptr<CIRpcSyncFunctionIn> >::iterator, bool> ret;
			ret = m_syncFunctions.insert(std::pair<unsigned int, std::shared_ptr<CIRpcSyncFunctionIn> > (sharedSyncFuncPtr->getUniqueId(), sharedSyncFuncPtr));
			return ret.second;	
		}
		
		RpcError CRpcExecutor::executeFunction(unsigned int uniqueId, const San2::Utils::bytes &in)
		{
			std::shared_ptr<CIRpcFunctionIn> func;

			if (findFunction(uniqueId, func))
            {
                if (!func->unpack(in))
			    {
				     FILE_LOG(logDEBUG4) << "FAIL: CRpcExecutor::executeFunction(): unpack() error";		
				     return RpcError::UNPACK_ERROR;
			    }

                (*func)(); // execute
                return RpcError::SUCCESS;
            }
			
            std::shared_ptr<CIRpcSyncFunctionIn> syncFunc;

            if (findSyncFunction(uniqueId, syncFunc))
            {
                if (!syncFunc->unpack(in))
			    {
				     FILE_LOG(logDEBUG4) << "FAIL: CRpcExecutor::executeFunction(): unpack() error";		
				     return RpcError::UNPACK_ERROR;
			    }

                (*syncFunc)(); // execute
                if (m_channel.sendData(uniqueId, syncFunc->getResponse())) return RpcError::SUCCESS;

                FILE_LOG(logDEBUG4) << "FAIL: CRpcExecutor::executeFunction(): send data; funcId:" << uniqueId;		
                return RpcError::FAILURE;
            }

			
			FILE_LOG(logDEBUG4) << "FAIL: CRpcExecutor::executeFunction(): function not found; funcId:" << uniqueId;		
			return RpcError::FUNCTION_NOT_FOUND;
		}
		
		bool CRpcExecutor::findFunction(unsigned int uniqueId, std::shared_ptr<CIRpcFunctionIn> &function)
		{
			std::map<unsigned int, std::shared_ptr<CIRpcFunctionIn> >::iterator it;
			it = m_functions.find(uniqueId);
			if (it == m_functions.end()) return false; // function not found
			function = it->second;
			return true; // ok, found
		}

        bool CRpcExecutor::findSyncFunction(unsigned int uniqueId, std::shared_ptr<CIRpcSyncFunctionIn> &syncFunction)
		{
			std::map<unsigned int, std::shared_ptr<CIRpcSyncFunctionIn> >::iterator it;
			it = m_syncFunctions.find(uniqueId);
			if (it == m_syncFunctions.end()) return false; // function not found
			syncFunction = it->second;
			return true; // ok, found
		}
	}
}
