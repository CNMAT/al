#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "al_type.h"

typedef struct _al_type_type {
	char typetag;
	char *name;
	int weight;
	struct _al_type_type **edges;
} al_type_type;

#define AL_TYPE_DEFTYPE(typetag, name, weight, ...)		\
	static al_type_type _al_type_##name = {	\
		typetag,				\
		#name,					\
		weight,					\
		(al_type_type *[]){__VA_ARGS__}	\
	};\
	al_type_type *al_type_##name = &_al_type_##name

AL_TYPE_DEFTYPE(AL_TYPE_STR, string, 17, NULL);
AL_TYPE_DEFTYPE(AL_TYPE_ALIST, alist, 16, &_al_type_string, NULL);
AL_TYPE_DEFTYPE(AL_TYPE_BLOB, blob, 15, &_al_type_string, NULL);
AL_TYPE_DEFTYPE(AL_TYPE_FN, expr, 14, &_al_type_string, NULL);
AL_TYPE_DEFTYPE(AL_TYPE_F64, double, 13, &_al_type_string, NULL);
AL_TYPE_DEFTYPE(AL_TYPE_TIME, time, 12, &_al_type_string, NULL);
AL_TYPE_DEFTYPE(AL_TYPE_U64, uint64, 11, &_al_type_string, NULL);
AL_TYPE_DEFTYPE(AL_TYPE_I64, int64, 10, &_al_type_string, NULL);
AL_TYPE_DEFTYPE(AL_TYPE_U32, uint32, 9, &_al_type_int64, &_al_type_uint64, &_al_type_double, NULL);
AL_TYPE_DEFTYPE(AL_TYPE_F32, float, 8, &_al_type_double, NULL);
AL_TYPE_DEFTYPE(AL_TYPE_I32, int32, 7, &_al_type_double, &_al_type_int64, NULL);
AL_TYPE_DEFTYPE(AL_TYPE_U16, uint16, 6, &_al_type_int32, &_al_type_uint32, &_al_type_float, NULL);
AL_TYPE_DEFTYPE(AL_TYPE_I16, int16, 5, &_al_type_int32, &_al_type_float, NULL);
AL_TYPE_DEFTYPE(AL_TYPE_U8, uint8, 4, &_al_type_int16, &_al_type_uint16, &_al_type_float, NULL);
AL_TYPE_DEFTYPE(AL_TYPE_I8, int8, 3, &_al_type_int16, &_al_type_float, NULL);
AL_TYPE_DEFTYPE(AL_TYPE_TRUE, true, 2, &_al_type_string, NULL);
AL_TYPE_DEFTYPE(AL_TYPE_FALSE, false, 1, &_al_type_string, NULL);
AL_TYPE_DEFTYPE(AL_TYPE_NIL, nil, 0, &_al_type_string, NULL);
AL_TYPE_DEFTYPE(0, bottom, -1, &_al_type_true, &_al_type_false, &_al_type_nil, &_al_type_int8, &_al_type_uint8, &_al_type_float, &_al_type_time, &_al_type_alist, &_al_type_string, &_al_type_blob, &_al_type_expr, NULL);

static al_type_type *al_type_map[128] = {
	&_al_type_bottom, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	// 32
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	// 64
	NULL, NULL, &_al_type_alist, &_al_type_uint8, NULL, NULL, &_al_type_false, NULL,
	&_al_type_uint64, &_al_type_uint32, NULL, NULL, NULL, NULL, &_al_type_nil, NULL,
	NULL, NULL, NULL, NULL, &_al_type_true, &_al_type_uint16, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	// 96
	NULL, NULL, &_al_type_blob, &_al_type_int8, &_al_type_double, NULL, &_al_type_float, NULL,
	&_al_type_int64, &_al_type_int32, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, &_al_type_string, &_al_type_time, &_al_type_int16, NULL, NULL, 
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
};

