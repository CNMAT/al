#include <stdarg.h>
#include <stdio.h>
#include "al_alist.h"
#include "al_list.h"
#include "al_atom.h"
#include "al_c.h"

al_obj al_alist_copy(al_region r, al_obj al)
{
	if(cal_obj_getType(r, al) == AL_OBJ_TYPE_LIST){
		al_obj copy = cal_alist_alloc(r, 0);
		for(int i = 0; i < cal_alist_length(r, al); i++){
			al_alist_append_m(r, copy, al_list_copy(r, cal_alist_nth(r, al, i)));
		}
		return copy;
	}else{
		return al_obj_copy(r, al);
	}
}

al_obj al_alist_isIndexable(al_region r, al_obj al)
{
	if(cal_obj_getType(r, al) == AL_OBJ_TYPE_ALIST){
		return cal_atom_true(r);
	}else{
		return al_obj_isIndexable(r, al);
	}
}

al_obj al_alist_nth(al_region r, al_obj al, al_obj n)
{
	if(cal_obj_getType(r, al) == AL_OBJ_TYPE_ALIST){
		if(cal_obj_getType(r, n) == AL_OBJ_TYPE_ATOM && cal_atom_isInt(r, n)){
			al_c_int32 i = cal_atom_getInt32(r, n);
			if(!al_c_error(i)){
				return cal_alist_nth(r, al, al_c_value(i));
			}else{
				// check for overflow
				return AL_OBJ_NULL;
			}
		}else{
			return AL_OBJ_NULL;
		}
	}else{
		return AL_OBJ_NULL;
	}
}

al_obj al_alist_length(al_region r, al_obj al)
{
	if(cal_obj_getType(r, al) == AL_OBJ_TYPE_ALIST){
		return cal_atom_int32(r, cal_alist_length(r, al));
	}else{
		return al_obj_length(r, al);
	}
}

al_obj al_alist_eql(al_region r, al_obj al1, al_obj al2)
{
	if(cal_obj_getType(r, al1) != cal_obj_getType(r, al2)){
		return cal_atom_false(r);
	}
	if(cal_obj_getType(r, al1) == AL_OBJ_TYPE_ALIST){
		return cal_alist_eql(r, al1, al2) == 1 ? cal_atom_true(r) : cal_atom_false(r);
	}else{
		return al_obj_eql(r, al1, al2);
	}
}

al_obj al_alist_eqv(al_region r, al_obj al1, al_obj al2)
{
	if(cal_obj_getType(r, al1) != cal_obj_getType(r, al2)){
		return cal_atom_false(r);
	}
	if(cal_obj_getType(r, al1) == AL_OBJ_TYPE_ALIST){
		return cal_alist_eql(r, al1, al2) == 1 ? cal_atom_true(r) : cal_atom_false(r);
	}else{
		return al_obj_eqv(r, al1, al2);
	}
}

al_obj al_alist_append(al_region r, al_obj al, al_obj l)
{
	if(cal_obj_getType(r, al) != AL_OBJ_TYPE_ALIST){
		return al;
	}
	al_obj *copy = al_region_getBytes(r, sizeof(al_obj));
	*copy = l;
	return cal_obj_alloc(r, al_pv2_append(r, (al_pv2 *)(cal_obj_getObj(r, al)), (void *)copy), AL_OBJ_TYPE_ALIST);
}

al_obj al_alist_append_m(al_region r, al_obj al, al_obj l)
{
	if(cal_obj_getType(r, al) != AL_OBJ_TYPE_ALIST){
		return al;
	}
	al_obj *copy = al_region_getBytes(r, sizeof(al_obj));
	*copy = l;
	return cal_obj_alloc(r, al_pv2_append_m(r, (al_pv2 *)(cal_obj_getObj(r, al)), (void *)copy), AL_OBJ_TYPE_ALIST);
}

