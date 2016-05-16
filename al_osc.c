#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include "al_osc.h"
#include "al_type.h"
#include "al_endian.h"
#include "al_strfmt.h"

static size_t al_osc_getPaddedStringLen(char *s)
{
	if(!s){
		return 0;
	}
	size_t n = strlen(s);
	n = (n + 4) & 0xfffffffc;
	return n;
}

static size_t al_osc_getPaddingForNBytes(size_t n)
{
	return (n + 4) & 0xfffffffc;
}

al_osc al_osc_alloc(al_region r, char *address, char type, int32_t num_bytes, int32_t num_padded_bytes, char *bytes)
{
#ifdef AL_HAVE_TIME_FN
	al_osctime time = AL_TIME_FN;
#else
	al_osctime time = AL_OSCTIME_NULL;
#endif
	int addresslen = al_osc_getPaddedStringLen(address);
	int32_t len = AL_OSC_HEADER_SIZE + 4 + addresslen + 4 + num_padded_bytes;
	char *buf = (char *)al_region_getBytes(r, 4 + len);
	memset(buf, 0, len + 4);
	char *p = buf;
	*((int32_t *)p) = hton32(len);
	p += 4;
	strncpy(p, AL_OSC_ID, AL_OSC_ID_SIZE);
	p += AL_OSC_ID_SIZE;
	al_osctime_htont(r, time, p);
	p += AL_OSCTIME_SIZEOF;
	*((int32_t *)p) = hton32(len - AL_OSC_HEADER_SIZE - 4);
	p += 4;
	strcpy(p, address);
	p += addresslen;
	*p++ = ',';
	*p++ = type;
	*p++ = 0;
	*p++ = 0;
	memcpy(p, bytes, num_bytes);
	return (al_osc)buf;
}

al_osc al_osc_copy(al_region r, al_osc n)
{
	if(n){
		int32_t len = ntoh32(*((int32_t *)n));
		char *p = al_region_getBytes(r, len + 4);
		memcpy(p, n, len + 4);
		return p;
	}
	return NULL;
}

size_t al_osc_nformat(al_region r, char *s, size_t n, al_osc x)
{
	switch(al_osc_getType(r, x)){
	case AL_TYPE_I8:
		return al_strfmt_int8(s, n, al_c_value(al_osc_getInt8(r, x)));
	case AL_TYPE_U8:
		return al_strfmt_uint8(s, n, al_c_value(al_osc_getUInt8(r, x)));
	case AL_TYPE_I16:
		return al_strfmt_int16(s, n, al_c_value(al_osc_getInt16(r, x)));
	case AL_TYPE_U16:
		return al_strfmt_uint16(s, n, al_c_value(al_osc_getUInt16(r, x)));
	case AL_TYPE_I32:
		return al_strfmt_int32(s, n, al_c_value(al_osc_getInt32(r, x)));
	case AL_TYPE_U32:
		return al_strfmt_uint32(s, n, al_c_value(al_osc_getUInt32(r, x)));
	case AL_TYPE_I64:
		return al_strfmt_int64(s, n, al_c_value(al_osc_getInt64(r, x)));
	case AL_TYPE_U64:
		return al_strfmt_uint64(s, n, al_c_value(al_osc_getUInt64(r, x)));
	case AL_TYPE_F32:
		return al_strfmt_float(s, n, al_c_value(al_osc_getFloat(r, x)));
	case AL_TYPE_F64:
		return al_strfmt_double(s, n, al_c_value(al_osc_getDouble(r, x)));
	case AL_TYPE_STR:
		return al_strfmt_quotedString(s, n, al_c_value(al_osc_getPtr(r, x)));
	case AL_TYPE_SYM:
		return al_strfmt_stringWithQuotedMeta(s, n, al_c_value(al_osc_getPtr(r, x)));
	case AL_TYPE_BLOB:
		//return al_strfmt_blob(s, n, al_c_value(al_osc_getPtr(r, x)));
		return al_strfmt_stringWithQuotedMeta(s, n, "blob");
	case AL_TYPE_TIME:
		_al_osctime_toISO8601(s, n, al_c_value(al_osc_getTime(r, x)));
	case AL_TYPE_TRUE:
		return al_strfmt_bool(s, n, AL_TYPE_TRUE);
	case AL_TYPE_FALSE:
		return al_strfmt_bool(s, n, AL_TYPE_FALSE);
	case AL_TYPE_NIL:
		return al_strfmt_null(s, n);
	case AL_TYPE_FN:
		return snprintf(s, n, "<native fn : %s, %p>", al_c_value(al_osc_getFnName(r, x)), al_c_value(al_osc_getFn(r, x)));
	default: return 0;
	}
}

al_osctime al_osc_getTimetag(al_region r, al_osc n)
{
	if(n){
		return al_osctime_ntoht(r, ((char *)n) + 4 + AL_OSC_ID_SIZE);
	}else{
		return AL_OSCTIME_NULL;
	}
}

int32_t al_osc_getSize(al_region r, al_osc n)
{
	if(n){
		return ntoh32(*((int32_t *)(((char *)n) + AL_OSC_HEADER_SIZE + 4)));
	}else{
		return 0;
	}
}

char al_osc_getType(al_region r, al_osc n)
{
	if(n){
		char *p = (char *)n;
		p += 4 + AL_OSC_HEADER_SIZE + 4;
		p += al_osc_getPaddedStringLen(p);
		p++;
		return *p;
	}else{
		return 0;
	}
}

char *al_osc_getData(al_region r, al_osc n)
{
	if(n){
		char *p = (char *)n;
		p += 4 + AL_OSC_HEADER_SIZE + 4; // alist size, header, msg size
		p += al_osc_getPaddedStringLen(p); // address 
		p += 4; // type
		return p;
	}else{
		return 0;
	}
}

static size_t al_osc_sizeof(unsigned char typetag, char *data){
	if(typetag > 127){
		return -1;
	}
	if(!data){
		return -1;
	}
	switch(typetag){
	case AL_TYPE_I8:
	case AL_TYPE_U8:
	case AL_TYPE_I16:
	case AL_TYPE_U16:
	case AL_TYPE_I32:
	case AL_TYPE_U32:
	case AL_TYPE_F32:
		return 4;
	case AL_TYPE_I64:
	case AL_TYPE_U64:
	case AL_TYPE_F64:
		return 8;
	case AL_TYPE_BLOB:
		return ntoh32(*((int32_t *)data)) + 4;
	case AL_TYPE_STR:
	case AL_TYPE_SYM:
		return al_osc_getPaddedStringLen(data);
	case AL_TYPE_TIME:
		return AL_OSCTIME_SIZEOF;
	default: return 0;
	}
}

