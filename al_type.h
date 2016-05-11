#ifndef __AL_TYPE_H__
#define __AL_TYPE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "al_region.h"

#define AL_TYPE_I8 'c'
#define AL_TYPE_U8 'C'
#define AL_TYPE_I16 'u'
#define AL_TYPE_U16 'U'
#define AL_TYPE_I32 'i'
#define AL_TYPE_U32 'I'
#define AL_TYPE_I64 'h'
#define AL_TYPE_U64 'H'
#define AL_TYPE_F32 'f'
#define AL_TYPE_F64 'd'
#define AL_TYPE_STR 's'
#define AL_TYPE_SYM 'S'
#define AL_TYPE_TIME 't'
#define AL_TYPE_BLOB 'b'
#define AL_TYPE_TRUE 'T'
#define AL_TYPE_FALSE 'F'
#define AL_TYPE_NIL 'N'
// non-standard
#define AL_TYPE_ALIST 'B'
#define AL_TYPE_FN 'n'

typedef char al_type;
char al_type_findLUB(al_region r, char t1, char t2);
char al_type_isSubtype(al_region r, char t1, char t2);
char *al_type_name(al_region r, char typetag);
void al_type_formatTypeLattice_dot(al_region r);

#define AL_TYPE_ISFLOAT(tt) (tt == AL_TYPE_F32 || tt == AL_TYPE_F64)
#define AL_TYPE_ISINT(tt) (tt == AL_TYPE_I32 || tt == AL_TYPE_U32 || tt == AL_TYPE_I64 || tt == AL_TYPE_U64 || tt == AL_TYPE_I16 || tt == AL_TYPE_U16 || tt == AL_TYPE_I8 || tt == AL_TYPE_U8)
#define AL_TYPE_ISINT64(tt) (tt == AL_TYPE_I64 || tt == AL_TYPE_U64)
#define AL_TYPE_ISINT32(tt) (tt == AL_TYPE_I8 || tt == AL_TYPE_U8 || tt == AL_TYPE_I16 || tt == AL_TYPE_U16 || tt == AL_TYPE_I32 || tt == AL_TYPE_U32 || tt == AL_TYPE_I64 || tt == AL_TYPE_U64)
#define AL_TYPE_ISSIGNED(tt) (tt == AL_TYPE_I8 || tt == AL_TYPE_I16 || tt == AL_TYPE_I32 || tt == AL_TYPE_I64 || tt == AL_TYPE_F32 || tt == AL_TYPE_F64)
#define AL_TYPE_ISBOOL(tt) (tt == 'T' || tt == 'F')
#define AL_TYPE_ISSTRING(tt) (tt == AL_TYPE_STR)
#define AL_TYPE_ISNUMERIC(tt) (tt == AL_TYPE_F32 || tt == AL_TYPE_F64 || tt == 'T' || tt == 'F' ||tt == AL_TYPE_I32 || tt == AL_TYPE_U32 || tt == AL_TYPE_I64 || tt == AL_TYPE_U64 || tt == AL_TYPE_I16 || tt == AL_TYPE_U16 || tt == AL_TYPE_I8 || tt == AL_TYPE_U8)

#ifdef __cplusplus
}
#endif

#endif
