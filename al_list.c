#include <stdarg.h>
#include <stdio.h>
#include "al_list.h"
#include "al_pv.h"
#include "al_atom.h"

al_obj al_list_copy(al_region r, al_obj l)
{
	if(cal_obj_getType(r, l) == AL_OBJ_LIST){
		al_obj copy = cal_list_alloc(r, 0);
		for(int i = 0; i < cal_list_length(r, l); i++){
			cal_list_append(r, copy, al_atom_copy(r, cal_list_nth(r, l, i)));
		}
		return copy;
	}else{
		return al_obj_copy(r, l);
	}
}

al_obj al_list_isIndexable(al_region r, al_obj l)
{
	if(cal_obj_getType(r, l) == AL_OBJ_LIST){
		return cal_atom_true(r);
	}else{
		return al_obj_isIndexable(r, l);
	}
}

al_obj al_list_nth(al_region r, al_obj l, al_obj n)
{
	if(cal_obj_getType(r, l) == AL_OBJ_LIST){
		if(cal_atom_isInt(r, n)){
			al_c_int32 i = cal_atom_getInt32(r, n);
			if(!al_c_error(i)){
				return cal_list_nth(r, l, al_c_value(i));
			}else{
				// check for overflow
				return cal_atom_nil(r);
			}
		}else{
			return cal_atom_nil(r);
		}
	}else{
		return al_obj_nth(r, l, n);
	}
}

al_obj al_list_rest(al_region r, al_obj l)
{
	if(cal_obj_getType(r, l) == AL_OBJ_LIST){
		// al_pv_popfirst seems broken
		al_obj copy = cal_list_alloc(r, 0);
		for(int i = 1; i < cal_list_length(r, l); i++){
			cal_list_append(r, copy, al_atom_copy(r, cal_list_nth(r, l, i)));
		}
		return copy;
	}else{
		return al_obj_rest(r, l);
	}
}

al_obj al_list_length(al_region r, al_obj l)
{
	if(cal_obj_getType(r, l) == AL_OBJ_LIST){
		return cal_atom_int32(r, cal_list_length(r, l));
	}else{
		return al_obj_length(r, l);
	}
}

al_obj al_list_eql(al_region r, al_obj l1, al_obj l2)
{
	if(cal_obj_getType(r, l1) != cal_obj_getType(r, l2)){
		return cal_atom_false(r);
	}
	if(cal_obj_getType(r, l1) == AL_OBJ_LIST){
		return cal_list_eql(r, l1, l2) == 1 ? cal_atom_true(r) : cal_atom_false(r);
	}else{
		return al_obj_eql(r, l1, l2);
	}
}

al_obj al_list_eqv(al_region r, al_obj l1, al_obj l2)
{
	if(cal_obj_getType(r, l1) != cal_obj_getType(r, l2)){
		return cal_atom_false(r);
	}
	if(cal_obj_getType(r, l1) == AL_OBJ_LIST){
		return cal_list_eqv(r, l1, l2) == 1 ? cal_atom_true(r) : cal_atom_false(r);
	}else{
		return al_obj_eqv(r, l1, l2);
	}
}


//////////////////////////////////////////////////
// C API
//////////////////////////////////////////////////

al_obj cal_list_alloc(al_region r, int n, ...)
{
	al_pv2 *l = al_pv2_alloc(r, NULL);
	va_list ap;
	va_start(ap, n);
	for(int i = 0; i < n; i++){
		al_obj li = va_arg(ap, al_obj);
		al_obj *copy = al_region_getBytes(r, sizeof(al_obj));
		*copy = li;
		al_pv2_append_m(r, l, (void *)copy);
	}
	va_end(ap);
	return cal_obj_alloc(r, l, AL_OBJ_LIST);
}

int cal_list_isIndexable(al_region r, al_obj l)
{
	if(cal_obj_getType(r, l) == AL_OBJ_LIST){
		return 1;
	}else{
		return cal_obj_isIndexable(r, l);
	}
}

al_obj cal_list_nth(al_region r, al_obj list, int i)
{
	if(cal_obj_getType(r, list) == AL_OBJ_LIST){
		al_obj *o = (al_obj *)al_pv2_nth((al_pv2 *)(cal_obj_getObj(r, list)), i);
		if(o && cal_obj_getType(r, *o) == AL_OBJ_ATOM){
			return *o;
		}
	}
	return cal_obj_alloc(r, NULL, 0);
}

