#if defined(__linux__)
#include <asm/fcntl.h>
#else
#include <fcntl.h>
#endif

unsigned int
kernelolargefile(void)
{
#ifdef O_LARGEFILE
	return O_LARGEFILE;
#else
	return 0;
#endif
}
