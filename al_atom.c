#include <stdio.h>
#include <string.h>
#include "al_atom.h"
#include "al_type.h"
#include "al_endian.h"

static al_obj cal_atom_alloc(al_region r, al_osc value);

al_obj al_atom_copy(al_region r, al_obj b)
{
	if(cal_obj_getType(r, b) == AL_OBJ_TYPE_ATOM){
		return cal_atom_alloc(r, al_osc_copy(r, cal_obj_getObj(r, b)));
	}else{
		return al_obj_copy(r, b);
	}
}

al_obj al_atom_isIndexable(al_region r, al_obj b)
{
	if(cal_obj_getType(r, b) == AL_OBJ_TYPE_ATOM){
		int ret = cal_atom_isIndexable(r, b);
		if(ret == 1){
			return cal_atom_true(r);
		}else if(ret == 0){
			return cal_atom_false(r);
		}else{
			return cal_atom_nil(r);
		}
	}else{
		return al_obj_isIndexable(r, b);
	}
}

al_obj al_atom_nth(al_region r, al_obj b, al_obj n)
{
	if(cal_obj_getType(r, b) == AL_OBJ_TYPE_ATOM){
		al_c_int32 i = cal_atom_getInt32(r, n);
		if(al_c_error(i)){
			return cal_atom_int8(r, 0);
		}else{
			return cal_atom_int8(r, cal_atom_nth(r, b, al_c_value(i)));
		}
	}else{
		return al_obj_nth(r, b, n);
	}
}

al_obj al_atom_rest(al_region r, al_obj b)
{
	if(cal_obj_getType(r, b) == AL_OBJ_TYPE_ATOM && cal_atom_isIndexable(r, b)){
		switch(cal_atom_getType(r, b)){
		case AL_TYPE_STR:
			{
				al_c_ptr p = cal_atom_getPtr(r, b);
				if(!al_c_error(p)){
					char *st = al_c_value(p);
					return cal_atom_symbol(r, st + 1);
				}
			}
		case AL_TYPE_SYM:
			{
				al_c_ptr p = cal_atom_getPtr(r, b);
				if(!al_c_error(p)){
					char *st = al_c_value(p);
					return cal_atom_string(r, st + 1);
				}
			}
		case AL_TYPE_BLOB:
			{
				al_c_ptr p = cal_atom_getPtr(r, b);
				if(!al_c_error(p)){
					char *st = al_c_value(p);
					int32_t len = ntoh32(*((int32_t *)st));
					char *buf = al_region_getBytes(r, len + 8);
					memset(buf, 0, len + 8);
					*((int32_t *)st) = hton32(len - 1);
					memcpy(buf + 4, st + 1, len - 1);
					return cal_atom_blob(r, len + 4, st);
				}
			}
		default: return cal_atom_nil(r);
		}

	}else{
		return cal_atom_nil(r);
	}
}

al_obj al_atom_length(al_region r, al_obj b)
{
	if(cal_obj_getType(r, b) == AL_OBJ_TYPE_ATOM){
		return cal_atom_int32(r, cal_atom_length(r, b));
	}
	return al_obj_length(r, b);
}

al_obj al_atom_eql(al_region r, al_obj b1, al_obj b2)
{
	if(cal_obj_getType(r, b1) == AL_OBJ_TYPE_ATOM && cal_obj_getType(r, b2) == AL_OBJ_TYPE_ATOM){
		return cal_atom_unit(r, cal_atom_eql(r, b1, b2) == 0 ? AL_TYPE_FALSE : AL_TYPE_TRUE);
	}
	return al_obj_eql(r, b1, b2);
}

al_obj al_atom_eqv(al_region r, al_obj b1, al_obj b2)
{
	if(cal_obj_getType(r, b1) == AL_OBJ_TYPE_ATOM && cal_obj_getType(r, b2) == AL_OBJ_TYPE_ATOM){
		return cal_atom_unit(r, cal_atom_eqv(r, b1, b2) == 0 ? AL_TYPE_FALSE : AL_TYPE_TRUE);
	}
	return al_obj_eqv(r, b1, b2);
}