al_obj al_alist_simpleLookup(al_region r, al_obj al, al_obj name)
{
	if(cal_obj_getType(r, al) == AL_OBJ_TYPE_ALIST && cal_alist_length(r, al) > 0){
		if(cal_obj_getType(r, name) == AL_OBJ_TYPE_ATOM){
			al_c_ptr p = cal_atom_getPtr(r, name);
			if(!al_c_error(p)){
				return cal_alist_simpleLookup(r, al, al_c_value(p));
			}else{
				return AL_OBJ_NULL;
			}
		}else{
			return AL_OBJ_NULL;
		}
	}else{
		return AL_OBJ_NULL;
	}
}

al_obj al_alist_union(al_region r, al_obj al1, al_obj al2)
{
	if(cal_obj_getType(r, al1) == AL_OBJ_TYPE_ALIST || cal_obj_getType(r, al2) == AL_OBJ_TYPE_ALIST){
		// there should be a better way of doing this...
		al_obj out = AL_OBJ_NULL;
		for(int i = 0; i < cal_alist_length(r, al2); i++){
			al_obj l = cal_alist_nth(r, al2, i);
			if(cal_obj_getType(r, al_alist_simpleLookup(r, al1, cal_list_nth(r, l, 0))) == AL_OBJ_TYPE_NULL){
				if(cal_obj_getType(r, out) == AL_OBJ_TYPE_NULL){
					out = al_alist_append(r, al1, l);
				}else{
					out = al_alist_append_m(r, out, l);
				}
			}
		}
		if(cal_obj_getType(r, out) == AL_OBJ_TYPE_NULL){
			return al1;
		}else{
			return out;
		}
	}else{
		return AL_OBJ_NULL;
	}
}

al_obj al_alist_intersection(al_region r, al_obj al1, al_obj al2)
{
	if(cal_obj_getType(r, al1) == AL_OBJ_TYPE_ALIST || cal_obj_getType(r, al2) == AL_OBJ_TYPE_ALIST){
		// there should be a better way of doing this...
		al_obj out = cal_alist_alloc(r, 0);
		for(int i = 0; i < cal_alist_length(r, al1); i++){
			al_obj l = cal_alist_nth(r, al1, i);
			if(cal_obj_getType(r, al_alist_simpleLookup(r, al2, cal_list_nth(r, l, 0))) != AL_OBJ_TYPE_NULL){
				out = al_alist_append_m(r, out, l);
			}
		}
		return out;
	}else{
		return AL_OBJ_NULL;
	}
}

al_obj al_alist_rcomplement(al_region r, al_obj al1, al_obj al2)
{
	if(cal_obj_getType(r, al1) == AL_OBJ_TYPE_ALIST || cal_obj_getType(r, al2) == AL_OBJ_TYPE_ALIST){
		// there should be a better way of doing this...
		al_obj out = cal_alist_alloc(r, 0);
		for(int i = 0; i < cal_alist_length(r, al1); i++){
			al_obj l = cal_alist_nth(r, al1, i);
			if(cal_obj_getType(r, al_alist_simpleLookup(r, al2, cal_list_nth(r, l, 0))) == AL_OBJ_TYPE_NULL){
				out = al_alist_append_m(r, out, l);
			}
		}
		return out;
	}else{
		return AL_OBJ_NULL;
	}
}

al_obj al_alist_applyLambda(al_region r, al_obj lambda, al_obj applicator, al_obj arg, al_obj context)
{

}

al_obj al_alist_applyScalarScalar(al_region r, al_obj fn, al_obj applicator, al_obj applicand, al_obj context)
{

}

al_obj al_alist_applyScalarList(al_region r, al_obj fn, al_obj applicator, al_obj applicand, al_obj context)
{

}

al_obj al_alist_applyListScalar(al_region r, al_obj fn, al_obj applicator, al_obj applicand, al_obj context)
{

}

al_obj al_alist_applyListList(al_region r, al_obj fn, al_obj applicator, al_obj applicand, al_obj context)
{

}

