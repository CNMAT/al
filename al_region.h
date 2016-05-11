#ifndef __AL_REGION_H__
#define __AL_REGION_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

#define AL_REGION_DEFAULT_STACK_SIZE_BYTES 1000000
#define AL_REGION_DEFAULT_TMP_STACK_SIZE_BYTES 256000

struct _al_region
{
	char *stackptr;
	char *stack;
	size_t stacksize;
};
typedef struct _al_region* al_region;

al_region al_region_allocWithBytes(size_t nbytes, char *bytes);
//#define AL_REGION_LOG 1
#ifdef AL_REGION_LOG
void *_al_region_getBytes(al_region r, size_t nbytes, const char *func);
#define al_region_getBytes(r, nbytes) _al_region_getBytes(r, nbytes, __func__);
#else
void *_al_region_getBytes(al_region r, size_t nbytes);
#define al_region_getBytes(r, nbytes) _al_region_getBytes(r, nbytes)
#endif
void *al_region_move(al_region r, char *to, char *from, size_t nbytes);
char *al_region_getPtr(al_region r);
void al_region_setPtr(al_region r, char *ptr);
size_t al_region_bytesUsed(al_region r);
size_t al_region_bytesFree(al_region r);

#ifdef __cplusplus
}
#endif

#endif
