
#pragma once

#include <functional>

#ifdef WINDOWS
	#include <windows.h> 
#endif

#include "bufferprocessor.hpp"
#include "helper.hpp"
#include "abstractreceiver.hpp"

namespace San2
{	
	namespace Cppl
	{
		class AbstractReceiver;
		class PipeServer; // forward declaration

		class PipeChannel : public BufferProcessor
		{
			public:
				~PipeChannel(); // should be private

				#ifdef LINUX
					PipeChannel(int sockt, std::function<AbstractReceiver* (void)> createAbstractReceiverProc, unsigned int timRX, unsigned int timTX);
				#endif

				#ifdef WINDOWS
					PipeChannel(HANDLE handlePipe, std::function<AbstractReceiver* (void)> createAbstractReceiverProc, unsigned int timRX, unsigned int timTX);
				#endif

					ErrorCode send(char *data, int len);
					ErrorCode getErrorCode();
			
			protected:		

			private:
				
				// Yes, read() MUST be private, only BufferProcessor class can call it!!!!
				ErrorCode read(char *data, unsigned int dataSize, unsigned int *bytesRead);
				void run();

				#ifdef LINUX
					int sock;
				#endif
				
				#ifdef WINDOWS
					HANDLE hPipe;
				#endif
					
					AbstractReceiver* absReceiver;
					unsigned int mTimRX, mTimTX;
				
				ErrorCode errcode;
				
				// another msvc fix
				#ifdef LINUX
					PipeChannel(const PipeChannel& copyFromMe)=delete;
					PipeChannel& operator=(const PipeChannel& copyFromMe)=delete;
				#endif
		};
	}
}