al_obj al_alist_apply(al_region r, al_obj fn, al_obj applicator, al_obj applicand, al_obj context)
{
	if(cal_obj_getType(r, applicand) == AL_OBJ_TYPE_LIST){
		if(cal_list_length(r, applicand) == 2){
			return al_alist_apply(r, fn, applicator, cal_list_nth(r, applicand, 1), context);
		}else{
			al_obj out = cal_list_alloc(r, 0);
			for(int i = 1; i < cal_list_length(r, applicand); i++){
				al_list_append_m(r, out, al_alist_apply(r, fn, applicator, cal_list_nth(r, applicand, i), context));
			}
			return out;
		}
	}else{
		if(cal_list_length(r, fn) <= 2){
			// fn is the first element of the list after the lhs address
			fn = cal_list_nth(r, fn, 1);
			if(cal_obj_getType(r, applicand) == AL_OBJ_TYPE_ATOM){
				if(AL_TYPE_ISINT(cal_atom_getType(r, applicand))){
					// nth
					if(cal_obj_getType(r, fn) == AL_OBJ_TYPE_NULL){
						// nth on this
						al_obj thismsg = cal_alist_simpleLookup(r, context, "this");
						if(cal_obj_getType(r, thismsg) == AL_OBJ_TYPE_LIST && cal_list_length(r, thismsg) > 1){
							al_obj this = cal_list_nth(r, thismsg, 1);
							return cal_alist_alloc(r, 1, al_list_prepend(r, al_alist_nth(r, this, applicand), cal_atom_symbol(r, "value")));
						}else{
							return AL_OBJ_NULL;
						}
					}else if(cal_obj_isIndexable(r, fn)){
						// nth on fn which should be a string, alist or something
						al_obj ret = al_obj_nth(r, fn, applicand);
						switch(cal_obj_getType(r, ret)){
						case AL_OBJ_TYPE_ALIST:
						case AL_OBJ_TYPE_ATOM:
							return cal_alist_alloc(r, 1, cal_list_alloc(r, 2, cal_atom_symbol(r, "value"), ret));
						case AL_OBJ_TYPE_LIST:
							return cal_alist_alloc(r, 1, al_list_prepend(r, ret, cal_atom_symbol(r, "value")));
						}
					}
				}else if(cal_atom_getType(r, applicand) == AL_TYPE_SYM || cal_atom_getType(r, applicand) == AL_TYPE_STR){
					// lookup
					if(cal_obj_getType(r, fn) == AL_OBJ_TYPE_NULL){
						al_obj ret = al_alist_simpleLookup(r, context, applicand);
						if(cal_obj_getType(r, ret) == AL_OBJ_TYPE_NULL){
							return AL_OBJ_NULL;
						}else{
							return cal_alist_alloc(r, 1, al_list_prepend(r, ret, cal_atom_symbol(r, "value")));
						}
					}else if(cal_obj_getType(r, fn) == AL_OBJ_TYPE_ALIST){
						// lookup in alist
						al_obj ret = al_alist_simpleLookup(r, fn, applicand);
						if(cal_obj_getType(r, ret) == AL_OBJ_TYPE_NULL){
							return AL_OBJ_NULL;
						}else{
							return cal_alist_alloc(r, 1, al_list_prepend(r, ret, cal_atom_symbol(r, "value")));
						}
					}
				}else{
					// rhs is either bool, float, or blob
					return AL_OBJ_NULL;
				}
			}else if(cal_obj_getType(r, applicand) == AL_OBJ_TYPE_ALIST){
				if(cal_obj_getType(r, fn) == AL_OBJ_TYPE_NULL){
					// union or funcall with this
					al_obj thismsg = cal_alist_simpleLookup(r, context, "this");
					if(cal_obj_getType(r, thismsg) == AL_OBJ_TYPE_LIST && cal_list_length(r, thismsg) > 1){
						al_obj this = cal_list_nth(r, thismsg, 1);
						return cal_alist_alloc(r, 1, cal_list_alloc(r, 2, cal_atom_symbol(r, "value"), al_alist_union(r, applicand, this)));
					}else{
						return cal_alist_alloc(r, 1, al_list_prepend(r, applicand, cal_atom_symbol(r, "value")));
					}					
				}else if(cal_obj_getType(r, fn) == AL_OBJ_TYPE_ALIST){
					// union or funcall with alist
					return cal_alist_alloc(r, 1, cal_list_alloc(r, 2, cal_atom_symbol(r, "value"), al_alist_union(r, applicand, fn)));
				}else{
					return AL_OBJ_NULL;
				}
			}else{
				return AL_OBJ_NULL;
			}
		}else{
			// fn is the list
			if(cal_obj_getType(r, applicand) == AL_OBJ_TYPE_ATOM){
				if(AL_TYPE_ISINT(cal_atom_getType(r, applicand))){
					// nth
					al_c_int32 i = cal_atom_getInt32(r, applicand);
					if(!al_c_error(i)){
						al_obj nth = cal_list_nth(r, fn, al_c_value(i) + 1);
						return cal_alist_alloc(r, 1, cal_list_alloc(r, 2, cal_atom_symbol(r, "value"), nth));
					}else{
						// check to see if the error was because of overflow
						return AL_OBJ_NULL;
					}
				}else{
					// rhs is either bool, float, or blob
					return AL_OBJ_NULL;
				}
			}else{
				return AL_OBJ_NULL;
			}
		}
	}
}

