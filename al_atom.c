#include <stdio.h>
#include <string.h>
#include "al_atom.h"
#include "al_type.h"
#include "al_endian.h"

static al_obj cal_atom_alloc(al_env env, al_osc value);

al_obj al_atom_copy(al_env env, al_obj b)
{
	if(cal_obj_getType(env, b) == AL_OBJ_TYPE_ATOM){
		return cal_atom_alloc(env, al_osc_copy(al_env_getRegion(env), cal_obj_getObj(env, b)));
	}else{
		return al_obj_copy(env, b);
	}
}

al_obj al_atom_isIndexable(al_env env, al_obj b)
{
	if(cal_obj_getType(env, b) == AL_OBJ_TYPE_ATOM){
		int ret = cal_atom_isIndexable(env, b);
		if(ret == 1){
			return cal_atom_true(env);
		}else if(ret == 0){
			return cal_atom_false(env);
		}else{
			return cal_atom_nil(env);
		}
	}else{
		return al_obj_isIndexable(env, b);
	}
}

al_obj al_atom_nth(al_env env, al_obj b, al_obj n)
{
	if(cal_obj_getType(env, b) == AL_OBJ_TYPE_ATOM){
		al_c_int32 i = cal_atom_getInt32(env, n);
		if(al_c_error(i)){
			return cal_atom_int8(env, 0);
		}else{
			return cal_atom_int8(env, cal_atom_nth(env, b, al_c_value(i)));
		}
	}else{
		return al_obj_nth(env, b, n);
	}
}

al_obj al_atom_rest(al_env env, al_obj b)
{
	if(cal_obj_getType(env, b) == AL_OBJ_TYPE_ATOM && cal_atom_isIndexable(env, b)){
		switch(cal_atom_getType(env, b)){
		case AL_TYPE_STR:
			{
				al_c_ptr p = cal_atom_getPtr(env, b);
				if(!al_c_error(p)){
					char *st = al_c_value(p);
					return cal_atom_symbol(env, st + 1);
				}
			}
		case AL_TYPE_SYM:
			{
				al_c_ptr p = cal_atom_getPtr(env, b);
				if(!al_c_error(p)){
					char *st = al_c_value(p);
					return cal_atom_string(env, st + 1);
				}
			}
		case AL_TYPE_BLOB:
			{
				al_c_ptr p = cal_atom_getPtr(env, b);
				if(!al_c_error(p)){
					char *st = al_c_value(p);
					int32_t len = ntoh32(*((int32_t *)st));
					char *buf = al_env_getBytes(env, len + 8);
					memset(buf, 0, len + 8);
					*((int32_t *)st) = hton32(len - 1);
					memcpy(buf + 4, st + 1, len - 1);
					return cal_atom_blob(env, len + 4, st);
				}
			}
		default: return cal_atom_nil(env);
		}

	}else{
		return cal_atom_nil(env);
	}
}

al_obj al_atom_length(al_env env, al_obj b)
{
	if(cal_obj_getType(env, b) == AL_OBJ_TYPE_ATOM){
		return cal_atom_int32(env, cal_atom_length(env, b));
	}
	return al_obj_length(env, b);
}

al_obj al_atom_eql(al_env env, al_obj b1, al_obj b2)
{
	if(cal_obj_getType(env, b1) == AL_OBJ_TYPE_ATOM && cal_obj_getType(env, b2) == AL_OBJ_TYPE_ATOM){
		return cal_atom_unit(env, cal_atom_eql(env, b1, b2) == 0 ? AL_TYPE_FALSE : AL_TYPE_TRUE);
	}
	return al_obj_eql(env, b1, b2);
}

al_obj al_atom_eqv(al_env env, al_obj b1, al_obj b2)
{
	if(cal_obj_getType(env, b1) == AL_OBJ_TYPE_ATOM && cal_obj_getType(env, b2) == AL_OBJ_TYPE_ATOM){
		return cal_atom_unit(env, cal_atom_eqv(env, b1, b2) == 0 ? AL_TYPE_FALSE : AL_TYPE_TRUE);
	}
	return al_obj_eqv(env, b1, b2);
}

