#ifndef __AL_ATOM_H__
#define __AL_ATOM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "al_obj.h"
#include "al_osc.h"
#include "al_region.h"
#include "al_c.h"
#include "al_type.h"
#include "al_time.h"

typedef al_obj al_atom;

al_obj al_atom_copy(al_region r, al_obj b);

al_obj al_atom_isIndexable(al_region r, al_obj b);
al_obj al_atom_nth(al_region r, al_obj b, al_obj n);
al_obj al_atom_rest(al_region r, al_obj b);
al_obj al_atom_length(al_region r, al_obj b);

al_obj al_atom_eql(al_region r, al_obj b1, al_obj b2);
al_obj al_atom_eqv(al_region r, al_obj b1, al_obj b2);

//////////////////////////////////////////////////
// atom specific
//////////////////////////////////////////////////
al_obj al_atom_isInt(al_region r, al_obj a);
al_obj al_atom_isFloat(al_region r, al_obj a);
al_obj al_atom_add(al_region r, al_obj lhs, al_obj rhs);

//////////////////////////////////////////////////
// C API
//////////////////////////////////////////////////
char cal_atom_getType(al_region r, al_obj v);
size_t cal_atom_nformat(al_region r, char *s, size_t n, al_obj b);

al_obj cal_atom_int8(al_region r, int8_t i);
al_obj cal_atom_uint8(al_region r, uint8_t i);
al_obj cal_atom_int16(al_region r, int16_t i);
al_obj cal_atom_uint16(al_region r, uint16_t i);
al_obj cal_atom_int32(al_region r, int32_t i);
al_obj cal_atom_uint32(al_region r, uint32_t i);
al_obj cal_atom_int64(al_region r, int64_t i);
al_obj cal_atom_uint64(al_region r, uint64_t i);
al_obj cal_atom_float(al_region r, float f);
al_obj cal_atom_double(al_region r, double f);
al_obj cal_atom_string(al_region r, char *s);
al_obj cal_atom_symbol(al_region r, char *s);
al_obj cal_atom_time(al_region r, al_time t);
al_obj cal_atom_blob(al_region r, int32_t num_bytes, char *b);
al_obj cal_atom_unit(al_region r, char typetag);
al_obj cal_atom_true(al_region r);
al_obj cal_atom_false(al_region r);
al_obj cal_atom_nil(al_region r);
al_obj cal_atom_fn(al_region r, al_c_fnptr fn, char *name);

al_c_int8 cal_atom_getInt8(al_region r, al_obj v);
al_c_uint8 cal_atom_getUInt8(al_region r, al_obj v);
al_c_int16 cal_atom_getInt16(al_region r, al_obj v);
al_c_uint16 cal_atom_getUInt16(al_region r, al_obj v);
al_c_int32 cal_atom_getInt32(al_region r, al_obj v);
al_c_uint32 cal_atom_getUInt32(al_region r, al_obj v);
al_c_int64 cal_atom_getInt64(al_region r, al_obj v);
al_c_uint64 cal_atom_getUInt64(al_region r, al_obj v);
al_c_float cal_atom_getFloat(al_region r, al_obj v);
al_c_double cal_atom_getDouble(al_region r, al_obj v);
al_c_ptr cal_atom_getPtr(al_region r, al_obj v);
al_c_fn cal_atom_getFn(al_region r, al_obj v);
al_c_ptr cal_atom_getFnName(al_region r, al_obj v);

al_obj cal_atom_convert(al_region r, al_obj b, al_type newtype);
int cal_atom_isIndexable(al_region r, al_obj b);
char cal_atom_nth(al_region r, al_obj b, int i);
int cal_atom_length(al_region r, al_obj b);
int cal_atom_eql(al_region r, al_obj b1, al_obj b2);
int cal_atom_eqv(al_region r, al_obj b1, al_obj b2);
int cal_atom_isInt(al_region r, al_obj a);
int cal_atom_isFloat(al_region r, al_obj a);
al_c_int32 cal_atom_strcmp(al_region r, al_obj s1, al_obj s2);

int cal_atom_print(al_region r, al_obj a);
int cal_atom_println(al_region r, al_obj a);

#ifdef __cplusplus
}
#endif
	
#endif