static al_type_type *_al_type_findLUB_t1(al_region r, al_type_type *t1, al_type_type *t2, al_type_type *lub, int *visited)
{
	if(!t1 || !t2){
		return lub;
	}
	if(visited[t1->weight]){
		return lub;
	}
	visited[t1->weight] = 1;
	if(t1 == t2){
		if(!lub || t1->weight < lub->weight){
			lub = t1;
		}
	}
	{
		int i = 0;
		al_type_type *t1_edge = t1->edges[0];
		while(t1_edge){
			if(t1_edge == t2){
				if(!lub || t1_edge->weight < lub->weight){
					lub = t1_edge;
				}
			}	
			lub = _al_type_findLUB_t1(r, t1_edge, t2, lub, visited);
			i++;
			t1_edge = t1->edges[i];
		}
	}

	return lub;
}

static al_type_type *_al_type_findLUB_t2(al_region r, al_type_type *t1, al_type_type *t2, al_type_type *lub)
{
	if(!t1 || !t2){
		return lub;
	}
	int visited[18];
	memset(visited, '\0', sizeof(int) * 18);
	lub = _al_type_findLUB_t1(r, t1, t2, lub, visited);
	{
		int i = 0;
		al_type_type *t2_edge = t2->edges[0];
		while(t2_edge){
			lub = _al_type_findLUB_t2(r, t1, t2_edge, lub);
			i++;
			t2_edge = t2->edges[i];
		}
	}
	return lub;
}

char al_type_findLUB(al_region r, char t1, char t2)
{
	if(t1 > 127 || t1 < 0 || t2 > 127 || t2 < 0){
		return 0;
	}
	al_type_type *_t1 = al_type_map[(int)t1];
	al_type_type *_t2 = al_type_map[(int)t2];
	al_type_type *lub = NULL;
	lub = _al_type_findLUB_t2(r, _t1, _t2, NULL);
	if(lub){
		return lub->typetag;
	}else{
		return 0;
	}
}

static al_type_type *_al_type_isSubtype(al_region r, al_type_type *t1, al_type_type *t2)
{
	if(!t1 || !t2){
		return NULL;
	}
	if(t1 == t2){
		return t1;
	}
	{
		int i = 0;
		al_type_type *t1_edge = t1->edges[0];
		while(t1_edge){
			if(t1_edge == t2){
				return t2;
			}	
			al_type_type *t = _al_type_isSubtype(r, t1_edge, t2);
			if(t){
				return t;
			}
			i++;
			t1_edge = t1->edges[i];
		}
	}

	return NULL;
}

char al_type_isSubtype(al_region r, char t1, char t2)
{
	if(t1 > 127 || t1 < 0 || t2 > 127 || t2 < 0){
		return 0;
	}
	al_type_type *_t1 = al_type_map[(int)t1];
	al_type_type *_t2 = al_type_map[(int)t2];
	al_type_type *t = _al_type_isSubtype(r, _t1, _t2);
	if(t){
		return t->typetag;
	}else{
		return 0;
	}
}

char *al_type_name(al_region r, char typetag)
{
	if(typetag > 127 || typetag < 0){
		return NULL;
	}
	al_type_type *t = al_type_map[(int)typetag];
	if(t){
		return t->name;
	}
	return NULL;
}

void al_type_formatTypeLattice_dot_r(al_region r, al_type_type *tt, int *visited)//t_osc_hashtab *ht)
{
	if(visited[tt->weight] && tt->weight >= 0){
		return;
	}
	visited[tt->weight] = 1;
	al_type_type **edges = tt->edges;
	al_type_type *edge = *edges;
	int i = 0;
	while(edge){
		printf("%s -> %s;\n", tt->name, edge->name);
		al_type_formatTypeLattice_dot_r(r, edge, visited);
		edge = edges[++i];
	}
}

void al_type_formatTypeLattice_dot(al_region r)
{
	int visited[18];
	memset(visited, '\0', sizeof(int) * 18);
	printf("digraph typelattice {\n");
	printf("rankdir = BT;\n");
	al_type_formatTypeLattice_dot_r(r, al_type_bottom, visited);
	printf("}\n");
}