al_obj al_atom_eval(al_env env, al_obj a, al_obj context)
{
	if(cal_obj_getType(env, a) == AL_OBJ_TYPE_ATOM){
		return a;
	}else{
		return al_obj_eval(env, a, context);
	}
}

//////////////////////////////////////////////////
// atom specific
//////////////////////////////////////////////////

al_obj al_atom_isInt(al_env env, al_obj a)
{
	if(cal_obj_getType(env, a) == AL_OBJ_TYPE_ATOM){
		return cal_atom_unit(env, cal_atom_isInt(env, a) == 0 ? AL_TYPE_FALSE : AL_TYPE_TRUE);
	}else{
		return cal_atom_false(env);
	}
}

al_obj al_atom_isFloat(al_env env, al_obj a)
{
	if(cal_obj_getType(env, a) == AL_OBJ_TYPE_ATOM){
		return cal_atom_unit(env, cal_atom_isFloat(env, a) == 0 ? AL_TYPE_FALSE : AL_TYPE_TRUE);
	}else{
		return cal_atom_false(env);
	}
}

al_obj al_atom_add(al_env env, al_obj lhs, al_obj rhs)
{
	if(cal_obj_getType(env, lhs) == AL_OBJ_TYPE_ATOM && cal_obj_getType(env, rhs) == AL_OBJ_TYPE_ATOM){
		return cal_atom_alloc(env, al_osc_add(al_env_getRegion(env), cal_obj_getObj(env, lhs), cal_obj_getObj(env, rhs)));
	}
	return cal_atom_nil(env);
}

//////////////////////////////////////////////////
// C API
//////////////////////////////////////////////////
static al_obj cal_atom_alloc(al_env env, al_osc value)
{
	return cal_obj_alloc(env, value, AL_OBJ_TYPE_ATOM);
}

size_t cal_atom_nformat(al_env env, char *s, size_t n, al_obj b)
{
	if(cal_obj_getType(env, b) == AL_OBJ_TYPE_ATOM){
		return al_osc_nformat(al_env_getRegion(env), s, n, cal_obj_getObj(env, b));
	}
	return 0;
}

char cal_atom_getType(al_env env, al_obj v)
{
	if(cal_obj_getType(env, v) == AL_OBJ_TYPE_ATOM){
		return al_osc_getType(al_env_getRegion(env), cal_obj_getObj(env, v));
	}else{
		return cal_obj_getType(env, v);
	}
}

al_obj cal_atom_int8(al_env env, int8_t i)
{
	return cal_atom_alloc(env, al_osc_int8(al_env_getRegion(env), i));
}

al_obj cal_atom_uint8(al_env env, uint8_t i)
{
	return cal_atom_alloc(env, al_osc_uint8(al_env_getRegion(env), i));
}

al_obj cal_atom_int16(al_env env, int16_t i)
{
	return cal_atom_alloc(env, al_osc_int16(al_env_getRegion(env), i));
}

al_obj cal_atom_uint16(al_env env, uint16_t i)
{
	return cal_atom_alloc(env, al_osc_uint16(al_env_getRegion(env), i));
}

al_obj cal_atom_int32(al_env env, int32_t i)
{
	return cal_atom_alloc(env, al_osc_int32(al_env_getRegion(env), i));
}

al_obj cal_atom_uint32(al_env env, uint32_t i)
{
	return cal_atom_alloc(env, al_osc_uint32(al_env_getRegion(env), i));
}

al_obj cal_atom_int64(al_env env, int64_t i)
{
	return cal_atom_alloc(env, al_osc_int64(al_env_getRegion(env), i));
}

al_obj cal_atom_uint64(al_env env, uint64_t i)
{
	return cal_atom_alloc(env, al_osc_uint64(al_env_getRegion(env), i));
}

