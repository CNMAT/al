#ifndef __AL_C_H__
#define __AL_C_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include "al_obj.h"
#include "al_time.h"
#include "al_env.h"

typedef al_obj (*al_c_fnptr)(al_env env, al_obj obj, al_obj context);

typedef struct _al_c_int8
{
	uint32_t error;
	int8_t val;
} al_c_int8;

typedef struct _al_c_uint8
{
	uint32_t error;
	uint8_t val;
} al_c_uint8;

typedef struct _al_c_int16
{
	uint32_t error;
	int16_t val;
} al_c_int16;

typedef struct _al_c_uint16
{
	uint32_t error;
	uint16_t val;
} al_c_uint16;

typedef struct _al_c_int32
{
	uint32_t error;
	int32_t val;
} al_c_int32;

typedef struct _al_c_uint32
{
	uint32_t error;
	uint32_t val;
} al_c_uint32;

typedef struct _al_c_int64
{
	uint32_t error;
	int64_t val;
} al_c_int64;

typedef struct _al_c_uint64
{
	uint32_t error;
	uint64_t val;
} al_c_uint64;

typedef struct _al_c_float
{
	uint32_t error;
	float val;
} al_c_float;

typedef struct _al_c_double
{
	uint32_t error;
	double val;
} al_c_double;

typedef struct _al_c_ptr
{
	uint32_t error;
	char *val;
} al_c_ptr;

typedef struct _al_c_time
{
	uint32_t error;
	al_time val;
} al_c_time;

typedef struct _al_c_fn
{
	uint32_t error;
	al_c_fnptr val;
} al_c_fn;

#define al_c_create(type, val, error) (al_c_##type){error, val}
#define al_c_error(v) (v.error)
#define al_c_value(v) (v.val)



#ifdef __cplusplus
}
#endif

#endif
