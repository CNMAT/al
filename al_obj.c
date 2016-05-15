#include "al_obj.h"
#include "al_atom.h"
#include "al_list.h"
#include "al_alist.h"

char *cal_obj_typenames[] = {"NULL", "atom", "list", NULL, "alist"};

al_obj al_obj_copy(al_region r, al_obj b)
{
	switch(cal_obj_getType(r, b)){
	case AL_OBJ_TYPE_ATOM:
		return al_atom_copy(r, b);
	case AL_OBJ_TYPE_LIST:
		return al_list_copy(r, b);
	case AL_OBJ_TYPE_ALIST:
		return al_alist_copy(r, b);
	}
	return AL_OBJ_NULL;
}

al_obj al_obj_isIndexable(al_region r, al_obj b)
{
	switch(cal_obj_getType(r, b)){
	case AL_OBJ_TYPE_ATOM:
		return al_atom_isIndexable(r, b);
	case AL_OBJ_TYPE_LIST:
		return al_list_isIndexable(r, b);
	case AL_OBJ_TYPE_ALIST:
		return al_alist_isIndexable(r, b);
	}
	return AL_OBJ_NULL;
}

al_obj al_obj_nth(al_region r, al_obj b, al_obj n)
{
	switch(cal_obj_getType(r, b)){
	case AL_OBJ_TYPE_ATOM:
		return al_atom_nth(r, b, n);
	case AL_OBJ_TYPE_LIST:
		return al_list_nth(r, b, n);
	case AL_OBJ_TYPE_ALIST:
		return al_alist_nth(r, b, n);
	}
	return AL_OBJ_NULL;
}

al_obj al_obj_rest(al_region r, al_obj b)
{
	switch(cal_obj_getType(r, b)){
	case AL_OBJ_TYPE_ATOM:
		return al_atom_rest(r, b);
	case AL_OBJ_TYPE_LIST:
		return al_list_rest(r, b);
	case AL_OBJ_TYPE_ALIST:
		return AL_OBJ_NULL;
	}
	return AL_OBJ_NULL;
}

al_obj al_obj_length(al_region r, al_obj b)
{
	switch(cal_obj_getType(r, b)){
	case AL_OBJ_TYPE_ATOM:
		return al_atom_length(r, b);
	case AL_OBJ_TYPE_LIST:
		return al_list_length(r, b);
	case AL_OBJ_TYPE_ALIST:
		return al_alist_length(r, b);
	}
	return AL_OBJ_NULL;
}

al_obj al_obj_eql(al_region r, al_obj b1, al_obj b2)
{
	if(cal_obj_getType(r, b1) != cal_obj_getType(r, b2)){
		return cal_atom_false(r);
	}
	switch(cal_obj_getType(r, b1)){
	case AL_OBJ_TYPE_ATOM:
		return al_atom_eql(r, b1, b2);
	case AL_OBJ_TYPE_LIST:
		return al_list_eql(r, b1, b2);
	case AL_OBJ_TYPE_ALIST:
		return al_alist_eql(r, b1, b2);
	}
	return AL_OBJ_NULL;
}

al_obj al_obj_eqv(al_region r, al_obj b1, al_obj b2)
{
	if(cal_obj_getType(r, b1) != cal_obj_getType(r, b2)){
		return cal_atom_false(r);
	}
	switch(cal_obj_getType(r, b1)){
	case AL_OBJ_TYPE_ATOM:
		return al_atom_eqv(r, b1, b2);
	case AL_OBJ_TYPE_LIST:
		return al_list_eqv(r, b1, b2);
	case AL_OBJ_TYPE_ALIST:
		return al_alist_eqv(r, b1, b2);
	}
	return AL_OBJ_NULL;
}

al_obj al_obj_eval(al_region r, al_obj o, al_obj context)
{
	switch(cal_obj_getType(r, o)){
	case AL_OBJ_TYPE_ATOM:
		return o;
	case AL_OBJ_TYPE_LIST:
		return al_list_eval(r, o, context);
	case AL_OBJ_TYPE_ALIST:
		return al_alist_eval(r, o, context);
	}
	return AL_OBJ_NULL;
}

