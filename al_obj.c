#include "al_obj.h"
#include "al_env.h"
#include "al_atom.h"
#include "al_list.h"
#include "al_alist.h"

char *cal_obj_typenames[] = {"NULL", "atom", "list", NULL, "alist"};

al_obj al_obj_copy(al_env env, al_obj b)
{
	switch(cal_obj_getType(env, b)){
	case AL_OBJ_TYPE_ATOM:
		return al_atom_copy(env, b);
	case AL_OBJ_TYPE_LIST:
		return al_list_copy(env, b);
	case AL_OBJ_TYPE_ALIST:
		return al_alist_copy(env, b);
	}
	return AL_OBJ_NULL;
}

al_obj al_obj_isIndexable(al_env env, al_obj b)
{
	switch(cal_obj_getType(env, b)){
	case AL_OBJ_TYPE_ATOM:
		return al_atom_isIndexable(env, b);
	case AL_OBJ_TYPE_LIST:
		return al_list_isIndexable(env, b);
	case AL_OBJ_TYPE_ALIST:
		return al_alist_isIndexable(env, b);
	}
	return AL_OBJ_NULL;
}

al_obj al_obj_nth(al_env env, al_obj b, al_obj n)
{
	switch(cal_obj_getType(env, b)){
	case AL_OBJ_TYPE_ATOM:
		return al_atom_nth(env, b, n);
	case AL_OBJ_TYPE_LIST:
		return al_list_nth(env, b, n);
	case AL_OBJ_TYPE_ALIST:
		return al_alist_nth(env, b, n);
	}
	return AL_OBJ_NULL;
}

al_obj al_obj_rest(al_env env, al_obj b)
{
	switch(cal_obj_getType(env, b)){
	case AL_OBJ_TYPE_ATOM:
		return al_atom_rest(env, b);
	case AL_OBJ_TYPE_LIST:
		return al_list_rest(env, b);
	case AL_OBJ_TYPE_ALIST:
		return AL_OBJ_NULL;
	}
	return AL_OBJ_NULL;
}

al_obj al_obj_length(al_env env, al_obj b)
{
	switch(cal_obj_getType(env, b)){
	case AL_OBJ_TYPE_ATOM:
		return al_atom_length(env, b);
	case AL_OBJ_TYPE_LIST:
		return al_list_length(env, b);
	case AL_OBJ_TYPE_ALIST:
		return al_alist_length(env, b);
	}
	return AL_OBJ_NULL;
}

al_obj al_obj_eql(al_env env, al_obj b1, al_obj b2)
{
	if(cal_obj_getType(env, b1) != cal_obj_getType(env, b2)){
		return cal_atom_false(env);
	}
	switch(cal_obj_getType(env, b1)){
	case AL_OBJ_TYPE_ATOM:
		return al_atom_eql(env, b1, b2);
	case AL_OBJ_TYPE_LIST:
		return al_list_eql(env, b1, b2);
	case AL_OBJ_TYPE_ALIST:
		return al_alist_eql(env, b1, b2);
	}
	return AL_OBJ_NULL;
}

al_obj al_obj_eqv(al_env env, al_obj b1, al_obj b2)
{
	if(cal_obj_getType(env, b1) != cal_obj_getType(env, b2)){
		return cal_atom_false(env);
	}
	switch(cal_obj_getType(env, b1)){
	case AL_OBJ_TYPE_ATOM:
		return al_atom_eqv(env, b1, b2);
	case AL_OBJ_TYPE_LIST:
		return al_list_eqv(env, b1, b2);
	case AL_OBJ_TYPE_ALIST:
		return al_alist_eqv(env, b1, b2);
	}
	return AL_OBJ_NULL;
}

al_obj al_obj_eval(al_env env, al_obj o, al_obj context)
{
	switch(cal_obj_getType(env, o)){
	case AL_OBJ_TYPE_ATOM:
		return o;
	case AL_OBJ_TYPE_LIST:
		return al_list_eval(env, o, context);
	case AL_OBJ_TYPE_ALIST:
		return al_alist_eval(env, o, context);
	}
	return AL_OBJ_NULL;
}