al_obj al_atom_eval(al_region r, al_obj a, al_obj context)
{
	if(cal_obj_getType(r, a) == AL_OBJ_TYPE_ATOM){
		return a;
	}else{
		return al_obj_eval(r, a, context);
	}
}

//////////////////////////////////////////////////
// atom specific
//////////////////////////////////////////////////

al_obj al_atom_isInt(al_region r, al_obj a)
{
	if(cal_obj_getType(r, a) == AL_OBJ_TYPE_ATOM){
		return cal_atom_unit(r, cal_atom_isInt(r, a) == 0 ? AL_TYPE_FALSE : AL_TYPE_TRUE);
	}else{
		return cal_atom_false(r);
	}
}

al_obj al_atom_isFloat(al_region r, al_obj a)
{
	if(cal_obj_getType(r, a) == AL_OBJ_TYPE_ATOM){
		return cal_atom_unit(r, cal_atom_isFloat(r, a) == 0 ? AL_TYPE_FALSE : AL_TYPE_TRUE);
	}else{
		return cal_atom_false(r);
	}
}

al_obj al_atom_add(al_region r, al_obj lhs, al_obj rhs)
{
	if(cal_obj_getType(r, lhs) == AL_OBJ_TYPE_ATOM && cal_obj_getType(r, rhs) == AL_OBJ_TYPE_ATOM){
		return cal_atom_alloc(r, al_osc_add(r, cal_obj_getObj(r, lhs), cal_obj_getObj(r, rhs)));
	}
	return cal_atom_nil(r);
}

//////////////////////////////////////////////////
// C API
//////////////////////////////////////////////////
static al_obj cal_atom_alloc(al_region r, al_osc value)
{
	return cal_obj_alloc(r, value, AL_OBJ_TYPE_ATOM);
}

size_t cal_atom_nformat(al_region r, char *s, size_t n, al_obj b)
{
	if(cal_obj_getType(r, b) == AL_OBJ_TYPE_ATOM){
		return al_osc_nformat(r, s, n, cal_obj_getObj(r, b));
	}
	return 0;
}

al_type cal_atom_getType(al_region r, al_obj v)
{
	if(cal_obj_getType(r, v) == AL_OBJ_TYPE_ATOM){
		return al_osc_getType(r, cal_obj_getObj(r, v));
	}else{
		return cal_obj_getType(r, v);
	}
}

al_obj cal_atom_int8(al_region r, int8_t i)
{
	return cal_atom_alloc(r, al_osc_int8(r, i));
}

al_obj cal_atom_uint8(al_region r, uint8_t i)
{
	return cal_atom_alloc(r, al_osc_uint8(r, i));
}

al_obj cal_atom_int16(al_region r, int16_t i)
{
	return cal_atom_alloc(r, al_osc_int16(r, i));
}

al_obj cal_atom_uint16(al_region r, uint16_t i)
{
	return cal_atom_alloc(r, al_osc_uint16(r, i));
}

al_obj cal_atom_int32(al_region r, int32_t i)
{
	return cal_atom_alloc(r, al_osc_int32(r, i));
}

al_obj cal_atom_uint32(al_region r, uint32_t i)
{
	return cal_atom_alloc(r, al_osc_uint32(r, i));
}

al_obj cal_atom_int64(al_region r, int64_t i)
{
	return cal_atom_alloc(r, al_osc_int64(r, i));
}

al_obj cal_atom_uint64(al_region r, uint64_t i)
{
	return cal_atom_alloc(r, al_osc_uint64(r, i));
}

al_obj cal_atom_float(al_region r, float f)
{
	return cal_atom_alloc(r, al_osc_float(r, f));
}

al_obj cal_atom_double(al_region r, double f)
{
	return cal_atom_alloc(r, al_osc_double(r, f));
}

