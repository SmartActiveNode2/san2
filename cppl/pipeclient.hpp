#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <functional>

#ifdef LINUX
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <sys/un.h>
	#include <pthread.h>
#endif

#include <string>

#include "bufferprocessor.hpp"
#include "utils/cthread.hpp"
#include "helper.hpp"
#include "abstractreceiver.hpp"

namespace San2 
{
	namespace Cppl
	{
		class AbstractReceiver;
		
		class PipeClient : public BufferProcessor
		{
			public:
				PipeClient(const char *pipeName, std::function<AbstractReceiver* (void)> createAbstractReceiverProc, unsigned int timCON, unsigned int timRX, unsigned int timTX);
				~PipeClient();
				ErrorCode open();
				ErrorCode send(char *data, int len);
				ErrorCode getErrorCode();
				
			protected:
				void run();
				ErrorCode runProc();
				
			private:
				// Yes, read() MUST be private, only BufferProcessor class can call it!!!!
				ErrorCode read(char *data, unsigned int dataSize, unsigned int *bytesRead);

				const char *pipename;
				AbstractReceiver *absReceiver;

			#ifdef LINUX
				int sock;
				struct sockaddr_un remote;
			#endif

			#ifdef WINDOWS
				HANDLE hPipe; 
			#endif
			
			unsigned int mTimCON, mTimRX, mTimTX;
			ErrorCode errcode;

			// another msvc fix
			#ifdef LINUX
				PipeClient(const PipeClient& copyFromMe)=delete;
				PipeClient& operator=(const PipeClient& copyFromMe)=delete;
			#endif
		};
	}
}