al_obj cal_alist_getEntryPointVal(al_region r, al_obj entrypoint, char *key)
{
	if(cal_obj_getType(r, entrypoint) == AL_OBJ_TYPE_ALIST){
		al_obj a = cal_alist_simpleLookup(r, entrypoint, "key");
		if(cal_obj_getType(r, a) == AL_OBJ_TYPE_LIST && cal_list_length(r, a) > 1){
			al_obj aa = cal_list_nth(r, a, 1);
			if(cal_obj_getType(r, aa) == AL_OBJ_TYPE_ATOM){
				return aa;
			}
		}
	}
	return AL_OBJ_NULL;
}

static al_obj al_alist_builtinEval(al_region r, al_obj al, al_obj context)
{
	al_obj apply_list = cal_alist_simpleLookup(r, al, "@");
	al_obj lambda_list = cal_alist_simpleLookup(r, al, "\\");
	al_obj lazy_applicator = cal_atom_symbol(r, "@");
	al_obj left_eager_applicator = cal_atom_symbol(r, "!@");
	al_obj right_eager_applicator = cal_atom_symbol(r, "@!");
	al_obj eager_applicator = cal_atom_symbol(r, "!@!");
	//al_obj value = cal_atom_symbol(r, "value");
	if(cal_obj_getType(r, apply_list) == AL_OBJ_TYPE_LIST){
		al_obj applicator = cal_list_nth(r, apply_list, 1);
		al_obj lhs = cal_alist_simpleLookup(r, al, "lhs");
		al_obj rhs = cal_alist_simpleLookup(r, al, "rhs");

		al_obj ll = lhs;
		al_obj rr = rhs;
		int ex = 0;
		if(cal_atom_eql(r, applicator, lazy_applicator)){
			;
		}else if(cal_atom_eql(r, applicator, left_eager_applicator)){
			ex |= 1;			
		}else if(cal_atom_eql(r, applicator, right_eager_applicator)){
			ex |= 2;
		}else if(cal_atom_eql(r, applicator, eager_applicator)){
			ex = 3;
		}else{ // user-defined applicator
			
		}
		if(ex & 1 && cal_obj_getType(r, ll) == AL_OBJ_TYPE_LIST && cal_list_length(r, ll) >= 2){
			ll = al_list_eval(r, ll, context);
		}
		if(ex & 2 && cal_obj_getType(r, rr) == AL_OBJ_TYPE_LIST && cal_list_length(r, rr) >= 2){
			rr = al_list_eval(r, rr, context);
		}
		al_obj ret = al_alist_apply(r, ll, applicator, rr, context);
		if(cal_obj_getType(r, ret) == AL_OBJ_TYPE_NULL){
			// maybe this should be wrapped in a lambda?
			return al;
		}else{
			return ret;
		}
	}else if(cal_obj_getType(r, lambda_list) == AL_OBJ_TYPE_LIST){
		return al;
	}else{
		al_obj out = cal_alist_alloc(r, 0);
		al_obj thismsg = cal_alist_simpleLookup(r, context, "this");
		al_obj parent;
		if(cal_obj_getType(r, thismsg) == AL_OBJ_TYPE_LIST && cal_list_length(r, thismsg) > 1){
			parent = cal_list_alloc(r, 2, cal_atom_symbol(r, "parent"), cal_list_nth(r, thismsg, 1));
		}else{
			parent = cal_list_alloc(r, 1, cal_atom_symbol(r, "parent"));
		}
		al_obj this = cal_list_alloc(r, 2, cal_atom_symbol(r, "this"), al);
		context = al_alist_union(r, cal_alist_alloc(r, 2, this, parent), context);
		for(int i = 0; i < cal_alist_length(r, al); i++){
			al_alist_append_m(r, out, al_list_eval(r, cal_alist_nth(r, al, i), context));
		}
		return out;
	}
}