al_obj cal_atom_float(al_env env, float f)
{
	return cal_atom_alloc(env, al_osc_float(al_env_getRegion(env), f));
}

al_obj cal_atom_double(al_env env, double f)
{
	return cal_atom_alloc(env, al_osc_double(al_env_getRegion(env), f));
}

al_obj cal_atom_string(al_env env, char *s)
{
	return cal_atom_alloc(env, al_osc_string(al_env_getRegion(env), s));
}

al_obj cal_atom_symbol(al_env env, char *s)
{
	return cal_atom_alloc(env, al_osc_symbol(al_env_getRegion(env), s));
}

al_obj cal_atom_time(al_env env, al_time t)
{
	return cal_atom_alloc(env, al_osc_time(al_env_getRegion(env), t));
}

al_obj cal_atom_blob(al_env env, int32_t num_bytes, char *b)
{
	return cal_atom_alloc(env, al_osc_blob(al_env_getRegion(env), num_bytes, b));
}

al_obj cal_atom_unit(al_env env, char typetag)
{
	return cal_atom_alloc(env, al_osc_unit(al_env_getRegion(env), typetag));
}

al_obj cal_atom_true(al_env env)
{
	return cal_atom_unit(env, AL_TYPE_TRUE);
}

al_obj cal_atom_false(al_env env)
{
	return cal_atom_unit(env, AL_TYPE_FALSE);
}

al_obj cal_atom_nil(al_env env)
{
	return cal_atom_unit(env, AL_TYPE_NIL);
}

al_obj cal_atom_fn(al_env env, al_c_fnptr fn, char *name)
{
	return cal_atom_alloc(env, al_osc_fn(al_env_getRegion(env), fn, name));
}

al_c_int8 cal_atom_getInt8(al_env env, al_obj v)
{
	if(cal_obj_getType(env, v) == AL_OBJ_TYPE_ATOM && cal_atom_getType(env, v) == AL_TYPE_I8){
		return al_osc_getInt8(al_env_getRegion(env), cal_obj_getObj(env, v));
	}
	return al_c_create(int8, 0, 1);
}

al_c_uint8 cal_atom_getUInt8(al_env env, al_obj v)
{
	if(cal_obj_getType(env, v) == AL_OBJ_TYPE_ATOM && cal_atom_getType(env, v) == AL_TYPE_U8){
		return al_osc_getUInt8(al_env_getRegion(env), cal_obj_getObj(env, v));
	}
	return al_c_create(uint8, 0, 1);
}

al_c_int16 cal_atom_getInt16(al_env env, al_obj v)
{
	if(cal_obj_getType(env, v) == AL_OBJ_TYPE_ATOM && cal_atom_getType(env, v) == AL_TYPE_I16){
		return al_osc_getInt16(al_env_getRegion(env), cal_obj_getObj(env, v));
	}
	return al_c_create(int16, 0, 1);
}

al_c_uint16 cal_atom_getUInt16(al_env env, al_obj v)
{
	if(cal_obj_getType(env, v) == AL_OBJ_TYPE_ATOM && cal_atom_getType(env, v) == AL_TYPE_U16){
		return al_osc_getUInt16(al_env_getRegion(env), cal_obj_getObj(env, v));
	}
	return al_c_create(uint16, 0, 1);
}

al_c_int32 cal_atom_getInt32(al_env env, al_obj v)
{
	if(cal_obj_getType(env, v) == AL_OBJ_TYPE_ATOM && cal_atom_getType(env, v) == AL_TYPE_I32){
		return al_osc_getInt32(al_env_getRegion(env), cal_obj_getObj(env, v));
	}
	return al_c_create(int32, 0, 1);
}

al_c_uint32 cal_atom_getUInt32(al_env env, al_obj v)
{
	if(cal_obj_getType(env, v) == AL_OBJ_TYPE_ATOM && cal_atom_getType(env, v) == AL_TYPE_U32){
		return al_osc_getUInt32(al_env_getRegion(env), cal_obj_getObj(env, v));
	}
	return al_c_create(uint32, 0, 1);
}

