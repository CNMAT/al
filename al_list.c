#include <stdarg.h>
#include <stdio.h>
#include "al_list.h"
#include "al_pv.h"
#include "al_atom.h"
#include "al_alist.h"

al_obj al_list_copy(al_env env, al_obj l)
{
	if(cal_obj_getType(env, l) == AL_OBJ_TYPE_LIST){
		al_obj copy = cal_list_alloc(env, 0);
		for(int i = 0; i < cal_list_length(env, l); i++){
			al_list_append_m(env, copy, al_atom_copy(env, cal_list_nth(env, l, i)));
		}
		return copy;
	}else{
		return al_obj_copy(env, l);
	}
}

al_obj al_list_isIndexable(al_env env, al_obj l)
{
	if(cal_obj_getType(env, l) == AL_OBJ_TYPE_LIST){
		return cal_atom_true(env);
	}else{
		return al_obj_isIndexable(env, l);
	}
}

al_obj al_list_nth(al_env env, al_obj l, al_obj n)
{
	if(cal_obj_getType(env, l) == AL_OBJ_TYPE_LIST){
		if(cal_obj_getType(env, n) == AL_OBJ_TYPE_ATOM && cal_atom_isInt(env, n)){
			al_c_int32 i = cal_atom_getInt32(env, n);
			if(!al_c_error(i)){
				return cal_list_nth(env, l, al_c_value(i));
			}else{
				// check for overflow
				return cal_atom_nil(env);
			}
		}else{
			return cal_atom_nil(env);
		}
	}else{
		return al_obj_nth(env, l, n);
	}
}

al_obj al_list_rest(al_env env, al_obj l)
{
	if(cal_obj_getType(env, l) == AL_OBJ_TYPE_LIST){
		// al_pv_popfirst seems broken
		al_obj copy = cal_list_alloc(env, 0);
		for(int i = 1; i < cal_list_length(env, l); i++){
			al_list_append(env, copy, al_atom_copy(env, cal_list_nth(env, l, i)));
		}
		return copy;
	}else{
		return al_obj_rest(env, l);
	}
}

al_obj al_list_length(al_env env, al_obj l)
{
	if(cal_obj_getType(env, l) == AL_OBJ_TYPE_LIST){
		return cal_atom_int32(env, cal_list_length(env, l));
	}else{
		return al_obj_length(env, l);
	}
}

al_obj al_list_eql(al_env env, al_obj l1, al_obj l2)
{
	if(cal_obj_getType(env, l1) != cal_obj_getType(env, l2)){
		return cal_atom_false(env);
	}
	if(cal_obj_getType(env, l1) == AL_OBJ_TYPE_LIST){
		return cal_list_eql(env, l1, l2) == 1 ? cal_atom_true(env) : cal_atom_false(env);
	}else{
		return al_obj_eql(env, l1, l2);
	}
}

al_obj al_list_eqv(al_env env, al_obj l1, al_obj l2)
{
	if(cal_obj_getType(env, l1) != cal_obj_getType(env, l2)){
		return cal_atom_false(env);
	}
	if(cal_obj_getType(env, l1) == AL_OBJ_TYPE_LIST){
		return cal_list_eqv(env, l1, l2) == 1 ? cal_atom_true(env) : cal_atom_false(env);
	}else{
		return al_obj_eqv(env, l1, l2);
	}
}

al_obj al_list_eval(al_env env, al_obj l, al_obj context)
{
	if(cal_obj_getType(env, l) != AL_OBJ_TYPE_LIST){
		return al_obj_eval(env, l, context);
	}
	al_obj value = cal_atom_symbol(env, "value");
	al_obj ll = cal_list_alloc(env, 0);
	for(int j = 0; j < cal_list_length(env, l); j++){
		al_obj e = al_obj_eval(env, cal_list_nth(env, l, j), context);
		switch(cal_obj_getType(env, e)){
		case AL_OBJ_TYPE_ALIST:
			{
				al_obj v = al_alist_simpleLookup(env, e, value);
				if(cal_obj_getType(env, v) == AL_OBJ_TYPE_LIST){
					for(int k = 1; k < cal_list_length(env, v); k++){
						al_list_append_m(env, ll, cal_list_nth(env, v, k));
					}
				}else{
					// value doesn't exist or was null
					al_list_append_m(env, ll, e);
				}
			}
			break;
		case AL_OBJ_TYPE_LIST:
			for(int k = 1; k < cal_list_length(env, e); k++){
				al_list_append_m(env, ll, cal_list_nth(env, e, k));
			}
			break;
		case AL_OBJ_TYPE_ATOM:
			al_list_append_m(env, ll, e);
			break;
		case AL_OBJ_TYPE_NULL:
			;
		}
	}
	return ll;
}