//////////////////////////////////////////////////
// C API
//////////////////////////////////////////////////
/*
al_obj cal_obj_alloc(al_region r, void *obj, char type)
{
	return (al_obj){obj, type};
}

void *cal_obj_getObj(al_region r, al_obj obj)
{
	return obj.obj;
}

char cal_obj_getType(al_region r, al_obj obj)
{
	return obj.type;
}
*/
char *cal_obj_getTypeName(al_region r, al_obj obj)
{
	int obj_type = cal_obj_getType(r, obj);
	if(obj_type < sizeof(cal_obj_typenames) / sizeof(char*)){
		return cal_obj_typenames[obj_type];
	}
	return "";
}

int cal_obj_isIndexable(al_region r, al_obj b)
{
	switch(cal_obj_getType(r, b)){
	case AL_OBJ_TYPE_ATOM:
		return cal_atom_isIndexable(r, b);
	case AL_OBJ_TYPE_LIST:
		return cal_list_isIndexable(r, b);
	case AL_OBJ_TYPE_ALIST:
		return cal_alist_isIndexable(r, b);
	}
	return 0;
}

al_obj cal_obj_nth(al_region r, al_obj b, int n)
{
	switch(cal_obj_getType(r, b)){
	case AL_OBJ_TYPE_ATOM:
		// use al, not cal because cal returns a char
		return al_atom_nth(r, b, cal_atom_int32(r, n));
	case AL_OBJ_TYPE_LIST:
		return cal_list_nth(r, b, n);
	case AL_OBJ_TYPE_ALIST:
		return cal_alist_nth(r, b, n);
	}
	return AL_OBJ_NULL;
}

int cal_obj_length(al_region r, al_obj b)
{
	switch(cal_obj_getType(r, b)){
	case AL_OBJ_TYPE_ATOM:
		return cal_atom_length(r, b);
	case AL_OBJ_TYPE_LIST:
		return cal_list_length(r, b);
	case AL_OBJ_TYPE_ALIST:
		return cal_alist_length(r, b);
	}
	return 0;
}

int cal_obj_eql(al_region r, al_obj b1, al_obj b2)
{
	if(cal_obj_getType(r, b1) != cal_obj_getType(r, b2)){
		return 0;
	}
	switch(cal_obj_getType(r, b1)){
	case AL_OBJ_TYPE_ATOM:
		return cal_atom_eql(r, b1, b2);
	case AL_OBJ_TYPE_LIST:
		return cal_list_eql(r, b1, b2);
	case AL_OBJ_TYPE_ALIST:
		return cal_alist_eql(r, b1, b2);
	}
	return 0;
}

int cal_obj_eqv(al_region r, al_obj b1, al_obj b2)
{
	if(cal_obj_getType(r, b1) != cal_obj_getType(r, b2)){
		return 0;
	}
	switch(cal_obj_getType(r, b1)){
	case AL_OBJ_TYPE_ATOM:
		return cal_atom_eqv(r, b1, b2);
	case AL_OBJ_TYPE_LIST:
		return cal_list_eqv(r, b1, b2);
	case AL_OBJ_TYPE_ALIST:
		return cal_alist_eqv(r, b1, b2);
	}
	return 0;
}

int cal_obj_print(al_region r, al_obj o)
{
	switch(cal_obj_getType(r, o)){
	case AL_OBJ_TYPE_ATOM:
		return cal_atom_print(r, o);
	case AL_OBJ_TYPE_LIST:
		return cal_list_print(r, o);
	case AL_OBJ_TYPE_ALIST:
		return cal_alist_print(r, o);
	case AL_OBJ_TYPE_NULL:
		return printf("(NULL)");
	}
	return 0;
}

int cal_obj_println(al_region r, al_obj o)
{
	switch(cal_obj_getType(r, o)){
	case AL_OBJ_TYPE_ATOM:
		return cal_atom_println(r, o);
	case AL_OBJ_TYPE_LIST:
		return cal_list_println(r, o);
	case AL_OBJ_TYPE_ALIST:
		return cal_alist_println(r, o);
	case AL_OBJ_TYPE_NULL:
		return printf("(NULL)\n");
	}
	return 0;
}