al_c_int64 cal_atom_getInt64(al_env env, al_obj v)
{
	if(cal_obj_getType(env, v) == AL_OBJ_TYPE_ATOM && cal_atom_getType(env, v) == AL_TYPE_I64){
		return al_osc_getInt64(al_env_getRegion(env), cal_obj_getObj(env, v));
	}
	return al_c_create(int64, 0, 1);
}

al_c_uint64 cal_atom_getUInt64(al_env env, al_obj v)
{
	if(cal_obj_getType(env, v) == AL_OBJ_TYPE_ATOM && cal_atom_getType(env, v) == AL_TYPE_U64){
		return al_osc_getUInt64(al_env_getRegion(env), cal_obj_getObj(env, v));
	}
	return al_c_create(uint64, 0, 1);
}

al_c_float cal_atom_getFloat(al_env env, al_obj v)
{
	if(cal_obj_getType(env, v) == AL_OBJ_TYPE_ATOM && cal_atom_getType(env, v) == AL_TYPE_F32){
		return al_osc_getFloat(al_env_getRegion(env), cal_obj_getObj(env, v));
	}
	return al_c_create(float, 0, 1);
}

al_c_double cal_atom_getDouble(al_env env, al_obj v)
{
	if(cal_obj_getType(env, v) == AL_OBJ_TYPE_ATOM && cal_atom_getType(env, v) == AL_TYPE_F64){
		return al_osc_getDouble(al_env_getRegion(env), cal_obj_getObj(env, v));
	}
	return al_c_create(double, 0, 1);
}

al_c_ptr cal_atom_getPtr(al_env env, al_obj v)
{
	if(cal_obj_getType(env, v) == AL_OBJ_TYPE_ATOM){
		if(cal_atom_getType(env, v) == AL_TYPE_STR ||
	    	   cal_atom_getType(env, v) == AL_TYPE_SYM ||
		   cal_atom_getType(env, v) == AL_TYPE_BLOB){
			return al_osc_getPtr(al_env_getRegion(env), cal_obj_getObj(env, v));
		}
	}
	return al_c_create(ptr, 0, 1);
}

al_c_fn cal_atom_getFn(al_env env, al_obj v)
{
	if(cal_obj_getType(env, v) == AL_OBJ_TYPE_ATOM && cal_atom_getType(env, v) == AL_TYPE_FN){
		return al_osc_getFn(al_env_getRegion(env), cal_obj_getObj(env, v));
	}
	return al_c_create(fn, 0, 1);
}

al_c_ptr cal_atom_getFnName(al_env env, al_obj v)
{
	if(cal_obj_getType(env, v) == AL_OBJ_TYPE_ATOM && cal_atom_getType(env, v) == AL_TYPE_FN){
		return al_osc_getFnName(al_env_getRegion(env), cal_obj_getObj(env, v));
	}
	return al_c_create(ptr, 0, 1);
}

al_obj cal_atom_convert(al_env env, al_obj b, char newtype)
{
	if(cal_obj_getType(env, b) == AL_OBJ_TYPE_ATOM){
		return cal_atom_alloc(env, al_osc_convert(al_env_getRegion(env), cal_obj_getObj(env, b), newtype));
	}
	return b;
}

int cal_atom_isIndexable(al_env env, al_obj b)
{
	if(cal_obj_getType(env, b) == AL_OBJ_TYPE_ATOM){
		return -1;
	}
	switch(cal_atom_getType(env, b)){
	case AL_TYPE_SYM:
	case AL_TYPE_STR:
	case AL_TYPE_BLOB:
		return 1;
	default: return 0;
	}
}