/*
static al_obj al_list_builtinEval(al_env env, al_obj l, al_obj context)
{
	//al_obj complete = cal_atom_symbol(env, "complete");
	//al_obj partial = cal_atom_symbol(env, "partial");
	al_obj value = cal_atom_symbol(env, "value");
	al_obj out = cal_list_alloc(env, 0);
	for(int i = 0; i < cal_list_length(env, l); i++){
		al_obj a = cal_list_nth(env, l, i);
		al_obj e = al_obj_eval(env, a, context);
		if(cal_obj_getType(env, e) == AL_OBJ_TYPE_ALIST){
			al_obj statusmsg = cal_alist_simpleLookup(env, e, "eval_status");
			if(cal_obj_getType(env, statusmsg) == AL_OBJ_TYPE_NULL){
				al_list_append_m(env, out, e);
			}else{
				if(cal_obj_getType(env, statusmsg) == AL_OBJ_TYPE_LIST && cal_list_length(env, statusmsg) > 1){
					al_obj status = cal_list_nth(env, statusmsg, 1);
					if(cal_obj_eql(env, status, complete)){
						// full evaluation
						al_obj vm = al_alist_simpleLookup(env, e, value);
						for(int i = 1; i < cal_list_length(env, vm); i++){
							al_obj v = cal_list_nth(env, vm, i);
							al_obj ret = AL_OBJ_NULL;
							if(cal_obj_getType(env, v) == AL_OBJ_TYPE_ALIST){
								ret = al_alist_intersection(env, e, v);
							}else if(cal_obj_getType(env, v) == AL_OBJ_TYPE_LIST){
								ret = al_alist_simpleLookup(env, e, v);
							}
							if(cal_obj_getType(env, ret) != AL_OBJ_TYPE_NULL){
								al_list_append_m(env, out, ret);
							}
						}
					}
				}else{
					al_list_append_m(env, out, e);
				}
			}
		}else{
			al_list_append_m(env, out, e);
		}
	}
	return out;
}

al_obj al_list_eval(al_env env, al_obj l, al_obj context)
{
	if(cal_obj_getType(env, l) == AL_OBJ_TYPE_LIST){
		al_obj entrypointmsg = cal_alist_simpleLookup(env, context, "entrypoint");
		al_obj entrypoint;
		if(cal_obj_getType(env, entrypointmsg) != AL_OBJ_TYPE_NULL){
			entrypoint = cal_list_nth(env, entrypointmsg, 1);
			context = al_alist_union(env, cal_alist_alloc(env, 1, entrypointmsg), context);
		}else{
			entrypointmsg = cal_alist_simpleLookup(env, l, "entrypoint");
			if(cal_obj_getType(env, entrypointmsg) != AL_OBJ_TYPE_NULL){
				entrypoint = cal_list_nth(env, entrypointmsg, 1);
			}else{
				entrypoint = AL_OBJ_NULL;
			}
		}
		if(cal_obj_getType(env, entrypoint) == AL_OBJ_TYPE_NULL){
			return al_list_builtinEval(env, l, context);
		}
		
		al_obj eval_status = cal_alist_getEntryPointVal(env, entrypoint, "eval_status");
		if(cal_obj_getType(env, eval_status) == AL_OBJ_TYPE_NULL){
			eval_status = cal_atom_symbol(env, "eval_status");
		}
		al_obj complete = cal_alist_getEntryPointVal(env, entrypoint, "complete");
		if(cal_obj_getType(env, complete) == AL_OBJ_TYPE_NULL){
			complete = cal_atom_symbol(env, "complete");
		}
		al_obj partial = cal_alist_getEntryPointVal(env, entrypoint, "partial");
		if(cal_obj_getType(env, partial) == AL_OBJ_TYPE_NULL){
			partial = cal_atom_symbol(env, "partial");
		}
		al_obj value = cal_alist_getEntryPointVal(env, entrypoint, "value");
		if(cal_obj_getType(env, value) == AL_OBJ_TYPE_NULL){
			value = cal_atom_symbol(env, "value");
		}
		al_obj out = cal_list_alloc(env, 0);
		for(int i = 0; i < cal_list_length(env, l); i++){
			al_obj a = cal_list_nth(env, l, i);
			al_obj e = al_obj_eval(env, a, context);
			if(cal_obj_getType(env, e) == AL_OBJ_TYPE_ALIST){
				al_obj statusmsg = al_alist_simpleLookup(env, e, eval_status);
				if(cal_obj_getType(env, statusmsg) == AL_OBJ_TYPE_NULL){
					al_list_append_m(env, out, e);
				}else{
					if(cal_obj_getType(env, statusmsg) == AL_OBJ_TYPE_LIST && cal_list_length(env, statusmsg) > 1){
						al_obj status = cal_list_nth(env, statusmsg, 1);
						if(cal_obj_eql(env, status, complete)){
							// full evaluation
							al_obj vm = al_alist_simpleLookup(env, e, value);
							for(int i = 1; i < cal_list_length(env, vm); i++){
								al_obj v = cal_list_nth(env, vm, i);
								al_obj ret = AL_OBJ_NULL;
								if(cal_obj_getType(env, v) == AL_OBJ_TYPE_ALIST){
									ret = al_alist_intersection(env, e, v);
								}else if(cal_obj_getType(env, v) == AL_OBJ_TYPE_LIST){
									ret = al_alist_simpleLookup(env, e, v);
								}
								if(cal_obj_getType(env, ret) != AL_OBJ_TYPE_NULL){
									al_list_append_m(env, out, ret);
								}
							}
						}
					}else{
						al_list_append_m(env, out, e);
					}
				}
			}else{
				continue;
			}
		}
		return out;
	}
	return al_obj_eval(env, l, context);
}
*/

