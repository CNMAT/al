#ifndef __AL_OSCTIME_H__
#define __AL_OSCTIME_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "al_region.h"

typedef struct _al_osctime_ntptime
{
	uint32_t sec;
	uint32_t frac_sec;
} al_osctime_ntptime;
typedef al_osctime_ntptime al_osctime;
#define AL_OSCTIME_SIZEOF_NTP 8
#define AL_OSCTIME_SIZEOF AL_OSCTIME_SIZEOF_NTP
#define AL_OSCTIME_NULL (al_osctime){0, 0}
#define AL_OSCTIME_IMMEDIATE (al_osctime){0, 1}

al_osctime al_osctime_add(al_region region, al_osctime t1, al_osctime t2);
al_osctime al_osctime_subtract(al_region region, al_osctime lhs, al_osctime rhs);
int al_osctime_compare(al_region region, al_osctime t1, al_osctime t2);
double al_osctime_toFloat(al_region region, al_osctime t);
al_osctime al_osctime_fromFloat(al_region region, double f);
char *al_osctime_toISO8601(al_region r, al_osctime t);
size_t _al_osctime_toISO8601(char *buf, size_t n, al_osctime t);
al_osctime al_osctime_fromISO8601(al_region region, char *s);
al_osctime al_osctime_ntoht(al_region region, char *buf);
void al_osctime_htont(al_region region, al_osctime t, char *buf);

#ifdef __cplusplus
}
#endif

#endif