al_obj cal_atom_string(al_region r, char *s)
{
	return cal_atom_alloc(r, al_osc_string(r, s));
}

al_obj cal_atom_symbol(al_region r, char *s)
{
	return cal_atom_alloc(r, al_osc_symbol(r, s));
}

al_obj cal_atom_time(al_region r, al_time t)
{
	return cal_atom_alloc(r, al_osc_time(r, t));
}

al_obj cal_atom_blob(al_region r, int32_t num_bytes, char *b)
{
	return cal_atom_alloc(r, al_osc_blob(r, num_bytes, b));
}

al_obj cal_atom_unit(al_region r, char typetag)
{
	return cal_atom_alloc(r, al_osc_unit(r, typetag));
}

al_obj cal_atom_true(al_region r)
{
	return cal_atom_unit(r, AL_TYPE_TRUE);
}

al_obj cal_atom_false(al_region r)
{
	return cal_atom_unit(r, AL_TYPE_FALSE);
}

al_obj cal_atom_nil(al_region r)
{
	return cal_atom_unit(r, AL_TYPE_NIL);
}

al_obj cal_atom_fn(al_region r, al_c_fnptr fn, char *name)
{
	return cal_atom_alloc(r, al_osc_fn(r, fn, name));
}

al_c_int8 cal_atom_getInt8(al_region r, al_obj v)
{
	if(cal_obj_getType(r, v) == AL_OBJ_TYPE_ATOM && cal_atom_getType(r, v) == AL_TYPE_I8){
		return al_osc_getInt8(r, cal_obj_getObj(r, v));
	}
	return al_c_create(int8, 0, 1);
}

al_c_uint8 cal_atom_getUInt8(al_region r, al_obj v)
{
	if(cal_obj_getType(r, v) == AL_OBJ_TYPE_ATOM && cal_atom_getType(r, v) == AL_TYPE_U8){
		return al_osc_getUInt8(r, cal_obj_getObj(r, v));
	}
	return al_c_create(uint8, 0, 1);
}

al_c_int16 cal_atom_getInt16(al_region r, al_obj v)
{
	if(cal_obj_getType(r, v) == AL_OBJ_TYPE_ATOM && cal_atom_getType(r, v) == AL_TYPE_I16){
		return al_osc_getInt16(r, cal_obj_getObj(r, v));
	}
	return al_c_create(int16, 0, 1);
}

al_c_uint16 cal_atom_getUInt16(al_region r, al_obj v)
{
	if(cal_obj_getType(r, v) == AL_OBJ_TYPE_ATOM && cal_atom_getType(r, v) == AL_TYPE_U16){
		return al_osc_getUInt16(r, cal_obj_getObj(r, v));
	}
	return al_c_create(uint16, 0, 1);
}

al_c_int32 cal_atom_getInt32(al_region r, al_obj v)
{
	if(cal_obj_getType(r, v) == AL_OBJ_TYPE_ATOM && cal_atom_getType(r, v) == AL_TYPE_I32){
		return al_osc_getInt32(r, cal_obj_getObj(r, v));
	}
	return al_c_create(int32, 0, 1);
}

al_c_uint32 cal_atom_getUInt32(al_region r, al_obj v)
{
	if(cal_obj_getType(r, v) == AL_OBJ_TYPE_ATOM && cal_atom_getType(r, v) == AL_TYPE_U32){
		return al_osc_getUInt32(r, cal_obj_getObj(r, v));
	}
	return al_c_create(uint32, 0, 1);
}

al_c_int64 cal_atom_getInt64(al_region r, al_obj v)
{
	if(cal_obj_getType(r, v) == AL_OBJ_TYPE_ATOM && cal_atom_getType(r, v) == AL_TYPE_I64){
		return al_osc_getInt64(r, cal_obj_getObj(r, v));
	}
	return al_c_create(int64, 0, 1);
}

