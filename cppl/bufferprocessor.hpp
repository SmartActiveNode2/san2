
#pragma once

#include "cpthread.hpp"
#include "helper.hpp"

namespace CPPL
{
    class BufferProcessor : public CpThread
    {
    public:
        BufferProcessor();
		virtual ~BufferProcessor();

        ErrorCode readSome(char *buffer, unsigned int bufferSize, unsigned int *bytesRead);
        ErrorCode readDelimeter(char *output, unsigned int outputSize, unsigned int *outputLen, char delimeter);
        ErrorCode readLine(char *line, unsigned int lineSize);
        virtual ErrorCode send(char *data, int len)=0;
        ErrorCode send(const char *nullTerminatedString);
        ErrorCode send(const std::string &data);
        ErrorCode sendLine(const char *nullTerminatedString);
        ErrorCode sendLine(void);

    protected:
        virtual ErrorCode read(char *data, unsigned int dataSize, unsigned int *bytesRead)=0;    

    private:
        bool bufferFull();
        ErrorCode fillBuffer();
        ErrorCode fillBuffer(char *readBuffer, unsigned int readBufferSize, unsigned int *readBufferLen);
        ErrorCode extractBuffer(char *outputBuffer, unsigned int outputBufferSize, unsigned int extractCount);

        static const unsigned int mReadBufferSize = 1024;
		char mReadBuffer[mReadBufferSize];
		unsigned int mReadBufferLen;


        // another msvc fix
		#ifdef LINUX
			BufferProcessor(const BufferProcessor& copyFromMe)=delete;
			BufferProcessor& operator=(const BufferProcessor& copyFromMe)=delete;
		#endif

    };

}