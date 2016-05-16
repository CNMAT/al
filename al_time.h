#ifndef __AL_TIME_H__
#define __AL_TIME_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "al_obj.h"
#include "al_env.h"
#include "al_osctime.h"

typedef al_osctime al_time;

al_time al_time_add(al_env env, al_time t1, al_time t2);
al_time al_time_subtract(al_env env, al_time lhs, al_time rhs);
al_obj al_time_compare(al_env env, al_time t1, al_time t2);
al_obj al_time_toFloat(al_env env, al_time t);
al_time al_time_fromFloat(al_env env, al_obj f);
al_obj al_time_toString(al_env env, al_time t);
al_time al_time_fromString(al_env env, al_obj s);

#ifdef __cplusplus
}
#endif

#endif