al_c_uint64 cal_atom_getUInt64(al_region r, al_obj v)
{
	if(cal_obj_getType(r, v) == AL_OBJ_TYPE_ATOM && cal_atom_getType(r, v) == AL_TYPE_U64){
		return al_osc_getUInt64(r, cal_obj_getObj(r, v));
	}
	return al_c_create(uint64, 0, 1);
}

al_c_float cal_atom_getFloat(al_region r, al_obj v)
{
	if(cal_obj_getType(r, v) == AL_OBJ_TYPE_ATOM && cal_atom_getType(r, v) == AL_TYPE_F32){
		return al_osc_getFloat(r, cal_obj_getObj(r, v));
	}
	return al_c_create(float, 0, 1);
}

al_c_double cal_atom_getDouble(al_region r, al_obj v)
{
	if(cal_obj_getType(r, v) == AL_OBJ_TYPE_ATOM && cal_atom_getType(r, v) == AL_TYPE_F64){
		return al_osc_getDouble(r, cal_obj_getObj(r, v));
	}
	return al_c_create(double, 0, 1);
}

al_c_ptr cal_atom_getPtr(al_region r, al_obj v)
{
	if(cal_obj_getType(r, v) == AL_OBJ_TYPE_ATOM){
		if(cal_atom_getType(r, v) == AL_TYPE_STR ||
	    	   cal_atom_getType(r, v) == AL_TYPE_SYM ||
		   cal_atom_getType(r, v) == AL_TYPE_BLOB){
			return al_osc_getPtr(r, cal_obj_getObj(r, v));
		}
	}
	return al_c_create(ptr, 0, 1);
}

al_c_fn cal_atom_getFn(al_region r, al_obj v)
{
	if(cal_obj_getType(r, v) == AL_OBJ_TYPE_ATOM && cal_atom_getType(r, v) == AL_TYPE_FN){
		return al_osc_getFn(r, cal_obj_getObj(r, v));
	}
	return al_c_create(fn, 0, 1);
}

al_c_ptr cal_atom_getFnName(al_region r, al_obj v)
{
	if(cal_obj_getType(r, v) == AL_OBJ_TYPE_ATOM && cal_atom_getType(r, v) == AL_TYPE_FN){
		return al_osc_getFnName(r, cal_obj_getObj(r, v));
	}
	return al_c_create(ptr, 0, 1);
}

al_obj cal_atom_convert(al_region r, al_obj b, char newtype)
{
	if(cal_obj_getType(r, b) == AL_OBJ_TYPE_ATOM){
		return cal_atom_alloc(r, al_osc_convert(r, cal_obj_getObj(r, b), newtype));
	}
	return b;
}

int cal_atom_isIndexable(al_region r, al_obj b)
{
	if(cal_obj_getType(r, b) == AL_OBJ_TYPE_ATOM){
		return -1;
	}
	switch(cal_atom_getType(r, b)){
	case AL_TYPE_SYM:
	case AL_TYPE_STR:
	case AL_TYPE_BLOB:
		return 1;
	default: return 0;
	}
}

char cal_atom_nth(al_region r, al_obj b, int i)
{
	if(cal_atom_isIndexable(r, b)){
		switch(cal_atom_getType(r, b)){
		case AL_TYPE_SYM:
		case AL_TYPE_STR:
			{
				al_c_ptr cv = cal_atom_getPtr(r, b);
				if(!al_c_error(cv)){
					char *ptr = al_c_value(cv);
					if(i < strlen(ptr)){
						return ptr[i];
					}
				}
				return 0;
			}
		case AL_TYPE_BLOB:
			{
				al_c_ptr cv = cal_atom_getPtr(r, b);
				if(!al_c_error(cv)){
					char *ptr = al_c_value(cv);
					int32_t len = ntoh32(*((int32_t *)ptr));
					if(i < len){
						return ptr[i + 4];
					}
				}
				return 0;
			}
		}
	}
	return 0;
}

