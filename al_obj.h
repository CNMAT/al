#ifndef __AL_OBJ_H__
#define __AL_OBJ_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "al_region.h"

#define AL_OBJ_TYPE_NULL 0
#define AL_OBJ_TYPE_ATOM 1
#define AL_OBJ_TYPE_LIST 2
#define AL_OBJ_TYPE_ALIST 4

struct _al_obj
{
	void *obj;
	char type;
};

typedef struct _al_obj al_obj;
#define AL_OBJ_NULL ((al_obj){NULL, 0})

al_obj al_obj_copy(al_region r, al_obj b);
al_obj al_obj_isIndexable(al_region r, al_obj b);
al_obj al_obj_nth(al_region r, al_obj b, al_obj n);
al_obj al_obj_rest(al_region r, al_obj b);
al_obj al_obj_length(al_region r, al_obj b);
al_obj al_obj_eql(al_region r, al_obj b1, al_obj b2);
al_obj al_obj_eqv(al_region r, al_obj b1, al_obj b2);
al_obj al_obj_apply(al_region r, al_obj fn, al_obj applicator, al_obj applicand, al_obj context);
al_obj al_obj_eval(al_region r, al_obj o, al_obj context);

//////////////////////////////////////////////////
// C API
//////////////////////////////////////////////////

//al_obj cal_obj_alloc(al_region r, void *obj, char type);
//void *cal_obj_getObj(al_region r, al_obj obj);
//char cal_obj_getType(al_region r, al_obj obj);
#define cal_obj_alloc(_r, _obj, _type) ((al_obj){(_obj), (_type)})
#define cal_obj_getObj(_r, _obj) ((_obj).obj)
#define cal_obj_getType(_r, _obj) ((_obj).type)
char *cal_obj_getTypeName(al_region r, al_obj obj);
int cal_obj_isIndexable(al_region r, al_obj b);
al_obj cal_obj_nth(al_region r, al_obj b, int n);
int cal_obj_length(al_region r, al_obj b);
int cal_obj_eql(al_region r, al_obj b1, al_obj b2);
int cal_obj_eqv(al_region r, al_obj b1, al_obj b2);
int cal_obj_print(al_region r, al_obj o);
int cal_obj_println(al_region r, al_obj o);


#ifdef __cplusplus
}
#endif

#endif
