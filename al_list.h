#ifndef __AL_LIST_H__
#define __AL_LIST_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "al_obj.h"
#include "al_pv.h"
#include "al_region.h"

typedef const al_pv2* al_list;
typedef al_pv2* al_list_m;
typedef struct _al_listitem al_listitem;

al_obj al_list_copy(al_region r, al_obj l);
al_obj al_list_isIndexable(al_region r, al_obj l);
al_obj al_list_nth(al_region r, al_obj l, al_obj n);
al_obj al_list_rest(al_region r, al_obj l);
al_obj al_list_length(al_region r, al_obj l);
al_obj al_list_eql(al_region r, al_obj l1, al_obj l2);
al_obj al_list_eqv(al_region r, al_obj l1, al_obj l2);

//////////////////////////////////////////////////
// C API
//////////////////////////////////////////////////
al_obj cal_list_alloc(al_region r, int n, ...);
int cal_list_isIndexable(al_region r, al_obj l);
al_obj cal_list_nth(al_region r, al_obj list, int i);
al_obj cal_list_append(al_region r, al_obj list, al_obj val);
al_obj cal_list_append_m(al_region r, al_obj list, al_obj val);
al_obj cal_list_prepend(al_region r, al_obj list, al_obj val);
al_obj cal_list_prepend_m(al_region r, al_obj list, al_obj val);
al_obj cal_list_popFirst(al_region r, al_obj list);
al_obj cal_list_popLast(al_region r, al_obj list);
int cal_list_length(al_region r, al_obj list);
int cal_list_eql(al_region r, al_obj l1, al_obj l2);
int cal_list_eqv(al_region r, al_obj l1, al_obj l2);

#ifdef __cplusplus
}
#endif

#endif
