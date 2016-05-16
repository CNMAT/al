#ifndef __AL_REGION_H__
#define __AL_REGION_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <inttypes.h>

#define AL_REGION_DEFAULT_STACK_SIZE_BYTES 1000000

#pragma pack(push)
#pragma pack(4)
struct _al_region
{
	int32_t size;
	char *ptr;
};
#pragma pack(pop)
typedef struct _al_region* al_region;

al_region al_region_allocWithBytes(int32_t nbytes, char *bytes);
//#define AL_REGION_LOG 1
#ifdef AL_REGION_LOG
void *_al_region_getBytes(al_region r, int32_t nbytes, const char *func);
#define al_region_getBytes(r, nbytes) _al_region_getBytes(r, nbytes, __func__);
#else
void *_al_region_getBytes(al_region r, int32_t nbytes);
#define al_region_getBytes(r, nbytes) _al_region_getBytes(r, nbytes)
#endif
char *al_region_getPtr(al_region r);
void al_region_setPtr(al_region r, char *ptr);
int32_t al_region_bytesUsed(al_region r);
int32_t al_region_bytesFree(al_region r);

#ifdef __cplusplus
}
#endif

#endif