al_osc al_osc_int8(al_region r, int8_t i)
{
	int32_t ii = hton32((int32_t)i);
	return al_osc_alloc(r, "/o", AL_TYPE_I8, 4, 4, (char *)&ii);
}

al_osc al_osc_uint8(al_region r, uint8_t i)
{
	int32_t ii = hton32((int32_t)i);
	return al_osc_alloc(r, "/o", AL_TYPE_U8, 4, 4, (char *)&ii);
}

al_osc al_osc_int16(al_region r, int16_t i)
{
	int32_t ii = hton32((int32_t)i);
	return al_osc_alloc(r, "/o", AL_TYPE_I16, 4, 4, (char *)&ii);
}

al_osc al_osc_uint16(al_region r, uint16_t i)
{
	int32_t ii = hton32((int32_t)i);
	return al_osc_alloc(r, "/o", AL_TYPE_U16, 4, 4, (char *)&ii);
}

al_osc al_osc_int32(al_region r, int32_t i)
{
	int32_t ii = hton32(i);
	return al_osc_alloc(r, "/o", AL_TYPE_I32, 4, 4, (char *)&ii);
}

al_osc al_osc_uint32(al_region r, uint32_t i)
{
	int32_t ii = hton32(i);
	return al_osc_alloc(r, "/o", AL_TYPE_U32, 4, 4, (char *)&ii);
}

al_osc al_osc_int64(al_region r, int64_t i)
{
	int64_t ii = hton64(i);
	al_osc xx = al_osc_alloc(r, "/o", AL_TYPE_I64, 8, 8, (char *)&ii);
	return xx;
}

al_osc al_osc_uint64(al_region r, uint64_t i)
{
	int64_t ii = hton64(i);
	return al_osc_alloc(r, "/o", AL_TYPE_U64, 8, 8, (char *)&ii);
}

al_osc al_osc_float(al_region r, float f)
{
	int32_t ii = hton32(*((int32_t *)&f));
	return al_osc_alloc(r, "/o", AL_TYPE_F32, 4, 4, (char *)&ii);
}

al_osc al_osc_double(al_region r, double f)
{
	int64_t ii = hton64(*((int64_t *)&f));
	return al_osc_alloc(r, "/o", AL_TYPE_F64, 8, 8, (char *)&ii);
}

al_osc al_osc_string(al_region r, char *s)
{
	return al_osc_alloc(r, "/o", AL_TYPE_STR, strlen(s), al_osc_getPaddedStringLen(s), s);
}

al_osc al_osc_symbol(al_region r, char *s)
{
	return al_osc_alloc(r, "/o", AL_TYPE_SYM, strlen(s), al_osc_getPaddedStringLen(s), s);
}

al_osc al_osc_time(al_region r, al_osctime t)
{
	char buf[AL_OSCTIME_SIZEOF];
	al_osctime_htont(r, t, buf);
	return al_osc_alloc(r, "/o", AL_TYPE_TIME, AL_OSCTIME_SIZEOF, AL_OSCTIME_SIZEOF, buf);
}

al_osc al_osc_blob(al_region r, int32_t num_bytes, char *b)
{
	return al_osc_alloc(r, "/o", AL_TYPE_BLOB, num_bytes, al_osc_getPaddingForNBytes(num_bytes), b);
}

al_osc al_osc_unit(al_region r, char type)
{
	return al_osc_alloc(r, "/o", type, 0, 0, NULL);
}

al_osc al_osc_fn(al_region r, al_c_fnptr fn, char *name)
{
#ifdef AL_HAVE_TIME_FN
	al_osctime time = AL_TIME_FN;
#else
	al_osctime time = AL_OSCTIME_NULL;
#endif
	char *address = "/o";
	int addresslen = al_osc_getPaddedStringLen(address);
	int namelen = al_osc_getPaddedStringLen(name);
	int32_t len = AL_OSC_HEADER_SIZE + 4 + addresslen + 4 + sizeof(intptr_t) + namelen;
	char *buf = (char *)al_region_getBytes(r, 4 + len);
	memset(buf, 0, len + 4);
	char *p = buf;
	*((int32_t *)p) = hton32(len);
	p += 4;
	strncpy(p, AL_OSC_ID, AL_OSC_ID_SIZE);
	p += AL_OSC_ID_SIZE;
	al_osctime_htont(r, time, p);
	p += AL_OSCTIME_SIZEOF;
	*((int32_t *)p) = hton32(len - AL_OSC_HEADER_SIZE - 4);
	p += 4;
	strcpy(p, address);
	p += addresslen;
	*p++ = ',';
	*p++ = AL_TYPE_FN;
	*p++ = AL_TYPE_SYM;
	*p++ = 0;
	*((intptr_t *)p) = (intptr_t)fn;
	p += sizeof(intptr_t);
	memcpy(p, name, strlen(name));
	return (al_osc)buf;
}

al_c_int8 al_osc_getInt8(al_region r, al_osc v)
{
	if(v){
		char tt = al_osc_getType(r, v);
		char *data = al_osc_getData(r, v);
		if(al_osc_sizeof(tt, data) >= 4){
			return al_c_create(int8, (int8_t)ntoh32(*((int32_t *)data)), 0);
		}
	}
	return al_c_create(int8, 0, 1);
}

al_c_uint8 al_osc_getUInt8(al_region r, al_osc v)
{
	if(v){
		char tt = al_osc_getType(r, v);
		char *data = al_osc_getData(r, v);
		if(al_osc_sizeof(tt, data) >= 4){
			return al_c_create(uint8, (uint8_t)ntoh32(*((int32_t *)data)), 0);
		}
	}
	return al_c_create(uint8, 0, 1);
}

al_c_int16 al_osc_getInt16(al_region r, al_osc v)
{
	if(v){
		char tt = al_osc_getType(r, v);
		char *data = al_osc_getData(r, v);
		if(al_osc_sizeof(tt, data) >= 4){
			return al_c_create(int16, (int16_t)ntoh32(*((int32_t *)data)), 0);
		}
	}
	return al_c_create(int16, 0, 1);
}

al_c_uint16 al_osc_getUInt16(al_region r, al_osc v)
{
	if(v){
		char tt = al_osc_getType(r, v);
		char *data = al_osc_getData(r, v);
		if(al_osc_sizeof(tt, data) >= 4){
			return al_c_create(uint16, (uint16_t)ntoh32(*((int32_t *)data)), 0);
		}
	}
	return al_c_create(uint16, 0, 1);
}

al_c_int32 al_osc_getInt32(al_region r, al_osc v)
{
	if(v){
		char tt = al_osc_getType(r, v);
		char *data = al_osc_getData(r, v);
		if(al_osc_sizeof(tt, data) >= 4){
			return al_c_create(int32, (int32_t)ntoh32(*((int32_t *)data)), 0);
		}
	}
	return al_c_create(int32, 0, 1);
}