int cal_atom_length(al_region r, al_obj b)
{
	if(cal_obj_getType(r, b) != AL_OBJ_TYPE_ATOM){
		return 0;
	}
	switch(cal_atom_getType(r, b)){
	case AL_TYPE_STR:
	case AL_TYPE_SYM:
		{
			al_c_ptr cv = cal_atom_getPtr(r, b);
			if(!al_c_error(cv)){
				return strlen(al_c_value(cv));
			}
			return 0;
		}
	case AL_TYPE_BLOB:
		{
			al_c_ptr cv = cal_atom_getPtr(r, b);
			if(!al_c_error(cv)){
				char *ptr = al_c_value(cv);
				int32_t len = ntoh32(*((int32_t *)ptr));
				return len;
			}
			return 0;
		}
	}
	return 0;
}

int cal_atom_isInt(al_region r, al_obj a)
{
	if(cal_obj_getType(r, a) == AL_OBJ_TYPE_ATOM){
		return AL_TYPE_ISINT(cal_atom_getType(r, a));
	}else{
		return 0;
	}
}

int cal_atom_isFloat(al_region r, al_obj a)
{
	if(cal_obj_getType(r, a) == AL_OBJ_TYPE_ATOM){
		return AL_TYPE_ISFLOAT(cal_atom_getType(r, a));
	}else{
		return 0;
	}
}

int cal_atom_eql(al_region r, al_obj b1, al_obj b2)
{
	if(cal_obj_getType(r, b1) == AL_OBJ_TYPE_ATOM && cal_obj_getType(r, b2) == AL_OBJ_TYPE_ATOM){
		return al_osc_eql(r, cal_obj_getObj(r, b1), cal_obj_getObj(r, b2));
	}
	return 0;
}

int cal_atom_eqv(al_region r, al_obj b1, al_obj b2)
{
	if(cal_obj_getType(r, b1) == AL_OBJ_TYPE_ATOM && cal_obj_getType(r, b2) == AL_OBJ_TYPE_ATOM){
		return al_osc_eqv(r, cal_obj_getObj(r, b1), cal_obj_getObj(r, b2));
	}
	return 0;
}

al_c_int32 cal_atom_strcmp(al_region r, al_obj s1, al_obj s2)
{
	if(cal_obj_getType(r, s1) != AL_OBJ_TYPE_ATOM && cal_obj_getType(r, s2) != AL_OBJ_TYPE_ATOM){
		return al_c_create(int32, 0, 1);
	}
	char tt1 = cal_atom_getType(r, s1);
	char tt2 = cal_atom_getType(r, s2);
	if(!(tt1 == AL_TYPE_STR || tt1 == AL_TYPE_SYM) && !(tt2 == AL_TYPE_STR || tt2 == AL_TYPE_SYM)){
		return al_c_create(int32, 0, 1);
	}
	if((tt1 == AL_TYPE_STR || tt1 == AL_TYPE_SYM) && !(tt2 == AL_TYPE_STR || tt2 == AL_TYPE_SYM)){
		return al_c_create(int32, 1, 1);
	}
	if(!(tt1 == AL_TYPE_STR || tt1 == AL_TYPE_SYM) && (tt2 == AL_TYPE_STR || tt2 == AL_TYPE_SYM)){
		return al_c_create(int32, -1, 1);
	}
	return al_c_create(int32, strcmp(al_c_value(cal_atom_getPtr(r, s1)), al_c_value(cal_atom_getPtr(r, s2))), 0);
}

int cal_atom_print(al_region r, al_obj a)
{
	if(cal_obj_getType(r, a) == AL_OBJ_TYPE_ATOM){
		size_t l = cal_atom_nformat(r, NULL, 0, a);
		char buf[l + 1];
		cal_atom_nformat(r, buf, l + 1, a);
		return printf("%s", buf);
	}
	return 0;
}

int cal_atom_println(al_region r, al_obj a)
{
	return cal_atom_print(r, a) + printf("\n");
}