al_obj al_alist_eval(al_region r, al_obj al, al_obj context)
{
	if(cal_obj_getType(r, al) != AL_OBJ_TYPE_ALIST){
		return al_obj_eval(r, al, context);
	}
	al_obj entrypointmsg = cal_alist_simpleLookup(r, al, "entrypoint");
	al_obj entrypoint;
	if(cal_obj_getType(r, entrypointmsg) != AL_OBJ_TYPE_NULL){
		entrypoint = cal_list_nth(r, entrypointmsg, 1);
		context = al_alist_union(r, cal_alist_alloc(r, 1, entrypointmsg), context);
	}else{
		entrypointmsg = cal_alist_simpleLookup(r, al, "entrypoint");
		if(cal_obj_getType(r, entrypointmsg) != AL_OBJ_TYPE_NULL){
			entrypoint = cal_list_nth(r, entrypointmsg, 1);
		}else{
			entrypoint = AL_OBJ_NULL;
		}
	}
	if(cal_obj_getType(r, entrypoint) == AL_OBJ_TYPE_NULL){
		return al_alist_builtinEval(r, al, context);
	}
	
	al_obj application = cal_alist_getEntryPointVal(r, entrypoint, "application");
	al_obj lazy_applicator = cal_alist_getEntryPointVal(r, entrypoint, "lazy_applicator");
	al_obj left_eager_applicator = cal_alist_getEntryPointVal(r, entrypoint, "left_eager_applicator");
	al_obj right_eager_applicator = cal_alist_getEntryPointVal(r, entrypoint, "right_eager_applicator");
	al_obj eager_applicator = cal_alist_getEntryPointVal(r, entrypoint, "eager_applicator");
	if(cal_obj_getType(r, application) == AL_OBJ_TYPE_NULL){
		application = cal_atom_symbol(r, "@");
	}
	if(cal_obj_getType(r, lazy_applicator) == AL_OBJ_TYPE_NULL){
		lazy_applicator = cal_atom_symbol(r, "@");
	}
	if(cal_obj_getType(r, left_eager_applicator) == AL_OBJ_TYPE_NULL){
		left_eager_applicator = cal_atom_symbol(r, "!@");
	}
	if(cal_obj_getType(r, right_eager_applicator) == AL_OBJ_TYPE_NULL){
		right_eager_applicator = cal_atom_symbol(r, "@!");
	}
	if(cal_obj_getType(r, eager_applicator) == AL_OBJ_TYPE_NULL){
		eager_applicator = cal_atom_symbol(r, "!@!");
	}
	
	al_obj lambda = cal_alist_getEntryPointVal(r, entrypoint, "lambda");
	if(cal_obj_getType(r, lambda) == AL_OBJ_TYPE_NULL){
		lambda = cal_atom_symbol(r, "\\");
	}

	al_obj apply_list = al_alist_simpleLookup(r, al, application);
	al_obj lambda_list = al_alist_simpleLookup(r, al, lambda);

	if(cal_obj_getType(r, apply_list) == AL_OBJ_TYPE_LIST){
		al_obj applicator = cal_list_nth(r, apply_list, 1);
		al_obj left_arg = cal_alist_getEntryPointVal(r, entrypoint, "left_arg");
		al_obj right_arg = cal_alist_getEntryPointVal(r, entrypoint, "right_arg");
		if(cal_obj_getType(r, left_arg) == AL_OBJ_TYPE_NULL){
			left_arg = cal_atom_symbol(r, "lhs");
		}
		if(cal_obj_getType(r, right_arg) == AL_OBJ_TYPE_NULL){
			right_arg = cal_atom_symbol(r, "rhs");
		}
		al_obj lhs = al_alist_simpleLookup(r, al, left_arg);
		al_obj rhs = al_alist_simpleLookup(r, al, right_arg);

		al_obj ll = lhs;
		al_obj rr = rhs;
		int ex = 0;
		if(cal_atom_eql(r, applicator, lazy_applicator)){
		}else if(cal_atom_eql(r, applicator, left_eager_applicator)){
			ex |= 1;			
		}else if(cal_atom_eql(r, applicator, right_eager_applicator)){
			ex |= 2;
		}else if(cal_atom_eql(r, applicator, eager_applicator)){
			ex = 3;
		}else{ // user-defined applicator
			
		}
		if(ex & 1 && cal_obj_getType(r, ll) == AL_OBJ_TYPE_LIST && cal_list_length(r, ll) >= 2){
			//ll = al_list_eval(r, ll, context);
		}
		if(ex & 2 && cal_obj_getType(r, rr) == AL_OBJ_TYPE_LIST && cal_list_length(r, rr) >= 2){
			//rr = al_list_eval(r, rr, context);
		}
		al_obj applyfnsym = cal_alist_getEntryPointVal(r, entrypoint, "applyfn");
		if(cal_obj_getType(r, applyfnsym) == AL_OBJ_TYPE_NULL){
			return al_alist_apply(r, ll, applicator, rr, context);
		}else{

		}

	}else if(cal_obj_getType(r, lambda_list) == AL_OBJ_TYPE_LIST){
		return al;
	}else{
		al_obj out = cal_alist_alloc(r, 0);
		for(int i = 0; i < cal_alist_length(r, al); i++){
			al_obj thismsg = cal_alist_simpleLookup(r, context, "this");
			al_obj parent;
			if(cal_obj_getType(r, thismsg) == AL_OBJ_TYPE_LIST && cal_list_length(r, thismsg) > 1){
				parent = cal_list_alloc(r, 2, cal_atom_symbol(r, "parent"), cal_list_nth(r, thismsg, 1));
			}else{
				parent = cal_list_alloc(r, 1, cal_atom_symbol(r, "parent"));
			}
			al_obj this = cal_list_alloc(r, 2, cal_atom_symbol(r, "this"), al);
			context = al_alist_union(r, cal_alist_alloc(r, 2, this, parent), context);
			//al_alist_append_m(r, out, al_list_eval(r, cal_alist_nth(r, al, i), context));
		}
		return out;
	}
}