//////////////////////////////////////////////////
// C API
//////////////////////////////////////////////////
/*
al_obj cal_obj_alloc(al_env env, void *obj, char type)
{
	return (al_obj){obj, type};
}

void *cal_obj_getObj(al_env env, al_obj obj)
{
	return obj.obj;
}

char cal_obj_getType(al_env env, al_obj obj)
{
	return obj.type;
}
*/
char *cal_obj_getTypeName(al_env env, al_obj obj)
{
	int obj_type = cal_obj_getType(env, obj);
	if(obj_type < sizeof(cal_obj_typenames) / sizeof(char*)){
		return cal_obj_typenames[obj_type];
	}
	return "";
}

int cal_obj_isIndexable(al_env env, al_obj b)
{
	switch(cal_obj_getType(env, b)){
	case AL_OBJ_TYPE_ATOM:
		return cal_atom_isIndexable(env, b);
	case AL_OBJ_TYPE_LIST:
		return cal_list_isIndexable(env, b);
	case AL_OBJ_TYPE_ALIST:
		return cal_alist_isIndexable(env, b);
	}
	return 0;
}

al_obj cal_obj_nth(al_env env, al_obj b, int n)
{
	switch(cal_obj_getType(env, b)){
	case AL_OBJ_TYPE_ATOM:
		// use al, not cal because cal returns a char
		return al_atom_nth(env, b, cal_atom_int32(env, n));
	case AL_OBJ_TYPE_LIST:
		return cal_list_nth(env, b, n);
	case AL_OBJ_TYPE_ALIST:
		return cal_alist_nth(env, b, n);
	}
	return AL_OBJ_NULL;
}

int cal_obj_length(al_env env, al_obj b)
{
	switch(cal_obj_getType(env, b)){
	case AL_OBJ_TYPE_ATOM:
		return cal_atom_length(env, b);
	case AL_OBJ_TYPE_LIST:
		return cal_list_length(env, b);
	case AL_OBJ_TYPE_ALIST:
		return cal_alist_length(env, b);
	}
	return 0;
}

int cal_obj_eql(al_env env, al_obj b1, al_obj b2)
{
	if(cal_obj_getType(env, b1) != cal_obj_getType(env, b2)){
		return 0;
	}
	switch(cal_obj_getType(env, b1)){
	case AL_OBJ_TYPE_ATOM:
		return cal_atom_eql(env, b1, b2);
	case AL_OBJ_TYPE_LIST:
		return cal_list_eql(env, b1, b2);
	case AL_OBJ_TYPE_ALIST:
		return cal_alist_eql(env, b1, b2);
	}
	return 0;
}

int cal_obj_eqv(al_env env, al_obj b1, al_obj b2)
{
	if(cal_obj_getType(env, b1) != cal_obj_getType(env, b2)){
		return 0;
	}
	switch(cal_obj_getType(env, b1)){
	case AL_OBJ_TYPE_ATOM:
		return cal_atom_eqv(env, b1, b2);
	case AL_OBJ_TYPE_LIST:
		return cal_list_eqv(env, b1, b2);
	case AL_OBJ_TYPE_ALIST:
		return cal_alist_eqv(env, b1, b2);
	}
	return 0;
}

int cal_obj_print(al_env env, al_obj o)
{
	switch(cal_obj_getType(env, o)){
	case AL_OBJ_TYPE_ATOM:
		return cal_atom_print(env, o);
	case AL_OBJ_TYPE_LIST:
		return cal_list_print(env, o);
	case AL_OBJ_TYPE_ALIST:
		return cal_alist_print(env, o);
	case AL_OBJ_TYPE_NULL:
		return printf("(NULL)");
	}
	return 0;
}

int cal_obj_println(al_env env, al_obj o)
{
	switch(cal_obj_getType(env, o)){
	case AL_OBJ_TYPE_ATOM:
		return cal_atom_println(env, o);
	case AL_OBJ_TYPE_LIST:
		return cal_list_println(env, o);
	case AL_OBJ_TYPE_ALIST:
		return cal_alist_println(env, o);
	case AL_OBJ_TYPE_NULL:
		return printf("(NULL)\n");
	}
	return 0;
}
