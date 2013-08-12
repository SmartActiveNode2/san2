
#include "time.hpp"
#include <stdlib.h>
#include <sys/time.h>

namespace San2 { namespace Utils {

#ifdef LINUX

unsigned long getSeconds()
{
    struct timeval t;
    gettimeofday(&t, NULL);
    return (unsigned long long)t.tv_sec;
}

#endif

#ifdef WINDOWS

unsigned long getSeconds()
{
	return 0; // TODO FIX
}

#endif

}} // ns
