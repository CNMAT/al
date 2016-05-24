#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include "al_alist.h"
#include "al_list.h"
#include "al_atom.h"
#include "al_c.h"

#define __OSC_PROFILE__
#include "../libo/osc_profile.h"

al_obj al_alist_copy(al_env env, al_obj al)
{
	if(cal_obj_getType(env, al) == AL_OBJ_TYPE_LIST){
		al_obj copy = cal_alist_alloc(env, 0);
		for(int i = 0; i < cal_alist_length(env, al); i++){
			al_alist_append_m(env, copy, al_list_copy(env, cal_alist_nth(env, al, i)));
		}
		return copy;
	}else{
		return al_obj_copy(env, al);
	}
}

al_obj al_alist_isIndexable(al_env env, al_obj al)
{
	if(cal_obj_getType(env, al) == AL_OBJ_TYPE_ALIST){
		return cal_atom_true(env);
	}else{
		return al_obj_isIndexable(env, al);
	}
}

al_obj al_alist_nth(al_env env, al_obj al, al_obj n)
{
	if(cal_obj_getType(env, al) == AL_OBJ_TYPE_ALIST){
		if(cal_obj_getType(env, n) == AL_OBJ_TYPE_ATOM && cal_atom_isInt(env, n)){
			al_c_int32 i = cal_atom_getInt32(env, n);
			if(!al_c_error(i)){
				return cal_alist_nth(env, al, al_c_value(i));
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

al_obj al_alist_length(al_env env, al_obj al)
{
	if(cal_obj_getType(env, al) == AL_OBJ_TYPE_ALIST){
		return cal_atom_int32(env, cal_alist_length(env, al));
	}else{
		return al_obj_length(env, al);
	}
}

al_obj al_alist_eql(al_env env, al_obj al1, al_obj al2)
{
	if(cal_obj_getType(env, al1) != cal_obj_getType(env, al2)){
		return cal_atom_false(env);
	}
	if(cal_obj_getType(env, al1) == AL_OBJ_TYPE_ALIST){
		return cal_alist_eql(env, al1, al2) == 1 ? cal_atom_true(env) : cal_atom_false(env);
	}else{
		return al_obj_eql(env, al1, al2);
	}
}

al_obj al_alist_eqv(al_env env, al_obj al1, al_obj al2)
{
	if(cal_obj_getType(env, al1) != cal_obj_getType(env, al2)){
		return cal_atom_false(env);
	}
	if(cal_obj_getType(env, al1) == AL_OBJ_TYPE_ALIST){
		return cal_alist_eql(env, al1, al2) == 1 ? cal_atom_true(env) : cal_atom_false(env);
	}else{
		return al_obj_eqv(env, al1, al2);
	}
}

al_obj al_alist_append(al_env env, al_obj al, al_obj l)
{
	if(cal_obj_getType(env, al) != AL_OBJ_TYPE_ALIST){
		return al;
	}
	al_obj *copy = (al_obj *)al_env_getBytes(env, sizeof(al_obj));
	*copy = l;
	return cal_obj_alloc(env, al_pv2_append(al_env_getRegion(env), (al_pv2 *)(cal_obj_getObj(env, al)), (void *)copy), AL_OBJ_TYPE_ALIST);
}

al_obj al_alist_append_m(al_env env, al_obj al, al_obj l)
{
	if(cal_obj_getType(env, al) != AL_OBJ_TYPE_ALIST){
		return al;
	}
	al_obj *copy = (al_obj *)al_env_getBytes(env, sizeof(al_obj));
	*copy = l;
	return cal_obj_alloc(env, al_pv2_append_m(al_env_getRegion(env), (al_pv2 *)(cal_obj_getObj(env, al)), (void *)copy), AL_OBJ_TYPE_ALIST);
}

al_obj al_alist_simpleLookup(al_env env, al_obj al, al_obj name)
{
	if(cal_obj_getType(env, al) == AL_OBJ_TYPE_ALIST && cal_alist_length(env, al) > 0){
		if(cal_obj_getType(env, name) == AL_OBJ_TYPE_ATOM){
			/*
			al_c_ptr p = cal_atom_getPtr(env, name);
			if(!al_c_error(p)){
				return cal_alist_simpleLookup(env, al, al_c_value(p));
			}else{
				return AL_OBJ_NULL;
			}
			*/
			for(int i = 0; i < cal_alist_length(env, al); i++){
				al_obj l = cal_alist_nth(env, al, i);
				if(cal_obj_eql(env, cal_list_nth(env, l, 0), name)){
					return l;
				}
			}
			return AL_OBJ_NULL;
		}else{
			return AL_OBJ_NULL;
		}
	}else{
		return AL_OBJ_NULL;
	}
}

al_obj al_alist_union(al_env env, al_obj al1, al_obj al2)
{
	if(cal_obj_getType(env, al1) == AL_OBJ_TYPE_ALIST || cal_obj_getType(env, al2) == AL_OBJ_TYPE_ALIST){
		// there should be a better way of doing this...
		al_obj out = AL_OBJ_NULL;
		for(int i = 0; i < cal_alist_length(env, al2); i++){
			al_obj l = cal_alist_nth(env, al2, i);
			if(cal_obj_getType(env, al_alist_simpleLookup(env, al1, cal_list_nth(env, l, 0))) == AL_OBJ_TYPE_NULL){
				if(cal_obj_getType(env, out) == AL_OBJ_TYPE_NULL){
					out = al_alist_append(env, al1, l);
				}else{
					//out = al_alist_append_m(env, out, l);
					out = al_alist_append(env, out, l);
				}
			}
		}
		if(cal_obj_getType(env, out) == AL_OBJ_TYPE_NULL){
			return al1;
		}else{
			return out;
		}
	}else{
		return AL_OBJ_NULL;
	}
}

al_obj al_alist_intersection(al_env env, al_obj al1, al_obj al2)
{
	if(cal_obj_getType(env, al1) == AL_OBJ_TYPE_ALIST || cal_obj_getType(env, al2) == AL_OBJ_TYPE_ALIST){
		// there should be a better way of doing this...
		al_obj out = cal_alist_alloc(env, 0);
		for(int i = 0; i < cal_alist_length(env, al1); i++){
			al_obj l = cal_alist_nth(env, al1, i);
			if(cal_obj_getType(env, al_alist_simpleLookup(env, al2, cal_list_nth(env, l, 0))) != AL_OBJ_TYPE_NULL){
				out = al_alist_append_m(env, out, l);
				continue;
			}
		}
		return out;
	}else{
		return AL_OBJ_NULL;
	}
}

al_obj al_alist_rcomplement(al_env env, al_obj al1, al_obj al2)
{
	if(cal_obj_getType(env, al1) == AL_OBJ_TYPE_ALIST || cal_obj_getType(env, al2) == AL_OBJ_TYPE_ALIST){
		// there should be a better way of doing this...
		al_obj out = cal_alist_alloc(env, 0);
		for(int i = 0; i < cal_alist_length(env, al1); i++){
			al_obj l = cal_alist_nth(env, al1, i);
			if(cal_obj_getType(env, al_alist_simpleLookup(env, al2, cal_list_nth(env, l, 0))) == AL_OBJ_TYPE_NULL){
				out = al_alist_append_m(env, out, l);
			}
		}
		return out;
	}else{
		return AL_OBJ_NULL;
	}
}

static al_obj al_alist_addThisToContext(al_env env, al_obj this, al_obj context)
{
	al_obj thismsg = cal_alist_simpleLookup(env, context, "this");
	al_obj parent;
	if(cal_obj_getType(env, thismsg) == AL_OBJ_TYPE_LIST && cal_list_length(env, thismsg) > 1){
		parent = cal_list_alloc(env, 2, cal_atom_symbol(env, "parent"), cal_list_nth(env, thismsg, 1));
	}else{
		parent = cal_list_alloc(env, 1, cal_atom_symbol(env, "parent"));
	}
	al_obj _this = cal_list_alloc(env, 2, cal_atom_symbol(env, "this"), this);
	context = al_alist_union(env, cal_alist_alloc(env, 2, _this, parent), context);
	return context;
}

al_obj al_alist_apply(al_env env, al_obj fn, al_obj applicator, al_obj applicand, al_obj context)
{
	if(cal_obj_getType(env, applicand) == AL_OBJ_TYPE_LIST){
		if(cal_list_length(env, applicand) == 2){
			return al_alist_apply(env, fn, applicator, cal_list_nth(env, applicand, 1), context);
		}else{
			al_obj out = cal_list_alloc(env, 0);
			for(int i = 1; i < cal_list_length(env, applicand); i++){
				al_list_append_m(env, out, al_alist_apply(env, fn, applicator, cal_list_nth(env, applicand, i), context));
			}
			return out;
		}
	}else{
		if(cal_obj_getType(env, fn) == AL_OBJ_TYPE_ATOM || (cal_obj_getType(env, fn) == AL_OBJ_TYPE_LIST && cal_list_length(env, fn) <= 2)){
			// fn is the first element of the list after the lhs address
			if((cal_obj_getType(env, fn) == AL_OBJ_TYPE_LIST && cal_list_length(env, fn) <= 2)){
				fn = cal_list_nth(env, fn, 1);
			}
			if(cal_obj_getType(env, applicand) == AL_OBJ_TYPE_ATOM){
				if(AL_TYPE_ISINT(cal_atom_getType(env, applicand))){
					// nth
					if(cal_obj_getType(env, fn) == AL_OBJ_TYPE_NULL){
						// nth on this
						al_obj thismsg = cal_alist_simpleLookup(env, context, "this");
						if(cal_obj_getType(env, thismsg) == AL_OBJ_TYPE_LIST && cal_list_length(env, thismsg) > 1){
							al_obj this = cal_list_nth(env, thismsg, 1);
							return cal_alist_alloc(env, 1, al_list_prepend(env, al_alist_nth(env, this, applicand), cal_atom_symbol(env, "value")));
						}else{
							goto def;
						}
					}else if(cal_obj_isIndexable(env, fn)){
						// nth on fn which should be a string, alist or something
						al_obj ret = al_obj_nth(env, fn, applicand);
						switch(cal_obj_getType(env, ret)){
						case AL_OBJ_TYPE_ALIST:
						case AL_OBJ_TYPE_ATOM:
							return cal_alist_alloc(env, 1, cal_list_alloc(env, 2, cal_atom_symbol(env, "value"), ret));
						case AL_OBJ_TYPE_LIST:
							return cal_alist_alloc(env, 1, al_list_prepend(env, ret, cal_atom_symbol(env, "value")));
						case AL_OBJ_TYPE_NULL:
							goto def;
						}
					}
				}else if(cal_atom_getType(env, applicand) == AL_TYPE_SYM || cal_atom_getType(env, applicand) == AL_TYPE_STR){
					// lookup
					if(cal_obj_getType(env, fn) == AL_OBJ_TYPE_NULL){
						al_obj ret = al_alist_simpleLookup(env, context, applicand);
						if(cal_obj_getType(env, ret) == AL_OBJ_TYPE_NULL){
							goto def;
						}else{
							return cal_alist_alloc(env, 1, al_list_prepend(env, ret, cal_atom_symbol(env, "value")));
						}
					}else if(cal_obj_getType(env, fn) == AL_OBJ_TYPE_ALIST){
						// lookup in alist
						al_obj ret = al_alist_simpleLookup(env, fn, applicand);
						if(cal_obj_getType(env, ret) == AL_OBJ_TYPE_NULL){
							goto def;
						}else{
							return cal_alist_alloc(env, 1, al_list_prepend(env, ret, cal_atom_symbol(env, "value")));
						}
					}
				}else{
					// rhs is either bool, float, or blob
					goto def;
				}
			}else if(cal_obj_getType(env, applicand) == AL_OBJ_TYPE_ALIST){
				if(cal_obj_getType(env, fn) == AL_OBJ_TYPE_NULL){
					// union or funcall with this
					al_obj thismsg = cal_alist_simpleLookup(env, context, "this");
					if(cal_obj_getType(env, thismsg) == AL_OBJ_TYPE_LIST && cal_list_length(env, thismsg) > 1){
						al_obj this = cal_list_nth(env, thismsg, 1);
						return cal_alist_alloc(env, 1, cal_list_alloc(env, 2, cal_atom_symbol(env, "value"), al_alist_union(env, applicand, this)));
					}else{
						return cal_alist_alloc(env, 1, al_list_prepend(env, applicand, cal_atom_symbol(env, "value")));
					}					
				}else if(cal_obj_getType(env, fn) == AL_OBJ_TYPE_ALIST){
					// union or funcall with alist
					if(cal_obj_getType(env, cal_alist_simpleLookup(env, fn, "lambda")) == AL_OBJ_TYPE_NULL){
						return cal_alist_alloc(env, 1, cal_list_alloc(env, 2, cal_atom_symbol(env, "value"), al_alist_union(env, applicand, fn)));
					}else{
						// lambda application
						al_obj expralist = cal_alist_alloc(env, 1, cal_list_alloc(env, 1, cal_atom_symbol(env, "expr")));
						al_obj lambdaalist = cal_alist_alloc(env, 1, cal_list_alloc(env, 1, cal_atom_symbol(env, "lambda")));
						al_obj argnames = cal_list_nth(env, cal_alist_simpleLookup(env, fn, "argnames"), 1);
						al_obj boundargs = al_alist_intersection(env, applicand, argnames);
						if(cal_alist_length(env, argnames) == cal_alist_length(env, boundargs)){

							al_obj expr = al_alist_intersection(env, fn, expralist);
							al_obj ctxt = al_alist_union(env, applicand, al_alist_union(env, al_alist_rcomplement(env, al_alist_rcomplement(env, fn, expralist), lambdaalist), context));
							al_obj e = al_obj_eval(env, expr, ctxt);
							al_obj lu = al_alist_union(env, applicand, fn);
							if(cal_obj_getType(env, e) == AL_OBJ_TYPE_ALIST){
								al_obj ee = cal_alist_simpleLookup(env, e, "expr");
								if(cal_obj_getType(env, ee) == AL_OBJ_TYPE_LIST){
									int n = cal_list_length(env, ee);
									if(n > 2 || (n == 2 && cal_obj_getType(env, cal_list_nth(env, ee, 1)) == AL_OBJ_TYPE_ATOM)){
										al_obj v = cal_list_alloc(env, 1, cal_atom_symbol(env, "value"));
										for(int i = 1; i < n; i++){
											v = al_list_append_m(env, v, al_alist_union(env, cal_list_nth(env, ee, i), lu));
										}
										return cal_alist_alloc(env, 1, v);
									}else if(n == 2){
										//return al_alist_union(env, cal_list_nth(env, ee, 1), lu);
										return cal_list_nth(env, ee, 1);
									}else if(n == 1){
										goto def;
									}else{
										return lu;
									}
								}else{
									return lu;
								}
							}else{
								return lu;
							}
						}else{
							al_obj unbound = al_alist_rcomplement(env, argnames, boundargs);
							return al_alist_union(env, applicand, al_alist_union(env, cal_alist_alloc(env, 1, cal_list_alloc(env, 2, cal_atom_symbol(env, "argnames"), unbound)), fn));
						}
					}
				}else if(cal_obj_getType(env, fn) == AL_OBJ_TYPE_ATOM && cal_atom_getType(env, fn) == AL_TYPE_FN){
					al_c_fn cfn = cal_atom_getFn(env, fn);
					if(!al_c_error(cfn)){
						return al_c_value(cfn)(env, applicand, al_alist_addThisToContext(env, fn, context));
					}else{
						goto def;
					}
				}else{
					goto def;
				}
			}else{
				goto def;
			}
		}else{
			// fn is the list
			if(cal_obj_getType(env, applicand) == AL_OBJ_TYPE_ATOM){
				if(AL_TYPE_ISINT(cal_atom_getType(env, applicand))){
					// nth
					al_c_int32 i = cal_atom_getInt32(env, applicand);
					if(!al_c_error(i)){
						al_obj nth = cal_list_nth(env, fn, al_c_value(i) + 1);
						return cal_alist_alloc(env, 1, cal_list_alloc(env, 2, cal_atom_symbol(env, "value"), nth));
					}else{
						// check to see if the error was because of overflow
						goto def;
					}
				}else{
					// rhs is either bool, float, or blob
					goto def;
				}
			}else{
				goto def;
			}
		}
	}
 def:
	return al_alist_makeApplication(env, fn, applicator, applicand);
}

al_obj al_alist_getEntryPointVal(al_env env, al_obj entrypoint, al_obj key)
{
	al_obj a = al_alist_simpleLookup(env, entrypoint, key);
	if(cal_obj_getType(env, a) == AL_OBJ_TYPE_LIST && cal_list_length(env, a) > 1){
		al_obj aa = cal_list_nth(env, a, 1);
		if(cal_obj_getType(env, aa) == AL_OBJ_TYPE_ATOM){
			return aa;
		}
	}
	return AL_OBJ_NULL;
}

al_obj cal_alist_getEntryPointVal(al_env env, al_obj entrypoint, char *key)
{
	al_obj a = cal_alist_simpleLookup(env, entrypoint, key);
	if(cal_obj_getType(env, a) == AL_OBJ_TYPE_LIST && cal_list_length(env, a) > 1){
		al_obj aa = cal_list_nth(env, a, 1);
		if(cal_obj_getType(env, aa) == AL_OBJ_TYPE_ATOM){
			return aa;
		}
	}
	return AL_OBJ_NULL;
}

al_obj cal_alist_bindArgs(al_env env, al_obj fn, al_obj context, ...)
{
	al_obj entrypoint = cal_alist_simpleLookup(env, context, "entrypoint");
	al_obj sym_lambda = cal_alist_getEntryPointVal(env, entrypoint, "lambda");
	al_obj sym_args = cal_alist_getEntryPointVal(env, entrypoint, "args");
	va_list ap;
	va_start(ap, context);
	al_obj arg;
	if(cal_obj_getType(env, fn) != AL_OBJ_TYPE_ALIST ||
	   (cal_obj_getType(env, fn) != AL_OBJ_TYPE_ALIST &&
	    cal_obj_getType(env, al_alist_simpleLookup(env, fn, sym_lambda)) == AL_OBJ_TYPE_NULL)){
		al_obj args = cal_alist_alloc(env, 0);
		int i = 0;
		while(cal_obj_getType(env, (arg = va_arg(ap, al_obj))) != AL_OBJ_TYPE_NULL){
			switch(cal_obj_getType(env, arg)){
			case AL_OBJ_TYPE_ATOM:
			case AL_OBJ_TYPE_ALIST:
				al_alist_append_m(env, args, cal_list_alloc(env, 2, cal_atom_int32(env, i), arg));
				break;
			case AL_OBJ_TYPE_LIST:
				al_list_prepend_m(env, arg, cal_atom_int32(env, i));
				break;
			}
			i++;
		}
		return args;
	}else if(cal_obj_getType(env, fn) == AL_OBJ_TYPE_ALIST){
		al_obj arglist = al_alist_simpleLookup(env, fn, sym_args);
		al_obj args = cal_alist_alloc(env, 0);
		if(cal_obj_getType(env, arglist) == AL_OBJ_TYPE_LIST){
			al_obj argnames = cal_list_nth(env, arglist, 1);
			if(cal_list_length(env, arglist) == 2 && cal_obj_getType(env, argnames) == AL_OBJ_TYPE_ALIST){
				int i;
				for(i = 0; i < cal_alist_length(env, argnames) && cal_obj_getType(env, (arg = va_arg(ap, al_obj))) != AL_OBJ_TYPE_NULL; i++){
					al_alist_append_m(env, args, cal_list_alloc(env, 2, cal_list_nth(env, cal_alist_nth(env, argnames, i), 0), arg));
				}
				// process any remaining args that didn't have a name in the arg list in the lambda
				while(cal_obj_getType(env, (arg = va_arg(ap, al_obj))) != AL_OBJ_TYPE_NULL){
					i++;
					al_alist_append_m(env, args, cal_list_alloc(env, 2, cal_atom_int32(env, i), arg));
				}
			}else if(cal_obj_getType(env, arglist) != AL_OBJ_TYPE_NULL){
				int i;
				for(i = 0; i < cal_list_length(env, arglist) && cal_obj_getType(env, (arg = va_arg(ap, al_obj))) != AL_OBJ_TYPE_NULL; i++){
					al_alist_append_m(env, args, cal_list_alloc(env, 2, cal_list_nth(env, arglist, i + 1), arg));
				}
				// process any remaining args that didn't have a name in the arg list in the lambda
				while(cal_obj_getType(env, (arg = va_arg(ap, al_obj))) != AL_OBJ_TYPE_NULL){
					i++;
					al_alist_append_m(env, args, cal_list_alloc(env, 2, cal_atom_int32(env, i), arg));
				}
			}
		}
	}
	return AL_OBJ_NULL;
}

al_obj al_alist_makeApplication(al_env env, al_obj fn, al_obj applicator, al_obj arg)
{
	al_obj lapplication = cal_list_alloc(env, 2, cal_atom_symbol(env, "@"), applicator);
	al_obj sym_fn = cal_atom_symbol(env, "fn");
	al_obj sym_arg = cal_atom_symbol(env, "arg");
	al_obj lfn;
	if(cal_obj_getType(env, fn) == AL_OBJ_TYPE_NULL){
		lfn = cal_list_alloc(env, 1, sym_fn);
	}else if(cal_obj_getType(env, fn) == AL_OBJ_TYPE_LIST){
		lfn = al_list_prepend(env, fn, sym_fn);
	}else{
		lfn = cal_list_alloc(env, 2, sym_fn, fn);
	}
	al_obj larg;
	if(cal_obj_getType(env, arg) == AL_OBJ_TYPE_LIST){
		larg = al_list_prepend(env, arg, sym_arg);
	}else{
		larg = cal_list_alloc(env, 2, sym_arg, arg);
	}
	return cal_alist_alloc(env, 3, lapplication, lfn, larg);
}

int cal_alist_isApplication(al_env env, al_obj al)
{
	if(cal_obj_getType(env, al) == AL_OBJ_TYPE_ALIST){
		if(cal_obj_getType(env, cal_alist_simpleLookup(env, al, "@")) != AL_OBJ_TYPE_NULL){
			return 1;
		}
	}
	return 0;
}

al_obj al_alist_eval(al_env env, al_obj al, al_obj context)
{
	if(cal_obj_getType(env, al) != AL_OBJ_TYPE_ALIST){
		return al_obj_eval(env, al, context);
	}
	al_obj apply_list = cal_alist_simpleLookup(env, al, "@");
	al_obj lambda_list = cal_alist_simpleLookup(env, al, "lambda");
	al_obj lazy_applicator = cal_atom_symbol(env, "@");
	al_obj left_eager_applicator = cal_atom_symbol(env, "!@");
	al_obj right_eager_applicator = cal_atom_symbol(env, "@!");
	al_obj eager_applicator = cal_atom_symbol(env, "!@!");
	if(cal_obj_getType(env, apply_list) == AL_OBJ_TYPE_LIST){
		al_obj applicator = cal_list_nth(env, apply_list, 1);
		al_obj lhs = cal_alist_simpleLookup(env, al, "fn");
		al_obj rhs = cal_alist_simpleLookup(env, al, "arg");

		al_obj ll = AL_OBJ_NULL;
		al_obj rr = AL_OBJ_NULL;
		int ex = 0;
		if(cal_atom_eql(env, applicator, lazy_applicator)){
			;
		}else if(cal_atom_eql(env, applicator, left_eager_applicator)){
			ex |= 1;			
		}else if(cal_atom_eql(env, applicator, right_eager_applicator)){
			ex |= 2;
		}else if(cal_atom_eql(env, applicator, eager_applicator)){
			ex = 3;
		}else{ // user-defined applicator
			
		}
		////////////////////////////////////////////////////////////////////////////////////////////////////
		// can't call al_list_eval because it reduces if there's a value message.
		// need to know if there is or isn't one to determine whether we continue with application
		////////////////////////////////////////////////////////////////////////////////////////////////////
		int ev = 3;
		if(ex & 1 && cal_obj_getType(env, lhs) == AL_OBJ_TYPE_LIST){// && cal_list_length(env, lhs) >= 2){
			ll = cal_list_alloc(env, 0);
			for(int i = 0; i < cal_list_length(env, lhs); i++){
				al_obj e = al_obj_eval(env, cal_list_nth(env, lhs, i), context);
				if(cal_obj_getType(env, e) == AL_OBJ_TYPE_ALIST){
					al_obj v = cal_alist_simpleLookup(env, e, "value");
					if(cal_obj_getType(env, v) == AL_OBJ_TYPE_LIST){
						for(int j = 1; j < cal_list_length(env, v); j++){
							al_list_append_m(env, ll, cal_list_nth(env, v, j));
						}
					}else{
						if(cal_alist_isApplication(env, e)){
							ev &= 0x2;
						}
						al_list_append_m(env, ll, e);
					}
				}else{
					al_list_append_m(env, ll, e);
				}
			}
		}else{
			ll = lhs;
		}
		if(ex & 2 && cal_obj_getType(env, rhs) == AL_OBJ_TYPE_LIST){// && cal_list_length(env, rhs) >= 2){
			rr = cal_list_alloc(env, 0);
			for(int i = 0; i < cal_list_length(env, rhs); i++){
				al_obj e = al_obj_eval(env, cal_list_nth(env, rhs, i), context);
				if(cal_obj_getType(env, e) == AL_OBJ_TYPE_ALIST){
					al_obj v = cal_alist_simpleLookup(env, e, "value");
					if(cal_obj_getType(env, v) == AL_OBJ_TYPE_LIST){
						for(int j = 1; j < cal_list_length(env, v); j++){
							al_list_append_m(env, rr, cal_list_nth(env, v, j));
						}
					}else{
						if(cal_alist_isApplication(env, e)){
							ev &= 0x2;
						}
						al_list_append_m(env, rr, e);
					}
				}else{
					al_list_append_m(env, rr, e);
				}
			}
		}else{
			rr = rhs;
		}
		if(ev == 3){
			al_obj ret = al_alist_apply(env, ll, applicator, rr, context);
			if(cal_obj_getType(env, ret) == AL_OBJ_TYPE_NULL){
				return cal_alist_alloc(env, 1, cal_list_alloc(env, 2, cal_atom_symbol(env, "value"), cal_alist_alloc(env, 2, cal_list_alloc(env, 1, cal_atom_symbol(env, "lambda")), cal_list_alloc(env, 2, cal_atom_symbol(env, "expr"), al))));
				//return al;
			}else{
				return ret;
			}
		}else{
			return cal_alist_alloc(env, 3, apply_list, ll, rr);
		}
	}else if(cal_obj_getType(env, lambda_list) == AL_OBJ_TYPE_LIST){
		return al;
	}else{
		al_obj out = cal_alist_alloc(env, 0);
		for(int i = 0; i < cal_alist_length(env, al); i++){
			al_alist_append_m(env, out, al_list_eval(env, cal_alist_nth(env, al, i), al_alist_union(env, al, al_alist_addThisToContext(env, al, context))));
		}
		return out;
	}
}

/*
al_obj al_alist_eval(al_env env, al_obj al, al_obj context)
{
	if(cal_obj_getType(env, al) != AL_OBJ_TYPE_ALIST){
		return al_obj_eval(env, al, context);
	}
	al_obj entrypointmsg = cal_alist_simpleLookup(env, al, "entrypoint");
	al_obj entrypoint;
	if(cal_obj_getType(env, entrypointmsg) != AL_OBJ_TYPE_NULL){
		entrypoint = cal_list_nth(env, entrypointmsg, 1);
		context = al_alist_union(env, cal_alist_alloc(env, 1, entrypointmsg), context);
	}else{
		entrypointmsg = cal_alist_simpleLookup(env, al, "entrypoint");
		if(cal_obj_getType(env, entrypointmsg) != AL_OBJ_TYPE_NULL){
			entrypoint = cal_list_nth(env, entrypointmsg, 1);
		}else{
			entrypoint = AL_OBJ_NULL;
		}
	}
	if(cal_obj_getType(env, entrypoint) == AL_OBJ_TYPE_NULL){
		return al_alist_builtinEval(env, al, context);
	}	
	al_obj application = cal_alist_getEntryPointVal(env, entrypoint, "application");
	al_obj lazy_applicator = cal_alist_getEntryPointVal(env, entrypoint, "lazy_applicator");
	al_obj left_eager_applicator = cal_alist_getEntryPointVal(env, entrypoint, "left_eager_applicator");
	al_obj right_eager_applicator = cal_alist_getEntryPointVal(env, entrypoint, "right_eager_applicator");
	al_obj eager_applicator = cal_alist_getEntryPointVal(env, entrypoint, "eager_applicator");
	if(cal_obj_getType(env, application) == AL_OBJ_TYPE_NULL){
		application = cal_atom_symbol(env, "@");
	}
	if(cal_obj_getType(env, lazy_applicator) == AL_OBJ_TYPE_NULL){
		lazy_applicator = cal_atom_symbol(env, "@");
	}
	if(cal_obj_getType(env, left_eager_applicator) == AL_OBJ_TYPE_NULL){
		left_eager_applicator = cal_atom_symbol(env, "!@");
	}
	if(cal_obj_getType(env, right_eager_applicator) == AL_OBJ_TYPE_NULL){
		right_eager_applicator = cal_atom_symbol(env, "@!");
	}
	if(cal_obj_getType(env, eager_applicator) == AL_OBJ_TYPE_NULL){
		eager_applicator = cal_atom_symbol(env, "!@!");
	}
	
	al_obj lambda = cal_alist_getEntryPointVal(env, entrypoint, "lambda");
	if(cal_obj_getType(env, lambda) == AL_OBJ_TYPE_NULL){
		lambda = cal_atom_symbol(env, "\\");
	}

	al_obj apply_list = al_alist_simpleLookup(env, al, application);
	al_obj lambda_list = al_alist_simpleLookup(env, al, lambda);

	if(cal_obj_getType(env, apply_list) == AL_OBJ_TYPE_LIST){
		al_obj applicator = cal_list_nth(env, apply_list, 1);
		al_obj left_arg = cal_alist_getEntryPointVal(env, entrypoint, "left_arg");
		al_obj right_arg = cal_alist_getEntryPointVal(env, entrypoint, "right_arg");
		if(cal_obj_getType(env, left_arg) == AL_OBJ_TYPE_NULL){
			left_arg = cal_atom_symbol(env, "lhs");
		}
		if(cal_obj_getType(env, right_arg) == AL_OBJ_TYPE_NULL){
			right_arg = cal_atom_symbol(env, "rhs");
		}
		al_obj lhs = al_alist_simpleLookup(env, al, left_arg);
		al_obj rhs = al_alist_simpleLookup(env, al, right_arg);

		al_obj ll = lhs;
		al_obj rr = rhs;
		int ex = 0;
		if(cal_atom_eql(env, applicator, lazy_applicator)){
		}else if(cal_atom_eql(env, applicator, left_eager_applicator)){
			ex |= 1;			
		}else if(cal_atom_eql(env, applicator, right_eager_applicator)){
			ex |= 2;
		}else if(cal_atom_eql(env, applicator, eager_applicator)){
			ex = 3;
		}else{ // user-defined applicator
			
		}
		if(ex & 1 && cal_obj_getType(env, ll) == AL_OBJ_TYPE_LIST && cal_list_length(env, ll) >= 2){
			//ll = al_list_eval(env, ll, context);
		}
		if(ex & 2 && cal_obj_getType(env, rr) == AL_OBJ_TYPE_LIST && cal_list_length(env, rr) >= 2){
			//rr = al_list_eval(env, rr, context);
		}
		al_obj applyfnsym = cal_alist_getEntryPointVal(env, entrypoint, "applyfn");
		if(cal_obj_getType(env, applyfnsym) == AL_OBJ_TYPE_NULL){
			return al_alist_apply(env, ll, applicator, rr, context);
		}else{

		}

	}else if(cal_obj_getType(env, lambda_list) == AL_OBJ_TYPE_LIST){
		return al;
	}else{
		al_obj out = cal_alist_alloc(env, 0);
		for(int i = 0; i < cal_alist_length(env, al); i++){
			al_obj thismsg = cal_alist_simpleLookup(env, context, "this");
			al_obj parent;
			if(cal_obj_getType(env, thismsg) == AL_OBJ_TYPE_LIST && cal_list_length(env, thismsg) > 1){
				parent = cal_list_alloc(env, 2, cal_atom_symbol(env, "parent"), cal_list_nth(env, thismsg, 1));
			}else{
				parent = cal_list_alloc(env, 1, cal_atom_symbol(env, "parent"));
			}
			al_obj this = cal_list_alloc(env, 2, cal_atom_symbol(env, "this"), al);
			context = al_alist_union(env, cal_alist_alloc(env, 2, this, parent), context);
			//al_alist_append_m(env, out, al_list_eval(env, cal_alist_nth(env, al, i), context));
		}
		return out;
	}
}
*/

//////////////////////////////////////////////////
// C API
//////////////////////////////////////////////////

al_obj cal_alist_alloc(al_env env, int n, ...)
{
	al_pv2 *l = al_pv2_alloc(al_env_getRegion(env), NULL);
	va_list ap;
	va_start(ap, n);
	for(int i = 0; i < n; i++){
		al_obj li = va_arg(ap, al_obj);
		if(cal_obj_getType(env, li) != AL_OBJ_TYPE_LIST){
			printf("%s: only accepts lists, but got a %s\n", __func__, cal_obj_getTypeName(env, li));
			return AL_OBJ_NULL;
		}
		al_obj *copy = (al_obj *)al_env_getBytes(env, sizeof(al_obj));
		*copy = li;
		al_pv2_append_m(al_env_getRegion(env), l, (void *)copy);
	}
	va_end(ap);
	return cal_obj_alloc(env, l, AL_OBJ_TYPE_ALIST);
}

int cal_alist_isIndexable(al_env env, al_obj al)
{
	if(cal_obj_getType(env, al) == AL_OBJ_TYPE_ALIST){
		return 1;
	}else{
		return cal_obj_isIndexable(env, al);
	}
}

al_obj cal_alist_nth(al_env env, al_obj al, int n)
{
	if(cal_obj_getType(env, al) == AL_OBJ_TYPE_ALIST){
		al_obj *o = (al_obj *)al_pv2_nth((al_pv2 *)(cal_obj_getObj(env, al)), n);
		if(o && cal_obj_getType(env, *o) == AL_OBJ_TYPE_LIST){
			return *o;
		}
	}
	return cal_obj_alloc(env, NULL, 0);
}



int cal_alist_length(al_env env, al_obj al)
{
	if(cal_obj_getType(env, al) == AL_OBJ_TYPE_ALIST){
		return al_pv2_length((al_pv2 *)(cal_obj_getObj(env, al)));
	}
	return 0;
}

al_obj cal_alist_append(al_env env, al_obj al, al_obj l)
{
	if(cal_obj_getType(env, al) != AL_OBJ_TYPE_ALIST){
		return al;
	}
	al_obj *copy = (al_obj *)al_env_getBytes(env, sizeof(al_obj));
	*copy = l;
	return cal_obj_alloc(env, al_pv2_append(al_env_getRegion(env), (al_pv2 *)(cal_obj_getObj(env, al)), (void *)copy), AL_OBJ_TYPE_ALIST);
}

al_obj cal_alist_append_m(al_env env, al_obj al, al_obj l)
{
	if(cal_obj_getType(env, al) != AL_OBJ_TYPE_ALIST){
		return al;
	}
	al_obj *copy = (al_obj *)al_env_getBytes(env, sizeof(al_obj));
	*copy = l;
	return cal_obj_alloc(env, al_pv2_append_m(al_env_getRegion(env), (al_pv2 *)(cal_obj_getObj(env, al)), (void *)copy), AL_OBJ_TYPE_ALIST);
}

int cal_alist_eql(al_env env, al_obj al1, al_obj al2)
{
	if(cal_obj_getType(env, al1) != cal_obj_getType(env, al2)){
		return 0;
	}
	if(cal_obj_getType(env, al1) != AL_OBJ_TYPE_ALIST || cal_obj_getType(env, al2) != AL_OBJ_TYPE_ALIST){
		return cal_obj_eql(env, al1, al2);
	}
	int al1len = cal_alist_length(env, al1);
	if(al1len != cal_alist_length(env, al2)){
		return 0;
	}
	for(int i = 0; i < al1len; i++){
		if(!cal_list_eql(env, cal_alist_nth(env, al1, i), cal_alist_nth(env, al2, i))){
			return 0;
		}
	}
	return 1;
}

int cal_alist_eqv(al_env env, al_obj al1, al_obj al2)
{
	if(cal_obj_getType(env, al1) != cal_obj_getType(env, al2)){
		return 0;
	}
	if(cal_obj_getType(env, al1) != AL_OBJ_TYPE_ALIST || cal_obj_getType(env, al2) != AL_OBJ_TYPE_ALIST){
		return cal_obj_eql(env, al1, al2);
	}
	int al1len = cal_alist_length(env, al1);
	if(al1len != cal_alist_length(env, al2)){
		return 0;
	}
	for(int i = 0; i < al1len; i++){
		if(!cal_list_eqv(env, cal_alist_nth(env, al1, i), cal_alist_nth(env, al2, i))){
			return 0;
		}
	}
	return 1;
}

al_obj cal_alist_simpleLookup(al_env env, al_obj al, char *name)
{
	// this is stupid 
	if(cal_obj_getType(env, al) == AL_OBJ_TYPE_ALIST && name){
		al_obj namestr = cal_atom_string(env, name);
		for(int i = 0; i < cal_alist_length(env, al); i++){
			al_obj l = cal_alist_nth(env, al, i);
			al_obj a = cal_list_nth(env, l, 0);
			al_c_int32 res = cal_atom_strcmp(env, namestr, a);
			if(!al_c_error(res)){
				if(al_c_value(res) == 0){
					return l;
				}
			}
		}
	}
	return AL_OBJ_NULL;
}

int cal_alist_print(al_env env, al_obj al)
{
	if(cal_obj_getType(env, al) == AL_OBJ_TYPE_ALIST){
		int n = 0;
		n += printf("{");
		for(int i = 0; i < cal_alist_length(env, al) - 1; i++){
			n += cal_obj_println(env, cal_alist_nth(env, al, i));
		}
		n += cal_obj_print(env, cal_alist_nth(env, al, cal_alist_length(env, al) - 1));
		n += printf("}");
		return n;
	}
	return 0;
}

int cal_alist_println(al_env env, al_obj al)
{
	return cal_alist_print(env, al) + printf("\n");
}
