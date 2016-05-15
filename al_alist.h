#ifndef __AL_ALIST_H__
#define __AL_ALIST_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "al_obj.h"
#include "al_region.h"

typedef al_obj al_alist;

al_obj al_alist_copy(al_region r, al_obj a);
al_obj al_alist_isIndexable(al_region r, al_obj a);
al_obj al_alist_nth(al_region r, al_obj a, al_obj n);
al_obj al_alist_length(al_region r, al_obj a);
al_obj al_alist_eql(al_region r, al_obj a1, al_obj a2);
al_obj al_alist_eqv(al_region r, al_obj a1, al_obj a2);
al_obj al_alist_append(al_region r, al_obj a, al_obj list);
al_obj al_alist_append_m(al_region r, al_obj a, al_obj list);
al_obj al_alist_simpleLookup(al_region r, al_obj a, al_obj name);
al_obj al_alist_union(al_region r, al_obj a1, al_obj a2);
al_obj al_alist_intersection(al_region r, al_obj a1, al_obj a2);
al_obj al_alist_rcomplement(al_region r, al_obj a1, al_obj a2);
al_obj al_alist_apply(al_region r, al_obj fn, al_obj applicator, al_obj applicand, al_obj context);
al_obj cal_alist_getEntryPointVal(al_region r, al_obj entrypoint, char *key);
al_obj al_alist_eval(al_region r, al_obj al, al_obj context);

//////////////////////////////////////////////////
// C API
//////////////////////////////////////////////////
al_obj cal_alist_alloc(al_region r, int n, ...);
int cal_alist_isIndexable(al_region r, al_obj al);
al_obj cal_alist_nth(al_region r, al_obj al, int n);
int cal_alist_length(al_region r, al_obj al);
int cal_alist_eql(al_region r, al_obj al1, al_obj al2);
int cal_alist_eqv(al_region r, al_obj al1, al_obj al2);
al_obj cal_alist_simpleLookup(al_region r, al_obj al, char *name);

int cal_alist_print(al_region r, al_obj al);
int cal_alist_println(al_region r, al_obj al);

#ifdef __cplusplus
}
#endif

#endif