//////////////////////////////////////////////////
// C API
//////////////////////////////////////////////////

al_obj cal_list_alloc(al_env env, int n, ...)
{
	al_pv2 *l = al_pv2_alloc(al_env_getRegion(env), NULL);
	va_list ap;
	va_start(ap, n);
	for(int i = 0; i < n; i++){
		al_obj li = va_arg(ap, al_obj);
		if(cal_obj_getType(env, li) != AL_OBJ_TYPE_ATOM && cal_obj_getType(env, li) != AL_OBJ_TYPE_ALIST){
			printf("%s: only accepts atoms and alists, but got: %s\n", __func__, cal_obj_getTypeName(env, li));
			return AL_OBJ_NULL;
		}
		al_obj *copy = (al_obj *)al_env_getBytes(env, sizeof(al_obj));
		*copy = li;
		al_pv2_append_m(al_env_getRegion(env), l, (void *)copy);
	}
	va_end(ap);
	return cal_obj_alloc(env, l, AL_OBJ_TYPE_LIST);
}

int cal_list_isIndexable(al_env env, al_obj l)
{
	if(cal_obj_getType(env, l) == AL_OBJ_TYPE_LIST){
		return 1;
	}else{
		return cal_obj_isIndexable(env, l);
	}
}

al_obj cal_list_nth(al_env env, al_obj list, int i)
{
	if(cal_obj_getType(env, list) == AL_OBJ_TYPE_LIST){
		al_obj *o = (al_obj *)al_pv2_nth((al_pv2 *)(cal_obj_getObj(env, list)), i);
		if(o){
			return *o;
		}
	}
	return cal_obj_alloc(env, NULL, 0);
}

int cal_list_length(al_env env, al_obj list)
{
	if(cal_obj_getType(env, list) == AL_OBJ_TYPE_LIST){
		return al_pv2_length((al_pv2 *)(cal_obj_getObj(env, list)));
	}
	return 0;
}

al_obj al_list_append(al_env env, al_obj list, al_obj val)
{
	if(cal_obj_getType(env, list) != AL_OBJ_TYPE_LIST){
		return list;
	}
	al_obj *copy = (al_obj *)al_env_getBytes(env, sizeof(al_obj));
	*copy = val;
	return cal_obj_alloc(env, al_pv2_append(al_env_getRegion(env), (al_pv2 *)(cal_obj_getObj(env, list)), (void *)copy), AL_OBJ_TYPE_LIST);
}