int cal_list_length(al_region r, al_obj list)
{
	if(cal_obj_getType(r, list) == AL_OBJ_LIST){
		return al_pv2_length((al_pv2 *)(cal_obj_getObj(r, list)));
	}
	return 0;
}

al_obj cal_list_append(al_region r, al_obj list, al_obj val)
{
	if(cal_obj_getType(r, list) != AL_OBJ_LIST){
		return list;
	}
	al_obj *copy = al_region_getBytes(r, sizeof(al_obj));
	*copy = val;
	return cal_obj_alloc(r, al_pv2_append(r, (al_pv2 *)(cal_obj_getObj(r, list)), (void *)copy), AL_OBJ_LIST);
}

al_obj cal_list_append_m(al_region r, al_obj list, al_obj val)
{
	if(cal_obj_getType(r, list) != AL_OBJ_LIST){
		return list;
	}
	al_obj *copy = al_region_getBytes(r, sizeof(al_obj));
	*copy = val;
	return cal_obj_alloc(r, al_pv2_append_m(r, (al_pv2 *)(cal_obj_getObj(r, list)), (void *)copy), AL_OBJ_LIST);
}

al_obj cal_list_prepend(al_region r, al_obj list, al_obj val)
{
	if(cal_obj_getType(r, list) != AL_OBJ_LIST){
		return list;
	}
	al_obj *copy = al_region_getBytes(r, sizeof(al_obj));
	*copy = val;
	return cal_obj_alloc(r, al_pv2_prepend(r, (al_pv2 *)(cal_obj_getObj(r, list)), (void *)copy), AL_OBJ_LIST);
}

al_obj cal_list_prepend_m(al_region r, al_obj list, al_obj val)
{
	if(cal_obj_getType(r, list) != AL_OBJ_LIST){
		return list;
	}
	al_obj *copy = al_region_getBytes(r, sizeof(al_obj));
	*copy = val;
	return cal_obj_alloc(r, al_pv2_prepend_m(r, (al_pv2 *)(cal_obj_getObj(r, list)), (void *)copy), AL_OBJ_LIST);
}

al_obj cal_list_popFirst(al_region r, al_obj list)
{
	if(cal_obj_getType(r, list) != AL_OBJ_LIST){
		return list;
	}
	return cal_obj_alloc(r, al_pv2_popFirst(r, (al_pv2 *)(cal_obj_getObj(r, list))), AL_OBJ_LIST);
}

al_obj cal_list_popLast(al_region r, al_obj list)
{
	if(cal_obj_getType(r, list) != AL_OBJ_LIST){
		return list;
	}
	return cal_obj_alloc(r, al_pv2_popLast(r, (al_pv2 *)(cal_obj_getObj(r, list))), AL_OBJ_LIST);
}

int cal_list_eql(al_region r, al_obj l1, al_obj l2)
{
	if(cal_obj_getType(r, l1) != cal_obj_getType(r, l2)){
		return 0;
	}
	if(cal_obj_getType(r, l1) == AL_OBJ_LIST && cal_obj_getType(r, l2) == AL_OBJ_LIST){
		int len1 = cal_obj_length(r, l1);
		if(len1 != cal_obj_length(r, l2)){
			return 0;
		}
		for(int i = 0; i < len1; i++){
			if(!cal_atom_eql(r, cal_list_nth(r, l1, i), cal_list_nth(r, l2, i))){
				return 0;
			}
		}
		return 1;
	}else{
		return cal_obj_eql(r, l1, l2);
	}
}

int cal_list_eqv(al_region r, al_obj l1, al_obj l2)
{
	if(cal_obj_getType(r, l1) != cal_obj_getType(r, l2)){
		return 0;
	}
	if(cal_obj_getType(r, l1) == AL_OBJ_LIST && cal_obj_getType(r, l2) == AL_OBJ_LIST){
		int len1 = cal_obj_length(r, l1);
		if(len1 != cal_obj_length(r, l2)){
			return 0;
		}
		for(int i = 0; i < len1; i++){
			if(!cal_atom_eqv(r, cal_list_nth(r, l1, i), cal_list_nth(r, l2, i))){
				return 0;
			}
		}
		return 1;
	}else{
		return cal_obj_eqv(r, l1, l2);
	}
}
