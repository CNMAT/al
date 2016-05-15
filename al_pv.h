#ifndef __AL_PV_H__
#define __AL_PV_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "al_region.h"

//#define AL_PV_NBRANCHES 32
//#define AL_PV_NBITS 5
#define AL_PV_NBRANCHES 8
#define AL_PV_NBITS 3

typedef struct _al_pv_node al_pv_node;


typedef struct _al_pv al_pv;
typedef struct _al_pv2 al_pv2;

extern al_pv *al_pv_empty;
extern al_pv2 *al_pv2_empty;
/*
al_pv *al_pv_alloc(al_region r, void (*freefn)(void *));
//void al_pv_release(al_pv *pvec);
al_pv *al_pv_copy(al_pv *pvec);
al_pv *al_pv_clone(al_pv *pvec);
void *al_pv_nth(al_pv *pvec, int i);
al_pv *al_pv_assocN(al_region r, al_pv *pvec, int i, void *val);
al_pv *al_pv_assocN_m(al_region r, al_pv *pvec, int i, void *val);
al_pv *al_pv_pop(al_region r, al_pv *pvec);
int al_pv_length(al_pv *pvec);
*/
al_pv2 *al_pv2_alloc(al_region r, void (*freefn)(void *));
//void al_pv2_release(al_pv2 *pvec2);
al_pv2 *al_pv2_copy(al_pv2 *pvec2);
al_pv2 *al_pv2_retain(al_pv2 *pvec2);
al_pv2 *al_pv2_clone(al_pv2 *pvec2);
void *al_pv2_nth(al_pv2 *pvec2, int i);
al_pv2 *al_pv2_assocN(al_region r, al_pv2 *pvec2, int i, void *val);
al_pv2 *al_pv2_assocN_m(al_region r, al_pv2 *pvec2, int i, void *val);
al_pv2 *al_pv2_append(al_region r, al_pv2 *pvec2, void *val);
al_pv2 *al_pv2_append_m(al_region r, al_pv2 *pvec2, void *val);
al_pv2 *al_pv2_prepend(al_region r, al_pv2 *pvec2, void *val);
al_pv2 *al_pv2_prepend_m(al_region r, al_pv2 *pvec2, void *val);
al_pv2 *al_pv2_popFirst(al_region r, al_pv2 *pvec2);
al_pv2 *al_pv2_popLast(al_region r, al_pv2 *pvec2);
int al_pv2_length(al_pv2 *pvec2);

void al_pv_print(al_pv *pvec);
void al_pv2_print(al_region r, al_pv2 *pv2);

#ifdef __cplusplus
}
#endif

#endif
