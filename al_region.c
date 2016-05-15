#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "al_region.h"
#include "al_alist.h"

#define AL_REGION_PRINTF_WARNING

al_region al_region_allocWithBytes(size_t nbytes, char *bytes)
{
	al_region r = ((al_region)bytes);
	char *p = bytes + sizeof(struct _al_region);
	nbytes -= sizeof(struct _al_region);
	r->stacksize = nbytes;
	r->stack = p;
	r->stackptr = r->stack;
	return r;
}

#ifdef AL_REGION_LOG
void *_al_region_getBytes(al_region r, size_t nbytes, const char *func)
{
	printf("%s: %ld\n", func, nbytes);
#else
void *_al_region_getBytes(al_region r, size_t nbytes)
{
#endif
	if((((r->stackptr + nbytes) - r->stack) < r->stacksize)){
		char *sp = r->stackptr;
		r->stackptr += nbytes;
		return (void *)sp;
	}else{
#ifdef AL_REGION_PRINTF_WARNING
		printf("%s: out of memory!\n", __func__);
#endif
		return NULL;
	}
}

void *al_region_move(al_region r, char *to, char *from, size_t nbytes)
{
	if(to == from){
		return to;
	}else{
		return memmove(to, from, nbytes);
	}
}

char *al_region_getPtr(al_region r)
{
	return r->stackptr;
}

void al_region_setPtr(al_region r, char *ptr)
{
	if(ptr){
		r->stackptr = ptr;
	}
}

size_t al_region_bytesUsed(al_region r)
{
	return r->stackptr - r->stack;
}

size_t al_region_bytesFree(al_region r)
{
	return r->stacksize - (r->stackptr - r->stack);
}