al_c_uint32 al_osc_getUInt32(al_region r, al_osc v)
{
	if(v){
		char tt = al_osc_getType(r, v);
		char *data = al_osc_getData(r, v);
		if(al_osc_sizeof(tt, data) >= 4){
			return al_c_create(uint32, (uint32_t)ntoh32(*((int32_t *)data)), 0);
		}
	}
	return al_c_create(uint32, 0, 1);
}

al_c_int64 al_osc_getInt64(al_region r, al_osc v)
{
	if(v){
		char tt = al_osc_getType(r, v);
		char *data = al_osc_getData(r, v);
		if(al_osc_sizeof(tt, data) >= 8){
			return al_c_create(int64, (int64_t)ntoh64(*((int64_t *)data)), 0);
		}
	}
	return al_c_create(int64, 0, 1);
}

al_c_uint64 al_osc_getUInt64(al_region r, al_osc v)
{
	if(v){
		char tt = al_osc_getType(r, v);
		char *data = al_osc_getData(r, v);
		if(al_osc_sizeof(tt, data) >= 8){
			return al_c_create(uint64, (uint64_t)ntoh64(*((int64_t *)data)), 0);
		}
	}
	return al_c_create(uint64, 0, 1);
}

al_c_float al_osc_getFloat(al_region r, al_osc v)
{
	if(v){
		char tt = al_osc_getType(r, v);
		char *data = al_osc_getData(r, v);
		if(al_osc_sizeof(tt, data) >= 4){
			int32_t i = ntoh32(*((int32_t *)data));
			return al_c_create(float, *((float *)&i), 0);
		}
	}
	return al_c_create(float, 0., 1);
}

al_c_double al_osc_getDouble(al_region r, al_osc v)
{
	if(v){
		char tt = al_osc_getType(r, v);
		char *data = al_osc_getData(r, v);
		if(al_osc_sizeof(tt, data) >= 8){
			int64_t i = ntoh64(*((int64_t *)data));
			return al_c_create(double, *((double *)&i), 0);
		}
	}
	return al_c_create(double, 0., 1);
}

al_c_ptr al_osc_getPtr(al_region r, al_osc v)
{
	if(v){
		char tt = al_osc_getType(r, v);
		char *data = al_osc_getData(r, v);
		if(al_osc_sizeof(tt, data) > 0){
			return al_c_create(ptr, al_osc_getData(r, v), 0);
		}
	}
	return al_c_create(ptr, NULL, 1);
}

al_c_time al_osc_getTime(al_region r, al_osc v)
{
	if(v){
		char tt = al_osc_getType(r, v);
		char *data = al_osc_getData(r, v);
		if(al_osc_sizeof(tt, data) >= 8){
			return al_c_create(time, al_osctime_ntoht(r, al_osc_getData(r, v)), 0);
		}
	}
	return al_c_create(time, AL_OSCTIME_NULL, 1);
}

al_c_fn al_osc_getFn(al_region r, al_osc v)
{
	if(v && al_osc_getType(r, v) == AL_TYPE_FN){
		char *data = al_osc_getData(r, v);
		return al_c_create(fn, (al_c_fnptr)*((intptr_t *)data), 0);
	}
	return al_c_create(fn, NULL, 1);	
}

al_c_ptr al_osc_getFnName(al_region r, al_osc v)
{
	if(v && al_osc_getType(r, v) == AL_TYPE_FN){
		return al_c_create(ptr, al_osc_getData(r, v) + sizeof(intptr_t), 0);
	}
	return al_c_create(ptr, NULL, 1);	
}

