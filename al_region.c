#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "al_region.h"
#include "al_alist.h"
#include "al_endian.h"

#define AL_REGION_PRINTF_WARNING

al_region al_region_allocWithBytes(int32_t nbytes, char *bytes)
{
	al_region r = ((al_region)bytes);
	char *p = bytes + sizeof(struct _al_region);
	nbytes -= sizeof(struct _al_region);
	r->size = hton32(nbytes);
	r->ptr = p;
	return r;
}

#ifdef AL_REGION_LOG
void *_al_region_getBytes(al_region r, int32_t nbytes, const char *func)
{
	printf("%s: %ld\n", func, nbytes);
#else
void *_al_region_getBytes(al_region r, int32_t nbytes)
{
#endif
	//if((((r->stackptr + nbytes) - r->stack) < r->stacksize)){
	if(((r->ptr + nbytes) - (char *)&(r->size)) < ntoh32(r->size)){
		char *sp = r->ptr;
		r->ptr += nbytes;
		return (void *)sp;
	}else{
#ifdef AL_REGION_PRINTF_WARNING
		printf("%s: out of memory!\n", __func__);
#endif
		return NULL;
	}
}

char *al_region_getPtr(al_region r)
{
	return r->ptr;
}

void al_region_setPtr(al_region r, char *ptr)
{
	if(ptr){
		r->ptr = ptr;
	}
}

int32_t al_region_bytesUsed(al_region r)
{
	return r->ptr - (char *)&(r->size);
}

int32_t al_region_bytesFree(al_region r)
{
	return ntoh32(r->size) - (r->ptr - (char *)&(r->size));
}