al_obj al_list_append_m(al_env env, al_obj list, al_obj val)
{
	if(cal_obj_getType(env, list) != AL_OBJ_TYPE_LIST){
		return list;
	}
	al_obj *copy = (al_obj *)al_env_getBytes(env, sizeof(al_obj));
	*copy = val;
	return cal_obj_alloc(env, al_pv2_append_m(al_env_getRegion(env), (al_pv2 *)(cal_obj_getObj(env, list)), (void *)copy), AL_OBJ_TYPE_LIST);
}

al_obj al_list_prepend(al_env env, al_obj list, al_obj val)
{
	if(cal_obj_getType(env, list) != AL_OBJ_TYPE_LIST){
		return list;
	}
	al_obj *copy = (al_obj *)al_env_getBytes(env, sizeof(al_obj));
	*copy = val;
	return cal_obj_alloc(env, al_pv2_prepend(al_env_getRegion(env), (al_pv2 *)(cal_obj_getObj(env, list)), (void *)copy), AL_OBJ_TYPE_LIST);
}

al_obj al_list_prepend_m(al_env env, al_obj list, al_obj val)
{
	if(cal_obj_getType(env, list) != AL_OBJ_TYPE_LIST){
		return list;
	}
	al_obj *copy = (al_obj *)al_env_getBytes(env, sizeof(al_obj));
	*copy = val;
	return cal_obj_alloc(env, al_pv2_prepend_m(al_env_getRegion(env), (al_pv2 *)(cal_obj_getObj(env, list)), (void *)copy), AL_OBJ_TYPE_LIST);
}

al_obj al_list_popFirst(al_env env, al_obj list)
{
	if(cal_obj_getType(env, list) != AL_OBJ_TYPE_LIST){
		return list;
	}
	return cal_obj_alloc(env, al_pv2_popFirst(al_env_getRegion(env), (al_pv2 *)(cal_obj_getObj(env, list))), AL_OBJ_TYPE_LIST);
}

al_obj al_list_popLast(al_env env, al_obj list)
{
	if(cal_obj_getType(env, list) != AL_OBJ_TYPE_LIST){
		return list;
	}
	return cal_obj_alloc(env, al_pv2_popLast(al_env_getRegion(env), (al_pv2 *)(cal_obj_getObj(env, list))), AL_OBJ_TYPE_LIST);
}

int cal_list_eql(al_env env, al_obj l1, al_obj l2)
{
	if(cal_obj_getType(env, l1) != cal_obj_getType(env, l2)){
		return 0;
	}
	if(cal_obj_getType(env, l1) == AL_OBJ_TYPE_LIST && cal_obj_getType(env, l2) == AL_OBJ_TYPE_LIST){
		int len1 = cal_obj_length(env, l1);
		if(len1 != cal_obj_length(env, l2)){
			return 0;
		}
		for(int i = 0; i < len1; i++){
			if(!cal_obj_eql(env, cal_list_nth(env, l1, i), cal_list_nth(env, l2, i))){
				return 0;
			}
		}
		return 1;
	}else{
		return cal_obj_eql(env, l1, l2);
	}
}

int cal_list_eqv(al_env env, al_obj l1, al_obj l2)
{
	if(cal_obj_getType(env, l1) != cal_obj_getType(env, l2)){
		return 0;
	}
	if(cal_obj_getType(env, l1) == AL_OBJ_TYPE_LIST && cal_obj_getType(env, l2) == AL_OBJ_TYPE_LIST){
		int len1 = cal_obj_length(env, l1);
		if(len1 != cal_obj_length(env, l2)){
			return 0;
		}
		for(int i = 0; i < len1; i++){
			if(!cal_atom_eqv(env, cal_list_nth(env, l1, i), cal_list_nth(env, l2, i))){
				return 0;
			}
		}
		return 1;
	}else{
		return cal_obj_eqv(env, l1, l2);
	}
}

int cal_list_print(al_env env, al_obj l)
{
	if(cal_obj_getType(env, l) == AL_OBJ_TYPE_LIST){
		int n = 0;
		n += printf("[");
		for(int i = 0; i < cal_list_length(env, l) - 1; i++){
			n += cal_obj_print(env, cal_list_nth(env, l, i));
			n += printf(" ");
		}
		n += cal_obj_print(env, cal_list_nth(env, l, cal_list_length(env, l) - 1));
		n += printf("]");
		return n;
	}
	return 0;
}

int cal_list_println(al_env env, al_obj l)
{
	return cal_list_print(env, l) + printf("\n");
}