al_osc al_osc_convert(al_region r, al_osc n, char newtype)
{
	char tt = al_osc_getType(r, n);
	switch(tt){
	case AL_TYPE_I8:
		switch(newtype){
		case AL_TYPE_I8:
			return al_osc_int8(r, (int8_t)al_c_value(al_osc_getInt8(r, n)));
		case AL_TYPE_U8:
			return al_osc_uint8(r, (uint8_t)al_c_value(al_osc_getInt8(r, n)));
		case AL_TYPE_I16:
			return al_osc_int16(r, (int16_t)al_c_value(al_osc_getInt8(r, n)));
		case AL_TYPE_U16:
			return al_osc_uint16(r, (uint16_t)al_c_value(al_osc_getInt8(r, n)));
		case AL_TYPE_I32:
			return al_osc_int32(r, (int32_t)al_c_value(al_osc_getInt8(r, n)));
		case AL_TYPE_U32:
			return al_osc_uint32(r, (uint32_t)al_c_value(al_osc_getInt8(r, n)));
		case AL_TYPE_I64:
			return al_osc_int64(r, (int64_t)al_c_value(al_osc_getInt8(r, n)));
		case AL_TYPE_U64:
			return al_osc_uint64(r, (uint64_t)al_c_value(al_osc_getInt8(r, n)));
		case AL_TYPE_F32:
			return al_osc_float(r, (float)al_c_value(al_osc_getInt8(r, n)));
		case AL_TYPE_F64:
			return al_osc_double(r, (double)al_c_value(al_osc_getInt8(r, n)));
		case AL_TYPE_STR:
			{
				size_t len = al_strfmt_int8(NULL, 0, al_c_value(al_osc_getInt8(r, n)));
				char *s = al_region_getBytes(r, len + 1);
				al_strfmt_int8(s, len + 1, al_c_value(al_osc_getInt8(r, n)));
				return al_osc_string(r, s);
			}
		case AL_TYPE_SYM:
			{
				size_t len = al_strfmt_int8(NULL, 0, al_c_value(al_osc_getInt8(r, n)));
				char *s = al_region_getBytes(r, len + 1);
				al_strfmt_int8(s, len + 1, al_c_value(al_osc_getInt8(r, n)));
				return al_osc_symbol(r, s);
			}
		case AL_TYPE_TIME:
			return al_osc_time(r, al_osctime_fromFloat(r, (double)al_c_value(al_osc_getInt8(r, n))));
		case AL_TYPE_BLOB:
			return al_osc_blob(r, al_osc_getSize(r, n), (char *)(n + 4));
		case AL_TYPE_TRUE:
			return al_osc_unit(r, AL_TYPE_TRUE);
		case AL_TYPE_FALSE:
			return al_osc_unit(r, AL_TYPE_FALSE);
		case AL_TYPE_NIL:
			return al_osc_unit(r, AL_TYPE_NIL);
		case AL_TYPE_ALIST:
			return n;
		}
	case AL_TYPE_U8:
		switch(newtype){
		case AL_TYPE_I8:
			return al_osc_int8(r, (int8_t)al_c_value(al_osc_getUInt8(r, n)));
		case AL_TYPE_U8:
			return al_osc_uint8(r, (uint8_t)al_c_value(al_osc_getUInt8(r, n)));
		case AL_TYPE_I16:
			return al_osc_int16(r, (int16_t)al_c_value(al_osc_getUInt8(r, n)));
		case AL_TYPE_U16:
			return al_osc_uint16(r, (uint16_t)al_c_value(al_osc_getUInt8(r, n)));
		case AL_TYPE_I32:
			return al_osc_int32(r, (int32_t)al_c_value(al_osc_getUInt8(r, n)));
		case AL_TYPE_U32:
			return al_osc_uint32(r, (uint32_t)al_c_value(al_osc_getUInt8(r, n)));
		case AL_TYPE_I64:
			return al_osc_int64(r, (int64_t)al_c_value(al_osc_getUInt8(r, n)));
		case AL_TYPE_U64:
			return al_osc_uint64(r, (uint64_t)al_c_value(al_osc_getUInt8(r, n)));
		case AL_TYPE_F32:
			return al_osc_float(r, (float)al_c_value(al_osc_getUInt8(r, n)));
		case AL_TYPE_F64:
			return al_osc_double(r, (double)al_c_value(al_osc_getUInt8(r, n)));
		case AL_TYPE_STR:
			{
				size_t len = al_strfmt_uint8(NULL, 0, al_c_value(al_osc_getUInt8(r, n)));
				char *s = al_region_getBytes(r, len + 1);
				al_strfmt_uint8(s, len + 1, al_c_value(al_osc_getUInt8(r, n)));
				return al_osc_string(r, s);
			}
		case AL_TYPE_SYM:
			{
				size_t len = al_strfmt_uint8(NULL, 0, al_c_value(al_osc_getUInt8(r, n)));
				char *s = al_region_getBytes(r, len + 1);
				al_strfmt_uint8(s, len + 1, al_c_value(al_osc_getUInt8(r, n)));
				return al_osc_symbol(r, s);
			}
		case AL_TYPE_TIME:
			return al_osc_time(r, al_osctime_fromFloat(r, (double)al_c_value(al_osc_getUInt8(r, n))));
		case AL_TYPE_BLOB:
			return al_osc_blob(r, al_osc_getSize(r, n), (char *)(n + 4));
		case AL_TYPE_TRUE:
			return al_osc_unit(r, AL_TYPE_TRUE);
		case AL_TYPE_FALSE:
			return al_osc_unit(r, AL_TYPE_FALSE);
		case AL_TYPE_NIL:
			return al_osc_unit(r, AL_TYPE_NIL);
		case AL_TYPE_ALIST:
			return n;
		}
	case AL_TYPE_I16:
		switch(newtype){
		case AL_TYPE_I8:
			return al_osc_int8(r, (int8_t)al_c_value(al_osc_getInt16(r, n)));
		case AL_TYPE_U8:
			return al_osc_uint8(r, (uint8_t)al_c_value(al_osc_getInt16(r, n)));
		case AL_TYPE_I16:
			return al_osc_int16(r, (int16_t)al_c_value(al_osc_getInt16(r, n)));
		case AL_TYPE_U16:
			return al_osc_uint16(r, (uint16_t)al_c_value(al_osc_getInt16(r, n)));
		case AL_TYPE_I32:
			return al_osc_int32(r, (int32_t)al_c_value(al_osc_getInt16(r, n)));
		case AL_TYPE_U32:
			return al_osc_uint32(r, (uint32_t)al_c_value(al_osc_getInt16(r, n)));
		case AL_TYPE_I64:
			return al_osc_int64(r, (int64_t)al_c_value(al_osc_getInt16(r, n)));
		case AL_TYPE_U64:
			return al_osc_uint64(r, (uint64_t)al_c_value(al_osc_getInt16(r, n)));
		case AL_TYPE_F32:
			return al_osc_float(r, (float)al_c_value(al_osc_getInt16(r, n)));
		case AL_TYPE_F64:
			return al_osc_double(r, (double)al_c_value(al_osc_getInt16(r, n)));
		case AL_TYPE_STR:
			{
				size_t len = al_strfmt_int16(NULL, 0, al_c_value(al_osc_getInt16(r, n)));
				char *s = al_region_getBytes(r, len + 1);
				al_strfmt_int16(s, len + 1, al_c_value(al_osc_getInt16(r, n)));
				return al_osc_string(r, s);
			}
		case AL_TYPE_SYM:
			{
				size_t len = al_strfmt_int16(NULL, 0, al_c_value(al_osc_getInt16(r, n)));
				char *s = al_region_getBytes(r, len + 1);
				al_strfmt_int16(s, len + 1, al_c_value(al_osc_getInt16(r, n)));
				return al_osc_symbol(r, s);
			}
		case AL_TYPE_TIME:
			return al_osc_time(r, al_osctime_fromFloat(r, (double)al_c_value(al_osc_getInt16(r, n))));
		case AL_TYPE_BLOB:
			return al_osc_blob(r, al_osc_getSize(r, n), (char *)(n + 4));
		case AL_TYPE_TRUE:
			return al_osc_unit(r, AL_TYPE_TRUE);
		case AL_TYPE_FALSE:
			return al_osc_unit(r, AL_TYPE_FALSE);
		case AL_TYPE_NIL:
			return al_osc_unit(r, AL_TYPE_NIL);
		case AL_TYPE_ALIST:
			return n;
		}
	case AL_TYPE_U16:
		switch(newtype){
		case AL_TYPE_I8:
			return al_osc_int8(r, (int8_t)al_c_value(al_osc_getUInt16(r, n)));
		case AL_TYPE_U8:
			return al_osc_uint8(r, (uint8_t)al_c_value(al_osc_getUInt16(r, n)));
		case AL_TYPE_I16:
			return al_osc_int16(r, (int16_t)al_c_value(al_osc_getUInt16(r, n)));
		case AL_TYPE_U16:
			return al_osc_uint16(r, (uint16_t)al_c_value(al_osc_getUInt16(r, n)));
		case AL_TYPE_I32:
			return al_osc_int32(r, (int32_t)al_c_value(al_osc_getUInt16(r, n)));
		case AL_TYPE_U32:
			return al_osc_uint32(r, (uint32_t)al_c_value(al_osc_getUInt16(r, n)));
		case AL_TYPE_I64:
			return al_osc_int64(r, (int64_t)al_c_value(al_osc_getUInt16(r, n)));
		case AL_TYPE_U64:
			return al_osc_uint64(r, (uint64_t)al_c_value(al_osc_getUInt16(r, n)));
		case AL_TYPE_F32:
			return al_osc_float(r, (float)al_c_value(al_osc_getUInt16(r, n)));
		case AL_TYPE_F64:
			return al_osc_double(r, (double)al_c_value(al_osc_getUInt16(r, n)));
		case AL_TYPE_STR:
			{
				size_t len = al_strfmt_uint16(NULL, 0, al_c_value(al_osc_getUInt16(r, n)));
				char *s = al_region_getBytes(r, len + 1);
				al_strfmt_uint16(s, len + 1, al_c_value(al_osc_getUInt16(r, n)));
				return al_osc_string(r, s);
			}
		case AL_TYPE_SYM:
			{
				size_t len = al_strfmt_uint16(NULL, 0, al_c_value(al_osc_getUInt16(r, n)));
				char *s = al_region_getBytes(r, len + 1);
				al_strfmt_uint16(s, len + 1, al_c_value(al_osc_getUInt16(r, n)));
				return al_osc_symbol(r, s);
			}
		case AL_TYPE_TIME:
			return al_osc_time(r, al_osctime_fromFloat(r, (double)al_c_value(al_osc_getUInt16(r, n))));
		case AL_TYPE_BLOB:
			return al_osc_blob(r, al_osc_getSize(r, n), (char *)(n + 4));
		case AL_TYPE_TRUE:
			return al_osc_unit(r, AL_TYPE_TRUE);
		case AL_TYPE_FALSE:
			return al_osc_unit(r, AL_TYPE_FALSE);
		case AL_TYPE_NIL:
			return al_osc_unit(r, AL_TYPE_NIL);
		case AL_TYPE_ALIST:
			return n;
		}
	case AL_TYPE_I32:
		switch(newtype){
		case AL_TYPE_I8:
			return al_osc_int8(r, (int8_t)al_c_value(al_osc_getInt32(r, n)));
		case AL_TYPE_U8:
			return al_osc_uint8(r, (uint8_t)al_c_value(al_osc_getInt32(r, n)));
		case AL_TYPE_I16:
			return al_osc_int16(r, (int16_t)al_c_value(al_osc_getInt32(r, n)));
		case AL_TYPE_U16:
			return al_osc_uint16(r, (uint16_t)al_c_value(al_osc_getInt32(r, n)));
		case AL_TYPE_I32:
			return al_osc_int32(r, (int32_t)al_c_value(al_osc_getInt32(r, n)));
		case AL_TYPE_U32:
			return al_osc_uint32(r, (uint32_t)al_c_value(al_osc_getInt32(r, n)));
		case AL_TYPE_I64:
			return al_osc_int64(r, (int64_t)al_c_value(al_osc_getInt32(r, n)));
		case AL_TYPE_U64:
			return al_osc_uint64(r, (uint64_t)al_c_value(al_osc_getInt32(r, n)));
		case AL_TYPE_F32:
			return al_osc_float(r, (float)al_c_value(al_osc_getInt32(r, n)));
		case AL_TYPE_F64:
			return al_osc_double(r, (double)al_c_value(al_osc_getInt32(r, n)));
		case AL_TYPE_STR:
			{
				size_t len = al_strfmt_int32(NULL, 0, al_c_value(al_osc_getInt32(r, n)));
				char *s = al_region_getBytes(r, len + 1);
				al_strfmt_int32(s, len + 1, al_c_value(al_osc_getInt32(r, n)));
				return al_osc_string(r, s);
			}
		case AL_TYPE_SYM:
			{
				size_t len = al_strfmt_int32(NULL, 0, al_c_value(al_osc_getInt32(r, n)));
				char *s = al_region_getBytes(r, len + 1);
				al_strfmt_int32(s, len + 1, al_c_value(al_osc_getInt32(r, n)));
				return al_osc_symbol(r, s);
			}
		case AL_TYPE_TIME:
			return al_osc_time(r, al_osctime_fromFloat(r, (double)al_c_value(al_osc_getInt32(r, n))));
		case AL_TYPE_BLOB:
			return al_osc_blob(r, al_osc_getSize(r, n), (char *)(n + 4));
		case AL_TYPE_TRUE:
			return al_osc_unit(r, AL_TYPE_TRUE);
		case AL_TYPE_FALSE:
			return al_osc_unit(r, AL_TYPE_FALSE);
		case AL_TYPE_NIL:
			return al_osc_unit(r, AL_TYPE_NIL);
		case AL_TYPE_ALIST:
			return n;
		}
	case AL_TYPE_U32:
		switch(newtype){
		case AL_TYPE_I8:
			return al_osc_int8(r, (int8_t)al_c_value(al_osc_getUInt32(r, n)));
		case AL_TYPE_U8:
			return al_osc_uint8(r, (uint8_t)al_c_value(al_osc_getUInt32(r, n)));
		case AL_TYPE_I16:
			return al_osc_int16(r, (int16_t)al_c_value(al_osc_getUInt32(r, n)));
		case AL_TYPE_U16:
			return al_osc_uint16(r, (uint16_t)al_c_value(al_osc_getUInt32(r, n)));
		case AL_TYPE_I32:
			return al_osc_int32(r, (int32_t)al_c_value(al_osc_getUInt32(r, n)));
		case AL_TYPE_U32:
			return al_osc_uint32(r, (uint32_t)al_c_value(al_osc_getUInt32(r, n)));
		case AL_TYPE_I64:
			return al_osc_int64(r, (int64_t)al_c_value(al_osc_getUInt32(r, n)));
		case AL_TYPE_U64:
			return al_osc_uint64(r, (uint64_t)al_c_value(al_osc_getUInt32(r, n)));
		case AL_TYPE_F32:
			return al_osc_float(r, (float)al_c_value(al_osc_getUInt32(r, n)));
		case AL_TYPE_F64:
			return al_osc_double(r, (double)al_c_value(al_osc_getUInt32(r, n)));
		case AL_TYPE_STR:
			{
				size_t len = al_strfmt_uint32(NULL, 0, al_c_value(al_osc_getUInt32(r, n)));
				char *s = al_region_getBytes(r, len + 1);
				al_strfmt_uint32(s, len + 1, al_c_value(al_osc_getUInt32(r, n)));
				return al_osc_string(r, s);
			}
		case AL_TYPE_SYM:
			{
				size_t len = al_strfmt_uint32(NULL, 0, al_c_value(al_osc_getUInt32(r, n)));
				char *s = al_region_getBytes(r, len + 1);
				al_strfmt_uint32(s, len + 1, al_c_value(al_osc_getUInt32(r, n)));
				return al_osc_symbol(r, s);
			}
		case AL_TYPE_TIME:
			return al_osc_time(r, al_osctime_fromFloat(r, (double)al_c_value(al_osc_getUInt32(r, n))));
		case AL_TYPE_BLOB:
			return al_osc_blob(r, al_osc_getSize(r, n), (char *)(n + 4));
		case AL_TYPE_TRUE:
			return al_osc_unit(r, AL_TYPE_TRUE);
		case AL_TYPE_FALSE:
			return al_osc_unit(r, AL_TYPE_FALSE);
		case AL_TYPE_NIL:
			return al_osc_unit(r, AL_TYPE_NIL);
		case AL_TYPE_ALIST:
			return n;
		}
	case AL_TYPE_I64:
		switch(newtype){
		case AL_TYPE_I8:
			return al_osc_int8(r, (int8_t)al_c_value(al_osc_getInt64(r, n)));
		case AL_TYPE_U8:
			return al_osc_uint8(r, (uint8_t)al_c_value(al_osc_getInt64(r, n)));
		case AL_TYPE_I16:
			return al_osc_int16(r, (int16_t)al_c_value(al_osc_getInt64(r, n)));
		case AL_TYPE_U16:
			return al_osc_uint16(r, (uint16_t)al_c_value(al_osc_getInt64(r, n)));
		case AL_TYPE_I32:
			return al_osc_int32(r, (int32_t)al_c_value(al_osc_getInt64(r, n)));
		case AL_TYPE_U32:
			return al_osc_uint32(r, (uint32_t)al_c_value(al_osc_getInt64(r, n)));
		case AL_TYPE_I64:
			return al_osc_int64(r, (int64_t)al_c_value(al_osc_getInt64(r, n)));
		case AL_TYPE_U64:
			return al_osc_uint64(r, (uint64_t)al_c_value(al_osc_getInt64(r, n)));
		case AL_TYPE_F32:
			return al_osc_float(r, (float)al_c_value(al_osc_getInt64(r, n)));
		case AL_TYPE_F64:
			return al_osc_double(r, (double)al_c_value(al_osc_getInt64(r, n)));
		case AL_TYPE_STR:
			{
				size_t len = al_strfmt_int64(NULL, 0, al_c_value(al_osc_getInt64(r, n)));
				char *s = al_region_getBytes(r, len + 1);
				al_strfmt_int64(s, len + 1, al_c_value(al_osc_getInt64(r, n)));
				return al_osc_string(r, s);
			}
		case AL_TYPE_SYM:
			{
				size_t len = al_strfmt_int64(NULL, 0, al_c_value(al_osc_getInt64(r, n)));
				char *s = al_region_getBytes(r, len + 1);
				al_strfmt_int64(s, len + 1, al_c_value(al_osc_getInt64(r, n)));
				return al_osc_symbol(r, s);
			}
		case AL_TYPE_TIME:
			return al_osc_time(r, al_osctime_fromFloat(r, (double)al_c_value(al_osc_getInt64(r, n))));
		case AL_TYPE_BLOB:
			return al_osc_blob(r, al_osc_getSize(r, n), (char *)(n + 4));
		case AL_TYPE_TRUE:
			return al_osc_unit(r, AL_TYPE_TRUE);
		case AL_TYPE_FALSE:
			return al_osc_unit(r, AL_TYPE_FALSE);
		case AL_TYPE_NIL:
			return al_osc_unit(r, AL_TYPE_NIL);
		case AL_TYPE_ALIST:
			return n;
		}
	case AL_TYPE_U64:
		switch(newtype){
		case AL_TYPE_I8:
			return al_osc_int8(r, (int8_t)al_c_value(al_osc_getUInt64(r, n)));
		case AL_TYPE_U8:
			return al_osc_uint8(r, (uint8_t)al_c_value(al_osc_getUInt64(r, n)));
		case AL_TYPE_I16:
			return al_osc_int16(r, (int16_t)al_c_value(al_osc_getUInt64(r, n)));
		case AL_TYPE_U16:
			return al_osc_uint16(r, (uint16_t)al_c_value(al_osc_getUInt64(r, n)));
		case AL_TYPE_I32:
			return al_osc_int32(r, (int32_t)al_c_value(al_osc_getUInt64(r, n)));
		case AL_TYPE_U32:
			return al_osc_uint32(r, (uint32_t)al_c_value(al_osc_getUInt64(r, n)));
		case AL_TYPE_I64:
			return al_osc_int64(r, (int64_t)al_c_value(al_osc_getUInt64(r, n)));
		case AL_TYPE_U64:
			return al_osc_uint64(r, (uint64_t)al_c_value(al_osc_getUInt64(r, n)));
		case AL_TYPE_F32:
			return al_osc_float(r, (float)al_c_value(al_osc_getUInt64(r, n)));
		case AL_TYPE_F64:
			return al_osc_double(r, (double)al_c_value(al_osc_getUInt64(r, n)));
		case AL_TYPE_STR:
			{
				size_t len = al_strfmt_uint64(NULL, 0, al_c_value(al_osc_getUInt64(r, n)));
				char *s = al_region_getBytes(r, len + 1);
				al_strfmt_uint64(s, len + 1, al_c_value(al_osc_getUInt64(r, n)));
				return al_osc_string(r, s);
			}
		case AL_TYPE_SYM:
			{
				size_t len = al_strfmt_uint64(NULL, 0, al_c_value(al_osc_getUInt64(r, n)));
				char *s = al_region_getBytes(r, len + 1);
				al_strfmt_uint64(s, len + 1, al_c_value(al_osc_getUInt64(r, n)));
				return al_osc_symbol(r, s);
			}
		case AL_TYPE_TIME:
			return al_osc_time(r, al_osctime_fromFloat(r, (double)al_c_value(al_osc_getUInt64(r, n))));
		case AL_TYPE_BLOB:
			return al_osc_blob(r, al_osc_getSize(r, n), (char *)(n + 4));
		case AL_TYPE_TRUE:
			return al_osc_unit(r, AL_TYPE_TRUE);
		case AL_TYPE_FALSE:
			return al_osc_unit(r, AL_TYPE_FALSE);
		case AL_TYPE_NIL:
			return al_osc_unit(r, AL_TYPE_NIL);
		case AL_TYPE_ALIST:
			return n;
		}
	case AL_TYPE_STR:
		switch(newtype){
		case AL_TYPE_I8:
			return al_osc_int8(r, 0);
		case AL_TYPE_U8:
			return al_osc_uint8(r, 0);
		case AL_TYPE_I16:
			return al_osc_int16(r, 0);
		case AL_TYPE_U16:
			return al_osc_uint16(r, 0);
		case AL_TYPE_I32:
			return al_osc_int32(r, 0);
		case AL_TYPE_U32:
			return al_osc_uint32(r, 0);
		case AL_TYPE_I64:
			return al_osc_int64(r, 0);
		case AL_TYPE_U64:
			return al_osc_uint64(r, 0);
		case AL_TYPE_F32:
			return al_osc_float(r, 0);
		case AL_TYPE_F64:
			return al_osc_double(r, 0);
		case AL_TYPE_STR:
			return n;
		case AL_TYPE_SYM:
			return al_osc_symbol(r, al_c_value(al_osc_getPtr(r, n)));
		case AL_TYPE_TIME:
			return al_osc_time(r, AL_OSCTIME_NULL);
		case AL_TYPE_BLOB:
			return al_osc_blob(r, al_osc_getSize(r, n), (char *)(n + 4));
		case AL_TYPE_TRUE:
			return al_osc_unit(r, AL_TYPE_TRUE);
		case AL_TYPE_FALSE:
			return al_osc_unit(r, AL_TYPE_FALSE);
		case AL_TYPE_NIL:
			return al_osc_unit(r, AL_TYPE_NIL);
		case AL_TYPE_ALIST:
			return n;
		}
	case AL_TYPE_SYM:
		switch(newtype){
		case AL_TYPE_I8:
			return al_osc_int8(r, 0);
		case AL_TYPE_U8:
			return al_osc_uint8(r, 0);
		case AL_TYPE_I16:
			return al_osc_int16(r, 0);
		case AL_TYPE_U16:
			return al_osc_uint16(r, 0);
		case AL_TYPE_I32:
			return al_osc_int32(r, 0);
		case AL_TYPE_U32:
			return al_osc_uint32(r, 0);
		case AL_TYPE_I64:
			return al_osc_int64(r, 0);
		case AL_TYPE_U64:
			return al_osc_uint64(r, 0);
		case AL_TYPE_F32:
			return al_osc_float(r, 0);
		case AL_TYPE_F64:
			return al_osc_double(r, 0);
		case AL_TYPE_STR:
			return al_osc_string(r, al_c_value(al_osc_getPtr(r, n)));
		case AL_TYPE_SYM:
			return n;
		case AL_TYPE_TIME:
			return al_osc_time(r, AL_OSCTIME_NULL);
		case AL_TYPE_BLOB:
			return al_osc_blob(r, al_osc_getSize(r, n), (char *)(n + 4));
		case AL_TYPE_TRUE:
			return al_osc_unit(r, AL_TYPE_TRUE);
		case AL_TYPE_FALSE:
			return al_osc_unit(r, AL_TYPE_FALSE);
		case AL_TYPE_NIL:
			return al_osc_unit(r, AL_TYPE_NIL);
		case AL_TYPE_ALIST:
			return n;
		}
	case AL_TYPE_TIME:
		switch(newtype){
		case AL_TYPE_I8:
			return al_osc_int8(r, (int8_t)al_osctime_toFloat(r, al_c_value(al_osc_getTime(r, n))));
		case AL_TYPE_U8:
			return al_osc_uint8(r, (uint8_t)al_osctime_toFloat(r, al_c_value(al_osc_getTime(r, n))));
		case AL_TYPE_I16:
			return al_osc_int16(r, (int16_t)al_osctime_toFloat(r, al_c_value(al_osc_getTime(r, n))));
		case AL_TYPE_U16:
			return al_osc_uint16(r, (uint16_t)al_osctime_toFloat(r, al_c_value(al_osc_getTime(r, n))));
		case AL_TYPE_I32:
			return al_osc_int32(r, (int32_t)al_osctime_toFloat(r, al_c_value(al_osc_getTime(r, n))));
		case AL_TYPE_U32:
			return al_osc_uint32(r, (uint32_t)al_osctime_toFloat(r, al_c_value(al_osc_getTime(r, n))));
		case AL_TYPE_I64:
			return al_osc_int64(r, (int64_t)al_osctime_toFloat(r, al_c_value(al_osc_getTime(r, n))));
		case AL_TYPE_U64:
			return al_osc_uint64(r, (uint64_t)al_osctime_toFloat(r, al_c_value(al_osc_getTime(r, n))));
		case AL_TYPE_F32:
			return al_osc_float(r, (float)al_osctime_toFloat(r, al_c_value(al_osc_getTime(r, n))));
		case AL_TYPE_F64:
			return al_osc_double(r, (double)al_osctime_toFloat(r, al_c_value(al_osc_getTime(r, n))));
		case AL_TYPE_STR:
			{
				size_t len = _al_osctime_toISO8601(NULL, 0, al_c_value(al_osc_getTime(r, n)));
				char *s = al_region_getBytes(r, len + 1);
				_al_osctime_toISO8601(s, len + 1, al_c_value(al_osc_getTime(r, n)));
				return al_osc_string(r, s);
			}
		case AL_TYPE_SYM:
			{
				size_t len = _al_osctime_toISO8601(NULL, 0, al_c_value(al_osc_getTime(r, n)));
				char *s = al_region_getBytes(r, len + 1);
				_al_osctime_toISO8601(s, len + 1, al_c_value(al_osc_getTime(r, n)));
				return al_osc_symbol(r, s);
			}
		case AL_TYPE_TIME:
			return al_osc_time(r, al_osctime_fromFloat(r, (double)al_osctime_toFloat(r, al_c_value(al_osc_getTime(r, n)))));
		case AL_TYPE_BLOB:
			return al_osc_blob(r, al_osc_getSize(r, n), (char *)(n + 4));
		case AL_TYPE_TRUE:
			return al_osc_unit(r, AL_TYPE_TRUE);
		case AL_TYPE_FALSE:
			return al_osc_unit(r, AL_TYPE_FALSE);
		case AL_TYPE_NIL:
			return al_osc_unit(r, AL_TYPE_FALSE);
		case AL_TYPE_ALIST:
			return n;
		}
	case AL_TYPE_BLOB:
		switch(newtype){
		case AL_TYPE_I8:
			return al_osc_int8(r, 0);
		case AL_TYPE_U8:
			return al_osc_uint8(r, 0);
		case AL_TYPE_I16:
			return al_osc_int16(r, 0);
		case AL_TYPE_U16:
			return al_osc_uint16(r, 0);
		case AL_TYPE_I32:
			return al_osc_int32(r, 0);
		case AL_TYPE_U32:
			return al_osc_uint32(r, 0);
		case AL_TYPE_I64:
			return al_osc_int64(r, 0);
		case AL_TYPE_U64:
			return al_osc_uint64(r, 0);
		case AL_TYPE_F32:
			return al_osc_float(r, 0);
		case AL_TYPE_F64:
			return al_osc_double(r, 0);
		case AL_TYPE_STR:
			return al_osc_string(r, "");
		case AL_TYPE_SYM:
			return al_osc_symbol(r, "");
		case AL_TYPE_TIME:
			return al_osc_time(r, AL_OSCTIME_NULL);				
		case AL_TYPE_BLOB:
			return n;
		case AL_TYPE_TRUE:
			return al_osc_unit(r, AL_TYPE_TRUE);
		case AL_TYPE_FALSE:
			return al_osc_unit(r, AL_TYPE_FALSE);
		case AL_TYPE_NIL:
			return al_osc_unit(r, AL_TYPE_NIL);
		case AL_TYPE_ALIST:
			return n;
		}
	case AL_TYPE_TRUE:
	case AL_TYPE_FALSE:
	case AL_TYPE_NIL:
		switch(newtype){
		case AL_TYPE_I8:
			return al_osc_int8(r, 0);
		case AL_TYPE_U8:
			return al_osc_uint8(r, 0);
		case AL_TYPE_I16:
			return al_osc_int16(r, 0);
		case AL_TYPE_U16:
			return al_osc_uint16(r, 0);
		case AL_TYPE_I32:
			return al_osc_int32(r, 0);
		case AL_TYPE_U32:
			return al_osc_uint32(r, 0);
		case AL_TYPE_I64:
			return al_osc_int64(r, 0);
		case AL_TYPE_U64:
			return al_osc_uint64(r, 0);
		case AL_TYPE_F32:
			return al_osc_float(r, 0);
		case AL_TYPE_F64:
			return al_osc_double(r, 0);
		case AL_TYPE_STR:
			return al_osc_string(r, "");
		case AL_TYPE_SYM:
			return al_osc_symbol(r, "");
		case AL_TYPE_TIME:
			return al_osc_time(r, AL_OSCTIME_NULL);
		case AL_TYPE_BLOB:
			return al_osc_blob(r, al_osc_getSize(r, n), (char *)(n + 4));
		case AL_TYPE_TRUE:
			return al_osc_unit(r, AL_TYPE_TRUE);
		case AL_TYPE_FALSE:
			return al_osc_unit(r, AL_TYPE_FALSE);
		case AL_TYPE_NIL:
			return al_osc_unit(r, AL_TYPE_NIL);
		case AL_TYPE_ALIST:
			return n;
		}
	case AL_TYPE_ALIST:
		switch(newtype){
		case AL_TYPE_I8:
			return al_osc_int8(r, 0);
		case AL_TYPE_U8:
			return al_osc_uint8(r, 0);
		case AL_TYPE_I16:
			return al_osc_int16(r, 0);
		case AL_TYPE_U16:
			return al_osc_uint16(r, 0);
		case AL_TYPE_I32:
			return al_osc_int32(r, 0);
		case AL_TYPE_U32:
			return al_osc_uint32(r, 0);
		case AL_TYPE_I64:
			return al_osc_int64(r, 0);
		case AL_TYPE_U64:
			return al_osc_uint64(r, 0);
		case AL_TYPE_F32:
			return al_osc_float(r, 0);
		case AL_TYPE_F64:
			return al_osc_double(r, 0);
		case AL_TYPE_STR:
			return al_osc_string(r, "");
		case AL_TYPE_SYM:
			return al_osc_symbol(r, "");
		case AL_TYPE_TIME:
			return al_osc_time(r, AL_OSCTIME_NULL);
		case AL_TYPE_BLOB:
			return al_osc_blob(r, al_osc_getSize(r, n), (char *)(n + 4));
		case AL_TYPE_TRUE:
			return al_osc_unit(r, AL_TYPE_TRUE);
		case AL_TYPE_FALSE:
			return al_osc_unit(r, AL_TYPE_FALSE);
		case AL_TYPE_NIL:
			return al_osc_unit(r, AL_TYPE_NIL);
		case AL_TYPE_ALIST:
			return n;
		}
	}
	return n;
}

