#ifndef __AL_ALIST_H__
#define __AL_ALIST_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "al_obj.h"
typedef al_obj al_alist;

#include "al_env.h"

al_obj al_alist_copy(al_env env, al_obj a);
al_obj al_alist_isIndexable(al_env env, al_obj a);
al_obj al_alist_nth(al_env env, al_obj a, al_obj n);
al_obj al_alist_length(al_env env, al_obj a);
al_obj al_alist_eql(al_env env, al_obj a1, al_obj a2);
al_obj al_alist_eqv(al_env env, al_obj a1, al_obj a2);
al_obj al_alist_append(al_env env, al_obj a, al_obj list);
al_obj al_alist_append_m(al_env env, al_obj a, al_obj list);
al_obj al_alist_simpleLookup(al_env env, al_obj a, al_obj name);
al_obj al_alist_union(al_env env, al_obj a1, al_obj a2);
al_obj al_alist_intersection(al_env env, al_obj a1, al_obj a2);
al_obj al_alist_rcomplement(al_env env, al_obj a1, al_obj a2);
al_obj al_alist_apply(al_env env, al_obj fn, al_obj applicator, al_obj applicand, al_obj context);
al_obj cal_alist_getEntryPointVal(al_env env, al_obj entrypoint, char *key);
al_obj al_alist_eval(al_env env, al_obj al, al_obj context);

//////////////////////////////////////////////////
// C API
//////////////////////////////////////////////////
al_obj cal_alist_alloc(al_env env, int n, ...);
int cal_alist_isIndexable(al_env env, al_obj al);
al_obj cal_alist_nth(al_env env, al_obj al, int n);
int cal_alist_length(al_env env, al_obj al);
int cal_alist_eql(al_env env, al_obj al1, al_obj al2);
int cal_alist_eqv(al_env env, al_obj al1, al_obj al2);
al_obj cal_alist_simpleLookup(al_env env, al_obj al, char *name);

int cal_alist_print(al_env env, al_obj al);
int cal_alist_println(al_env env, al_obj al);

#ifdef __cplusplus
}
#endif

#endif