//////////////////////////////////////////////////
// C API
//////////////////////////////////////////////////

al_obj cal_alist_alloc(al_region r, int n, ...)
{
	al_pv2 *l = al_pv2_alloc(r, NULL);
	va_list ap;
	va_start(ap, n);
	for(int i = 0; i < n; i++){
		al_obj li = va_arg(ap, al_obj);
		if(cal_obj_getType(r, li) != AL_OBJ_TYPE_LIST){
			printf("%s: only accepts lists, but got a %s\n", __func__, cal_obj_getTypeName(r, li));
			return AL_OBJ_NULL;
		}
		al_obj *copy = al_region_getBytes(r, sizeof(al_obj));
		*copy = li;
		al_pv2_append_m(r, l, (void *)copy);
	}
	va_end(ap);
	return cal_obj_alloc(r, l, AL_OBJ_TYPE_ALIST);
}

int cal_alist_isIndexable(al_region r, al_obj al)
{
	if(cal_obj_getType(r, al) == AL_OBJ_TYPE_ALIST){
		return 1;
	}else{
		return cal_obj_isIndexable(r, al);
	}
}

al_obj cal_alist_nth(al_region r, al_obj al, int n)
{
	if(cal_obj_getType(r, al) == AL_OBJ_TYPE_ALIST){
		al_obj *o = (al_obj *)al_pv2_nth((al_pv2 *)(cal_obj_getObj(r, al)), n);
		if(o && cal_obj_getType(r, *o) == AL_OBJ_TYPE_LIST){
			return *o;
		}
	}
	return cal_obj_alloc(r, NULL, 0);
}