int al_osc_eql(al_region r, al_osc n1, al_osc n2)
{
	if(n1 && n2){
		char *_n1 = (char *)n1;
		char *_n2 = (char *)n2;
		int32_t l1 = ntoh32(*((int32_t *)_n1));
		int32_t l2 = ntoh32(*((int32_t *)_n2));
		if(l1 == l2){
			return memcmp(_n1 + 20, _n2 + 20, l1 - 16) == 0;
		}
	}
	return 0;
}

int al_osc_eqv(al_region r, al_osc n1, al_osc n2)
{
	char tt1 = al_osc_getType(r, n1);
	char tt2 = al_osc_getType(r, n2);
	char lub = al_type_findLUB(r, tt1, tt2);
	al_osc _n1, _n2;
	if(tt1 == tt2){
		_n1 = n1;
		_n2 = n2;
	}else if(tt1 != lub && tt2 == lub){
		_n1 = al_osc_convert(r, n1, lub);
		_n2 = n2;
	}else if(tt1 == lub && tt2 != lub){
		_n1 = n1;
		_n2 = al_osc_convert(r, n2, lub);
	}else{
		_n1 = al_osc_convert(r, n1, lub);
		_n2 = al_osc_convert(r, n2, lub);
	}
	int res = al_osc_eql(r, _n1, _n2);
	return res;
}