char cal_atom_nth(al_env env, al_obj b, int i)
{
	if(cal_atom_isIndexable(env, b)){
		switch(cal_atom_getType(env, b)){
		case AL_TYPE_SYM:
		case AL_TYPE_STR:
			{
				al_c_ptr cv = cal_atom_getPtr(env, b);
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
				al_c_ptr cv = cal_atom_getPtr(env, b);
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

int cal_atom_length(al_env env, al_obj b)
{
	if(cal_obj_getType(env, b) != AL_OBJ_TYPE_ATOM){
		return 0;
	}
	switch(cal_atom_getType(env, b)){
	case AL_TYPE_STR:
	case AL_TYPE_SYM:
		{
			al_c_ptr cv = cal_atom_getPtr(env, b);
			if(!al_c_error(cv)){
				return strlen(al_c_value(cv));
			}
			return 0;
		}
	case AL_TYPE_BLOB:
		{
			al_c_ptr cv = cal_atom_getPtr(env, b);
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

int cal_atom_isInt(al_env env, al_obj a)
{
	if(cal_obj_getType(env, a) == AL_OBJ_TYPE_ATOM){
		return AL_TYPE_ISINT(cal_atom_getType(env, a));
	}else{
		return 0;
	}
}

int cal_atom_isFloat(al_env env, al_obj a)
{
	if(cal_obj_getType(env, a) == AL_OBJ_TYPE_ATOM){
		return AL_TYPE_ISFLOAT(cal_atom_getType(env, a));
	}else{
		return 0;
	}
}

int cal_atom_eql(al_env env, al_obj b1, al_obj b2)
{
	if(cal_obj_getType(env, b1) == AL_OBJ_TYPE_ATOM && cal_obj_getType(env, b2) == AL_OBJ_TYPE_ATOM){
		return al_osc_eql(al_env_getRegion(env), cal_obj_getObj(env, b1), cal_obj_getObj(env, b2));
	}
	return 0;
}

int cal_atom_eqv(al_env env, al_obj b1, al_obj b2)
{
	if(cal_obj_getType(env, b1) == AL_OBJ_TYPE_ATOM && cal_obj_getType(env, b2) == AL_OBJ_TYPE_ATOM){
		return al_osc_eqv(al_env_getRegion(env), cal_obj_getObj(env, b1), cal_obj_getObj(env, b2));
	}
	return 0;
}

al_c_int32 cal_atom_strcmp(al_env env, al_obj s1, al_obj s2)
{
	if(cal_obj_getType(env, s1) != AL_OBJ_TYPE_ATOM && cal_obj_getType(env, s2) != AL_OBJ_TYPE_ATOM){
		return al_c_create(int32, 0, 1);
	}
	char tt1 = cal_atom_getType(env, s1);
	char tt2 = cal_atom_getType(env, s2);
	if(!(tt1 == AL_TYPE_STR || tt1 == AL_TYPE_SYM) && !(tt2 == AL_TYPE_STR || tt2 == AL_TYPE_SYM)){
		return al_c_create(int32, 0, 1);
	}
	if((tt1 == AL_TYPE_STR || tt1 == AL_TYPE_SYM) && !(tt2 == AL_TYPE_STR || tt2 == AL_TYPE_SYM)){
		return al_c_create(int32, 1, 1);
	}
	if(!(tt1 == AL_TYPE_STR || tt1 == AL_TYPE_SYM) && (tt2 == AL_TYPE_STR || tt2 == AL_TYPE_SYM)){
		return al_c_create(int32, -1, 1);
	}
	return al_c_create(int32, strcmp(al_c_value(cal_atom_getPtr(env, s1)), al_c_value(cal_atom_getPtr(env, s2))), 0);
}

int cal_atom_print(al_env env, al_obj a)
{
	if(cal_obj_getType(env, a) == AL_OBJ_TYPE_ATOM){
		size_t l = cal_atom_nformat(env, NULL, 0, a);
		char buf[l + 1];
		cal_atom_nformat(env, buf, l + 1, a);
		return printf("%s", buf);
	}
	return 0;
}

int cal_atom_println(al_env env, al_obj a)
{
	return cal_atom_print(env, a) + printf("\n");
}
