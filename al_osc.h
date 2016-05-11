#ifndef __AL_OSC_H__
#define __AL_OSC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <inttypes.h>
#include "al_osctime.h"
#include "al_region.h"
#include "al_c.h"

#define AL_OSC_HEADER_SIZE 16 // bundle\0 + 8 byte timetag.
#define AL_OSC_IDENTIFIER "#bundle\0"
#define AL_OSC_IDENTIFIER_SIZE 8
#define AL_OSC_EMPTY_HEADER "#bundle\0\0\0\0\0\0\0\0\0"
#define AL_OSC_ID AL_OSC_IDENTIFIER
#define AL_OSC_ID_SIZE AL_OSC_IDENTIFIER_SIZE

typedef char* al_osc;

al_osc al_osc_copy(al_region r, al_osc n);
al_osctime al_osc_getTimetag(al_region r, al_osc n);
int32_t al_osc_getSize(al_region r, al_osc n);
char al_osc_getType(al_region r, al_osc n);
char *al_osc_getData(al_region r, al_osc n);
size_t al_osc_nformat(al_region r, char *s, size_t n, al_osc x);

al_osc al_osc_int8(al_region r, int8_t i);
al_osc al_osc_uint8(al_region r, uint8_t i);
al_osc al_osc_int16(al_region r, int16_t i);
al_osc al_osc_uint16(al_region r, uint16_t i);
al_osc al_osc_int32(al_region r, int32_t i);
al_osc al_osc_uint32(al_region r, uint32_t i);
al_osc al_osc_int64(al_region r, int64_t i);
al_osc al_osc_uint64(al_region r, uint64_t i);
al_osc al_osc_float(al_region r, float f);
al_osc al_osc_double(al_region r, double f);
al_osc al_osc_string(al_region r, char *s);
al_osc al_osc_symbol(al_region r, char *s);
al_osc al_osc_time(al_region r, al_osctime t);
al_osc al_osc_blob(al_region r, int32_t num_bytes, char *b);
al_osc al_osc_unit(al_region r, char typetag);
al_osc al_osc_fn(al_region r, al_c_fnptr fn, char *name);

al_c_int8 al_osc_getInt8(al_region r, al_osc v);
al_c_uint8 al_osc_getUInt8(al_region r, al_osc v);
al_c_int16 al_osc_getInt16(al_region r, al_osc v);
al_c_uint16 al_osc_getUInt16(al_region r, al_osc v);
al_c_int32 al_osc_getInt32(al_region r, al_osc v);
al_c_uint32 al_osc_getUInt32(al_region r, al_osc v);
al_c_int64 al_osc_getInt64(al_region r, al_osc v);
al_c_uint64 al_osc_getUInt64(al_region r, al_osc v);
al_c_float al_osc_getFloat(al_region r, al_osc v);
al_c_double al_osc_getDouble(al_region r, al_osc v);
al_c_ptr al_osc_getPtr(al_region r, al_osc v);
al_c_time al_osc_getTime(al_region r, al_osc v);
al_c_fn al_osc_getFn(al_region r, al_osc v);
al_c_ptr al_osc_getFnName(al_region r, al_osc v);

al_osc al_osc_convert(al_region r, al_osc n, char newtype);

int al_osc_eql(al_region r, al_osc n1, al_osc n2);
int al_osc_eqv(al_region r, al_osc n1, al_osc n2);
al_osc al_osc_add(al_region r, al_osc lhs, al_osc rhs);

#ifdef __cplusplus
}
#endif

#endif