int cal_alist_length(al_region r, al_obj al)
{
	if(cal_obj_getType(r, al) == AL_OBJ_TYPE_ALIST){
		return al_pv2_length((al_pv2 *)(cal_obj_getObj(r, al)));
	}
	return 0;
}

al_obj cal_alist_append(al_region r, al_obj al, al_obj l)
{
	if(cal_obj_getType(r, al) != AL_OBJ_TYPE_ALIST){
		return al;
	}
	al_obj *copy = al_region_getBytes(r, sizeof(al_obj));
	*copy = l;
	return cal_obj_alloc(r, al_pv2_append(r, (al_pv2 *)(cal_obj_getObj(r, al)), (void *)copy), AL_OBJ_TYPE_ALIST);
}

al_obj cal_alist_append_m(al_region r, al_obj al, al_obj l)
{
	if(cal_obj_getType(r, al) != AL_OBJ_TYPE_ALIST){
		return al;
	}
	al_obj *copy = al_region_getBytes(r, sizeof(al_obj));
	*copy = l;
	return cal_obj_alloc(r, al_pv2_append_m(r, (al_pv2 *)(cal_obj_getObj(r, al)), (void *)copy), AL_OBJ_TYPE_ALIST);
}

int cal_alist_eql(al_region r, al_obj al1, al_obj al2)
{
	if(cal_obj_getType(r, al1) != cal_obj_getType(r, al2)){
		return 0;
	}
	if(cal_obj_getType(r, al1) != AL_OBJ_TYPE_ALIST || cal_obj_getType(r, al2) != AL_OBJ_TYPE_ALIST){
		return cal_obj_eql(r, al1, al2);
	}
	int al1len = cal_alist_length(r, al1);
	if(al1len != cal_alist_length(r, al2)){
		return 0;
	}
	for(int i = 0; i < al1len; i++){
		if(!cal_list_eql(r, cal_alist_nth(r, al1, i), cal_alist_nth(r, al2, i))){
			return 0;
		}
	}
	return 1;
}

int cal_alist_eqv(al_region r, al_obj al1, al_obj al2)
{
	if(cal_obj_getType(r, al1) != cal_obj_getType(r, al2)){
		return 0;
	}
	if(cal_obj_getType(r, al1) != AL_OBJ_TYPE_ALIST || cal_obj_getType(r, al2) != AL_OBJ_TYPE_ALIST){
		return cal_obj_eql(r, al1, al2);
	}
	int al1len = cal_alist_length(r, al1);
	if(al1len != cal_alist_length(r, al2)){
		return 0;
	}
	for(int i = 0; i < al1len; i++){
		if(!cal_list_eqv(r, cal_alist_nth(r, al1, i), cal_alist_nth(r, al2, i))){
			return 0;
		}
	}
	return 1;
}

al_obj cal_alist_simpleLookup(al_region r, al_obj al, char *name)
{
	// this is stupid 
	if(cal_obj_getType(r, al) == AL_OBJ_TYPE_ALIST && name){
		al_obj namestr = cal_atom_string(r, name);
		for(int i = 0; i < cal_alist_length(r, al); i++){
			al_obj l = cal_alist_nth(r, al, i);
			al_obj a = cal_list_nth(r, l, 0);
			al_c_int32 res = cal_atom_strcmp(r, namestr, a);
			if(!al_c_error(res)){
				if(al_c_value(res) == 0){
					return l;
				}
			}
		}
	}
	return AL_OBJ_NULL;
}

int cal_alist_print(al_region r, al_obj al)
{
	if(cal_obj_getType(r, al) == AL_OBJ_TYPE_ALIST){
		int n = 0;
		n += printf("{");
		for(int i = 0; i < cal_alist_length(r, al) - 1; i++){
			n += cal_list_println(r, cal_alist_nth(r, al, i));
		}
		n += cal_list_print(r, cal_alist_nth(r, al, cal_alist_length(r, al) - 1));
		n += printf("}");
		return n;
	}
	return 0;
}

int cal_alist_println(al_region r, al_obj al)
{
	return cal_alist_print(r, al) + printf("\n");
}
