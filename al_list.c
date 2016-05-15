#include <stdarg.h>
#include <stdio.h>
#include "al_list.h"
#include "al_pv.h"
#include "al_atom.h"
#include "al_alist.h"

al_obj al_list_copy(al_region r, al_obj l)
{
	if(cal_obj_getType(r, l) == AL_OBJ_TYPE_LIST){
		al_obj copy = cal_list_alloc(r, 0);
		for(int i = 0; i < cal_list_length(r, l); i++){
			al_list_append_m(r, copy, al_atom_copy(r, cal_list_nth(r, l, i)));
		}
		return copy;
	}else{
		return al_obj_copy(r, l);
	}
}

al_obj al_list_isIndexable(al_region r, al_obj l)
{
	if(cal_obj_getType(r, l) == AL_OBJ_TYPE_LIST){
		return cal_atom_true(r);
	}else{
		return al_obj_isIndexable(r, l);
	}
}

al_obj al_list_nth(al_region r, al_obj l, al_obj n)
{
	if(cal_obj_getType(r, l) == AL_OBJ_TYPE_LIST){
		if(cal_obj_getType(r, n) == AL_OBJ_TYPE_ATOM && cal_atom_isInt(r, n)){
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
	if(cal_obj_getType(r, l) == AL_OBJ_TYPE_LIST){
		// al_pv_popfirst seems broken
		al_obj copy = cal_list_alloc(r, 0);
		for(int i = 1; i < cal_list_length(r, l); i++){
			al_list_append(r, copy, al_atom_copy(r, cal_list_nth(r, l, i)));
		}
		return copy;
	}else{
		return al_obj_rest(r, l);
	}
}

al_obj al_list_length(al_region r, al_obj l)
{
	if(cal_obj_getType(r, l) == AL_OBJ_TYPE_LIST){
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
	if(cal_obj_getType(r, l1) == AL_OBJ_TYPE_LIST){
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
	if(cal_obj_getType(r, l1) == AL_OBJ_TYPE_LIST){
		return cal_list_eqv(r, l1, l2) == 1 ? cal_atom_true(r) : cal_atom_false(r);
	}else{
		return al_obj_eqv(r, l1, l2);
	}
}

al_obj al_list_eval(al_region r, al_obj l, al_obj context)
{
	if(cal_obj_getType(r, l) != AL_OBJ_TYPE_LIST){
		return al_obj_eval(r, l, context);
	}
	al_obj value = cal_atom_symbol(r, "value");
	al_obj ll = cal_list_alloc(r, 0);
	for(int j = 0; j < cal_list_length(r, l); j++){
		al_obj e = al_alist_eval(r, cal_list_nth(r, l, j), context);
		switch(cal_obj_getType(r, e)){
		case AL_OBJ_TYPE_ALIST:
			{
				al_obj v = al_alist_simpleLookup(r, e, value);
				if(cal_obj_getType(r, v) == AL_OBJ_TYPE_LIST){
					for(int k = 1; k < cal_list_length(r, v); k++){
						al_list_append_m(r, ll, cal_list_nth(r, v, k));
					}
				}else{
					// value doesn't exist or was null
					al_list_append_m(r, ll, e);
				}
			}
			break;
		case AL_OBJ_TYPE_LIST:
			for(int k = 1; k < cal_list_length(r, e); k++){
				al_list_append_m(r, ll, cal_list_nth(r, e, k));
			}
			break;
		case AL_OBJ_TYPE_ATOM:
			al_list_append_m(r, ll, e);
			break;
		}
	}
	return ll;
}

/*
static al_obj al_list_builtinEval(al_region r, al_obj l, al_obj context)
{
	//al_obj complete = cal_atom_symbol(r, "complete");
	//al_obj partial = cal_atom_symbol(r, "partial");
	al_obj value = cal_atom_symbol(r, "value");
	al_obj out = cal_list_alloc(r, 0);
	for(int i = 0; i < cal_list_length(r, l); i++){
		al_obj a = cal_list_nth(r, l, i);
		al_obj e = al_obj_eval(r, a, context);
		if(cal_obj_getType(r, e) == AL_OBJ_TYPE_ALIST){
			al_obj statusmsg = cal_alist_simpleLookup(r, e, "eval_status");
			if(cal_obj_getType(r, statusmsg) == AL_OBJ_TYPE_NULL){
				al_list_append_m(r, out, e);
			}else{
				if(cal_obj_getType(r, statusmsg) == AL_OBJ_TYPE_LIST && cal_list_length(r, statusmsg) > 1){
					al_obj status = cal_list_nth(r, statusmsg, 1);
					if(cal_obj_eql(r, status, complete)){
						// full evaluation
						al_obj vm = al_alist_simpleLookup(r, e, value);
						for(int i = 1; i < cal_list_length(r, vm); i++){
							al_obj v = cal_list_nth(r, vm, i);
							al_obj ret = AL_OBJ_NULL;
							if(cal_obj_getType(r, v) == AL_OBJ_TYPE_ALIST){
								ret = al_alist_intersection(r, e, v);
							}else if(cal_obj_getType(r, v) == AL_OBJ_TYPE_LIST){
								ret = al_alist_simpleLookup(r, e, v);
							}
							if(cal_obj_getType(r, ret) != AL_OBJ_TYPE_NULL){
								al_list_append_m(r, out, ret);
							}
						}
					}
				}else{
					al_list_append_m(r, out, e);
				}
			}
		}else{
			al_list_append_m(r, out, e);
		}
	}
	return out;
}

al_obj al_list_eval(al_region r, al_obj l, al_obj context)
{
	if(cal_obj_getType(r, l) == AL_OBJ_TYPE_LIST){
		al_obj entrypointmsg = cal_alist_simpleLookup(r, context, "entrypoint");
		al_obj entrypoint;
		if(cal_obj_getType(r, entrypointmsg) != AL_OBJ_TYPE_NULL){
			entrypoint = cal_list_nth(r, entrypointmsg, 1);
			context = al_alist_union(r, cal_alist_alloc(r, 1, entrypointmsg), context);
		}else{
			entrypointmsg = cal_alist_simpleLookup(r, l, "entrypoint");
			if(cal_obj_getType(r, entrypointmsg) != AL_OBJ_TYPE_NULL){
				entrypoint = cal_list_nth(r, entrypointmsg, 1);
			}else{
				entrypoint = AL_OBJ_NULL;
			}
		}
		if(cal_obj_getType(r, entrypoint) == AL_OBJ_TYPE_NULL){
			return al_list_builtinEval(r, l, context);
		}
		
		al_obj eval_status = cal_alist_getEntryPointVal(r, entrypoint, "eval_status");
		if(cal_obj_getType(r, eval_status) == AL_OBJ_TYPE_NULL){
			eval_status = cal_atom_symbol(r, "eval_status");
		}
		al_obj complete = cal_alist_getEntryPointVal(r, entrypoint, "complete");
		if(cal_obj_getType(r, complete) == AL_OBJ_TYPE_NULL){
			complete = cal_atom_symbol(r, "complete");
		}
		al_obj partial = cal_alist_getEntryPointVal(r, entrypoint, "partial");
		if(cal_obj_getType(r, partial) == AL_OBJ_TYPE_NULL){
			partial = cal_atom_symbol(r, "partial");
		}
		al_obj value = cal_alist_getEntryPointVal(r, entrypoint, "value");
		if(cal_obj_getType(r, value) == AL_OBJ_TYPE_NULL){
			value = cal_atom_symbol(r, "value");
		}
		al_obj out = cal_list_alloc(r, 0);
		for(int i = 0; i < cal_list_length(r, l); i++){
			al_obj a = cal_list_nth(r, l, i);
			al_obj e = al_obj_eval(r, a, context);
			if(cal_obj_getType(r, e) == AL_OBJ_TYPE_ALIST){
				al_obj statusmsg = al_alist_simpleLookup(r, e, eval_status);
				if(cal_obj_getType(r, statusmsg) == AL_OBJ_TYPE_NULL){
					al_list_append_m(r, out, e);
				}else{
					if(cal_obj_getType(r, statusmsg) == AL_OBJ_TYPE_LIST && cal_list_length(r, statusmsg) > 1){
						al_obj status = cal_list_nth(r, statusmsg, 1);
						if(cal_obj_eql(r, status, complete)){
							// full evaluation
							al_obj vm = al_alist_simpleLookup(r, e, value);
							for(int i = 1; i < cal_list_length(r, vm); i++){
								al_obj v = cal_list_nth(r, vm, i);
								al_obj ret = AL_OBJ_NULL;
								if(cal_obj_getType(r, v) == AL_OBJ_TYPE_ALIST){
									ret = al_alist_intersection(r, e, v);
								}else if(cal_obj_getType(r, v) == AL_OBJ_TYPE_LIST){
									ret = al_alist_simpleLookup(r, e, v);
								}
								if(cal_obj_getType(r, ret) != AL_OBJ_TYPE_NULL){
									al_list_append_m(r, out, ret);
								}
							}
						}
					}else{
						al_list_append_m(r, out, e);
					}
				}
			}else{
				continue;
			}
		}
		return out;
	}
	return al_obj_eval(r, l, context);
}
*/

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
		if(cal_obj_getType(r, li) != AL_OBJ_TYPE_ATOM && cal_obj_getType(r, li) != AL_OBJ_TYPE_ALIST){
			printf("%s: only accepts atoms and alists, but got: %s\n", __func__, cal_obj_getTypeName(r, li));
			return AL_OBJ_NULL;
		}
		al_obj *copy = al_region_getBytes(r, sizeof(al_obj));
		*copy = li;
		al_pv2_append_m(r, l, (void *)copy);
	}
	va_end(ap);
	return cal_obj_alloc(r, l, AL_OBJ_TYPE_LIST);
}

int cal_list_isIndexable(al_region r, al_obj l)
{
	if(cal_obj_getType(r, l) == AL_OBJ_TYPE_LIST){
		return 1;
	}else{
		return cal_obj_isIndexable(r, l);
	}
}

al_obj cal_list_nth(al_region r, al_obj list, int i)
{
	if(cal_obj_getType(r, list) == AL_OBJ_TYPE_LIST){
		al_obj *o = (al_obj *)al_pv2_nth((al_pv2 *)(cal_obj_getObj(r, list)), i);
		if(o){
			return *o;
		}
	}
	return cal_obj_alloc(r, NULL, 0);
}

int cal_list_length(al_region r, al_obj list)
{
	if(cal_obj_getType(r, list) == AL_OBJ_TYPE_LIST){
		return al_pv2_length((al_pv2 *)(cal_obj_getObj(r, list)));
	}
	return 0;
}

al_obj al_list_append(al_region r, al_obj list, al_obj val)
{
	if(cal_obj_getType(r, list) != AL_OBJ_TYPE_LIST){
		return list;
	}
	al_obj *copy = al_region_getBytes(r, sizeof(al_obj));
	*copy = val;
	return cal_obj_alloc(r, al_pv2_append(r, (al_pv2 *)(cal_obj_getObj(r, list)), (void *)copy), AL_OBJ_TYPE_LIST);
}

al_obj al_list_append_m(al_region r, al_obj list, al_obj val)
{
	if(cal_obj_getType(r, list) != AL_OBJ_TYPE_LIST){
		return list;
	}
	al_obj *copy = al_region_getBytes(r, sizeof(al_obj));
	*copy = val;
	return cal_obj_alloc(r, al_pv2_append_m(r, (al_pv2 *)(cal_obj_getObj(r, list)), (void *)copy), AL_OBJ_TYPE_LIST);
}

al_obj al_list_prepend(al_region r, al_obj list, al_obj val)
{
	if(cal_obj_getType(r, list) != AL_OBJ_TYPE_LIST){
		return list;
	}
	al_obj *copy = al_region_getBytes(r, sizeof(al_obj));
	*copy = val;
	return cal_obj_alloc(r, al_pv2_prepend(r, (al_pv2 *)(cal_obj_getObj(r, list)), (void *)copy), AL_OBJ_TYPE_LIST);
}

al_obj al_list_prepend_m(al_region r, al_obj list, al_obj val)
{
	if(cal_obj_getType(r, list) != AL_OBJ_TYPE_LIST){
		return list;
	}
	al_obj *copy = al_region_getBytes(r, sizeof(al_obj));
	*copy = val;
	return cal_obj_alloc(r, al_pv2_prepend_m(r, (al_pv2 *)(cal_obj_getObj(r, list)), (void *)copy), AL_OBJ_TYPE_LIST);
}

al_obj al_list_popFirst(al_region r, al_obj list)
{
	if(cal_obj_getType(r, list) != AL_OBJ_TYPE_LIST){
		return list;
	}
	return cal_obj_alloc(r, al_pv2_popFirst(r, (al_pv2 *)(cal_obj_getObj(r, list))), AL_OBJ_TYPE_LIST);
}

al_obj al_list_popLast(al_region r, al_obj list)
{
	if(cal_obj_getType(r, list) != AL_OBJ_TYPE_LIST){
		return list;
	}
	return cal_obj_alloc(r, al_pv2_popLast(r, (al_pv2 *)(cal_obj_getObj(r, list))), AL_OBJ_TYPE_LIST);
}

int cal_list_eql(al_region r, al_obj l1, al_obj l2)
{
	if(cal_obj_getType(r, l1) != cal_obj_getType(r, l2)){
		return 0;
	}
	if(cal_obj_getType(r, l1) == AL_OBJ_TYPE_LIST && cal_obj_getType(r, l2) == AL_OBJ_TYPE_LIST){
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
	if(cal_obj_getType(r, l1) == AL_OBJ_TYPE_LIST && cal_obj_getType(r, l2) == AL_OBJ_TYPE_LIST){
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

int cal_list_print(al_region r, al_obj l)
{
	if(cal_obj_getType(r, l) == AL_OBJ_TYPE_LIST){
		int n = 0;
		n += printf("[");
		for(int i = 0; i < cal_list_length(r, l) - 1; i++){
			n += cal_obj_print(r, cal_list_nth(r, l, i));
			n += printf(" ");
		}
		n += cal_obj_print(r, cal_list_nth(r, l, cal_list_length(r, l) - 1));
		n += printf("]");
		return n;
	}
	return 0;
}

int cal_list_println(al_region r, al_obj l)
{
	return cal_list_print(r, l) + printf("\n");
}
