#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#ifdef LINUX
	#include <sys/types.h>
	#include <sys/un.h>
	#include <signal.h>
#endif

#include "cppl/pipeclient.hpp"
#include "cppl/pipeserver.hpp"
#include "cppl/pipechannel.hpp"
#include "serverreceiver.hpp"

#define TIMEOUT_CON 5000
#define TIMEOUT_RX  5000
#define TIMEOUT_TX  5000

#ifdef LINUX
	#define SRV_PIPENAME "/tmp/asd5345ds1asd315aa"
    #define SRV_TERMINATE_SEC 30
#endif
#ifdef WINDOWS
	#define SRV_PIPENAME "\\\\.\\pipe\\mynamedpipe"
#endif

San2::Cppl::PipeChannel* createPipeChannel(CPPL_PIPETYPE handle, unsigned int timRX, unsigned int timTX)
{
	return new ServerReceiver(handle, timRX, timTX);
}

San2::Cppl::PipeServer ps(SRV_PIPENAME, createPipeChannel, TIMEOUT_CON, TIMEOUT_RX, TIMEOUT_TX);

/*
#ifdef LINUX
  struct sigaction act;
  void sighandler(int signum, siginfo_t *info, void *ptr)
  {
     printf("Received signal %d\n from process %lu\n", signum, (unsigned long)info->si_pid);
     
     if (signum == SIGALRM && (unsigned long)info->si_pid == 0)
     {
  	   printf("alarm ps teriminaton\n");
  	   ps.terminate();
     }
     
  }
#endif
*/

int main(int argc, char *argv[])
{
	printf("RpcServer\n");
	ps.start();
/*
  #ifdef LINUX
    // Testing server termination
	  memset(&act, 0, sizeof(act));
    act.sa_sigaction = sighandler;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGALRM, &act, NULL);
	  alarm(SRV_TERMINATE_SEC);
  #endif
*/
	ps.join();
	printf("returned: %d\n", errorCodeToInt(ps.getErrorCode()));	
	return 0;
}