al_osc al_osc_add(al_region r, al_osc lhs, al_osc rhs)
{
	char tt = al_osc_getType(r, lhs);
	if(AL_TYPE_ISNUMERIC(tt)){
		if(AL_TYPE_ISINT32(tt)){
			al_c_int32 ll = al_osc_getInt32(r, lhs);
			al_c_int32 rr = al_osc_getInt32(r, rhs);
			if(!al_c_error(ll) && !al_c_error(rr)){
				return al_osc_int32(r, al_c_value(ll) + al_c_value(rr));
			}
		}else if(AL_TYPE_ISINT64(tt)){
			al_c_int64 ll = al_osc_getInt64(r, lhs);
			al_c_int64 rr = al_osc_getInt64(r, rhs);
			if(!al_c_error(ll) && !al_c_error(rr)){
				return al_osc_int64(r, al_c_value(ll) + al_c_value(rr));
			}
		}else if(tt == AL_TYPE_F32){
			al_c_float ll = al_osc_getFloat(r, lhs);
			al_c_float rr = al_osc_getFloat(r, rhs);
			if(!al_c_error(ll) && !al_c_error(rr)){
				return al_osc_float(r, al_c_value(ll) + al_c_value(rr));
			}
		}else if(tt == AL_TYPE_F64){
			al_c_double ll = al_osc_getDouble(r, lhs);
			al_c_double rr = al_osc_getDouble(r, rhs);
			if(!al_c_error(ll) && !al_c_error(rr)){
				return al_osc_double(r, al_c_value(ll) + al_c_value(rr));
			}
		}
	}
	return al_osc_int32(r, 0);
}
