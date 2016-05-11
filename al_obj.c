#include "al_obj.h"
#include "al_atom.h"
#include "al_list.h"

al_obj al_obj_copy(al_region r, al_obj b)
{
	switch(cal_obj_getType(r, b)){
	case AL_OBJ_ATOM:
		return al_atom_copy(r, b);
	case AL_OBJ_LIST:
		return al_list_copy(r, b);
	case AL_OBJ_ALIST:
		return (al_obj){NULL, 0};
	}
	return AL_OBJ_NULL;
}

al_obj al_obj_isIndexable(al_region r, al_obj b)
{
	switch(cal_obj_getType(r, b)){
	case AL_OBJ_ATOM:
		return al_atom_isIndexable(r, b);
	case AL_OBJ_LIST:
		return al_list_isIndexable(r, b);
	case AL_OBJ_ALIST:
		return (al_obj){NULL, 0};
	}
	return AL_OBJ_NULL;
}

al_obj al_obj_nth(al_region r, al_obj b, al_obj n)
{
	switch(cal_obj_getType(r, b)){
	case AL_OBJ_ATOM:
		return al_atom_nth(r, b, n);
	case AL_OBJ_LIST:
		return al_list_nth(r, b, n);
	case AL_OBJ_ALIST:
		return (al_obj){NULL, 0};
	}
	return AL_OBJ_NULL;
}

al_obj al_obj_rest(al_region r, al_obj b)
{
	switch(cal_obj_getType(r, b)){
	case AL_OBJ_ATOM:
		return al_atom_rest(r, b);
	case AL_OBJ_LIST:
		return al_list_rest(r, b);
	case AL_OBJ_ALIST:
		return (al_obj){NULL, 0};
	}
	return AL_OBJ_NULL;
}

al_obj al_obj_length(al_region r, al_obj b)
{
	switch(cal_obj_getType(r, b)){
	case AL_OBJ_ATOM:
		return al_atom_length(r, b);
	case AL_OBJ_LIST:
		return al_list_length(r, b);
	case AL_OBJ_ALIST:
		return (al_obj){NULL, 0};
	}
	return AL_OBJ_NULL;
}

al_obj al_obj_eql(al_region r, al_obj b1, al_obj b2)
{
	if(cal_obj_getType(r, b1) != cal_obj_getType(r, b2)){
		return cal_atom_false(r);
	}
	switch(cal_obj_getType(r, b1)){
	case AL_OBJ_ATOM:
		return al_atom_eql(r, b1, b2);
	case AL_OBJ_LIST:
		return al_list_eql(r, b1, b2);
	case AL_OBJ_ALIST:
		return (al_obj){NULL, 0};
	}
	return AL_OBJ_NULL;
}

al_obj al_obj_eqv(al_region r, al_obj b1, al_obj b2)
{
	if(cal_obj_getType(r, b1) != cal_obj_getType(r, b2)){
		return cal_atom_false(r);
	}
	switch(cal_obj_getType(r, b1)){
	case AL_OBJ_ATOM:
		return al_atom_eql(r, b1, b2);
	case AL_OBJ_LIST:
		return al_list_eql(r, b1, b2);
	case AL_OBJ_ALIST:
		return (al_obj){NULL, 0};
	}
	return AL_OBJ_NULL;
}

//////////////////////////////////////////////////
// C API
//////////////////////////////////////////////////

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

int cal_obj_isIndexable(al_region r, al_obj b)
{
	switch(cal_obj_getType(r, b)){
	case AL_OBJ_ATOM:
		return cal_atom_isIndexable(r, b);
	case AL_OBJ_LIST:
		return cal_list_isIndexable(r, b);
	case AL_OBJ_ALIST:
		return 0;
	}
	return 0;
}

al_obj cal_obj_nth(al_region r, al_obj b, int n)
{
	switch(cal_obj_getType(r, b)){
	case AL_OBJ_ATOM:
		// use al, not cal because cal returns a char
		return al_atom_nth(r, b, cal_atom_int32(r, n));
	case AL_OBJ_LIST:
		return cal_list_nth(r, b, n);
	case AL_OBJ_ALIST:
		return (al_obj){NULL, 0};
	}
	return AL_OBJ_NULL;
}

int cal_obj_length(al_region r, al_obj b)
{
	switch(cal_obj_getType(r, b)){
	case AL_OBJ_ATOM:
		return cal_atom_length(r, b);
	case AL_OBJ_LIST:
		return cal_list_length(r, b);
	case AL_OBJ_ALIST:
		return 0;
	}
	return 0;
}

int cal_obj_eql(al_region r, al_obj b1, al_obj b2)
{
	if(cal_obj_getType(r, b1) != cal_obj_getType(r, b2)){
		return 0;
	}
	switch(cal_obj_getType(r, b1)){
	case AL_OBJ_ATOM:
		return cal_atom_eql(r, b1, b2);
	case AL_OBJ_LIST:
		return cal_list_eql(r, b1, b2);
	case AL_OBJ_ALIST:
		return 0;
	}
	return 0;
}

int cal_obj_eqv(al_region r, al_obj b1, al_obj b2)
{
	if(cal_obj_getType(r, b1) != cal_obj_getType(r, b2)){
		return 0;
	}
	switch(cal_obj_getType(r, b1)){
	case AL_OBJ_ATOM:
		return cal_atom_eql(r, b1, b2);
	case AL_OBJ_LIST:
		return cal_list_eql(r, b1, b2);
	case AL_OBJ_ALIST:
		return 0;
	}
	return 0;
}
