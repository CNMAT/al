#include "al_pv.h"
#include <string.h>
#include <stdio.h>
#include <inttypes.h>

int ntabs;
char tabs[128];
int al_pv_length(al_pv *pvec);

//#define P(fmt, ...) if(al_pv_debug) printf("%s%s:%d: "fmt"\n", tabs, __func__, __LINE__, ##__VA_ARGS__);
#define P(fmt, ...) {}

//#define AL_PV_ENTER if(al_pv_debug){printf("%s%s {\n", tabs, __func__); tabs[ntabs++] = '\t';}; OSC_PROFILE_TIMER_START(t1);
#define AL_PV_ENTER

//#define AL_PV_RETURN if(al_pv_debug){OSC_PROFILE_TIMER_STOP(t1); OSC_PROFILE_TIMER_PRINTF(t1); tabs[--ntabs] = '\0'; printf("%s}\n", tabs);}; return;
//#define AL_PV_RETURN_OBJ(obj, ...) if(al_pv_debug){OSC_PROFILE_TIMER_STOP(t1); OSC_PROFILE_TIMER_PRINTF(t1); __VA_ARGS__(obj); tabs[--ntabs] = '\0'; printf("%s}\n", tabs);}; return obj;
#define AL_PV_RETURN return;
#define AL_PV_RETURN_OBJ(obj, ...) return obj;

#define AL_PV_INCRC(obj) (obj ? ++(*((int32_t *)(obj))) : -1); if(obj && al_pv_debug) P("INC REFCOUNT for %p from %d to %d\n", obj, (*((int32_t *)(obj))) - 1, (*((int32_t *)(obj))));
#define AL_PV_DECRC(obj) (obj ? --(*((int32_t *)(obj))) : -1); if(obj && al_pv_debug) P("DEC REFCOUNT for %p from %d to %d\n", obj, (*((int32_t *)(obj))) + 1, (*((int32_t *)(obj))));
#define AL_PV_GETRC(obj) (obj ? (*((int32_t *)(obj))) : -1)

#define AL_PV2_INCRC(obj) (obj ? ++(*((int32_t *)(obj))) : -1); if(obj && al_pv_debug) P("INC REFCOUNT for %p from %d to %d\n", obj, (*((int32_t *)(obj))) - 1, (*((int32_t *)(obj))));
#define AL_PV2_DECRC(obj) (obj ? --(*((int32_t *)(obj))) : -1); if(obj && al_pv_debug) P("DEC REFCOUNT for %p from %d to %d\n", obj, (*((int32_t *)(obj))) + 1, (*((int32_t *)(obj))));
#define AL_PV2_GETRC(obj) (obj ? (*((int32_t *)(obj))) : -1)

int al_pv_debug = 1;

void al_pv_printRefCount(void *obj)
{
	if(obj){
		printf("%srefcount for %p = %d\n", tabs, obj, AL_PV_GETRC(obj));
	}
}


#define AL_PV_MASK AL_PV_NBRANCHES - 1

#pragma pack(push)
#pragma pack(4)
typedef struct _al_pv_ud
{
	int32_t refcount;
	void *data;
} al_pv_ud;
#pragma pack(pop)

al_pv_ud *al_pv_ud_alloc(al_region r, void *userdata);
al_pv_ud *al_pv_ud_copy(al_pv_ud *ud);
void al_pv_ud_free(al_pv_ud *ud, void (*freefn)(void *));

al_pv_ud *al_pv_ud_alloc(al_region r, void *userdata)
{
	AL_PV_ENTER;
	al_pv_ud *ud = al_region_getBytes(r, sizeof(al_pv_ud));
	if(ud){
		ud->data = userdata;
		ud->refcount = 1;
	}
	AL_PV_RETURN_OBJ(ud, al_pv_printRefCount);
}

al_pv_ud *al_pv_ud_copy(al_pv_ud *ud)
{
	AL_PV_ENTER;
	if(ud){
		AL_PV_INCRC(ud);
	}
	AL_PV_RETURN_OBJ(ud);
}

void *al_pv_ud_getData(al_pv_ud *ud)
{
	AL_PV_ENTER;
	if(ud){
		AL_PV_RETURN_OBJ(ud->data);
	}
	AL_PV_RETURN_OBJ(NULL);
}
/*
void al_pv_ud_free(al_pv_ud *ud, void (*freefn)(void *))
{
	AL_PV_ENTER;
	if(ud){
		if(AL_PV_GETRC(ud) < 0){
			AL_PV_RETURN;
		}
		int rc = AL_PV_DECRC(ud);
		if(rc == 0){
			if(freefn){
				freefn(al_pv_ud_getData(ud));
			}
			memset(ud, 0, sizeof(al_pv_ud));
			AL_PV_DECRC(ud); // set to -1
			osc_mem_free(ud);
		}		
	}
	AL_PV_RETURN;
}
*/
#pragma pack(push)
#pragma pack(4)
struct _al_pv_node
{
	int32_t refcount;
	void *array[AL_PV_NBRANCHES];
};
#pragma pack(pop)

//static al_pv_node _al_pv_node_empty;
//static al_pv_node *al_pv_node_empty = &_al_pv_node_empty;
al_pv_node *al_pv_node_allocWithArray(al_region r, int length, void **array);
al_pv_node *al_pv_node_clone(al_region r, al_pv_node *n);
al_pv_node *al_pv_node_copy(al_pv_node *n);
void **al_pv_node_getArray(al_pv_node *node);

al_pv_node *al_pv_node_alloc(al_region r)
{
	AL_PV_ENTER;
	al_pv_node *n = al_pv_node_allocWithArray(r, 0, NULL);
	AL_PV_RETURN_OBJ(n, al_pv_printRefCount);
}

al_pv_node *al_pv_node_allocWithArray(al_region r, int length, void **array)
{
	AL_PV_ENTER
	al_pv_node *node = al_region_getBytes(r, sizeof(al_pv_node));
	memset(node, 0, sizeof(al_pv_node));
	if(array){
		memcpy(node->array, array, length * sizeof(void *));
		//}else{
		//memset(node->array, 0, length * sizeof(void *));
	}
	AL_PV_INCRC(node);
	AL_PV_RETURN_OBJ(node);
}

al_pv_node *al_pv_node_clone(al_region r, al_pv_node *n)
{
	AL_PV_ENTER
	al_pv_node *copy = al_pv_node_copy(n);
	al_pv_node *clone = al_pv_node_allocWithArray(r, AL_PV_NBRANCHES, al_pv_node_getArray(copy));
	AL_PV_DECRC(copy);
	AL_PV_RETURN_OBJ(clone);
}

al_pv_node *al_pv_node_copy(al_pv_node *n)
{
	AL_PV_ENTER;
	if(!n){
		AL_PV_RETURN_OBJ(NULL);
	}
	AL_PV_INCRC(n);
	void **array = al_pv_node_getArray(n);
	for(int i = 0; i < AL_PV_NBRANCHES; i++){
		P("%d: %p", i, array[i]);
		AL_PV_INCRC(array[i]);
	}
	AL_PV_RETURN_OBJ(n);
}
/*
void al_pv_node_free(al_pv_node *node, int level, void (*freefn)(void *))
{
	AL_PV_ENTER
	if(node){
		if(AL_PV_GETRC(node) < 0){
			AL_PV_RETURN;
		}
		P("%p, rc = %d", node, AL_PV_GETRC(node));
		int rc = AL_PV_DECRC(node);
		if(rc == 0){
			void **array = al_pv_node_getArray(node);
			if(level == 0){
				for(int i = 0; i < AL_PV_NBRANCHES; i++){
					al_pv_ud_free((al_pv_ud *)array[i], freefn);
				}				
				memset(node, 0, sizeof(al_pv_ud));
				AL_PV_DECRC(node); // set to -1
			}else{
				for(int i = 0; i < AL_PV_NBRANCHES; i++){
					al_pv_node_free((al_pv_node *)array[i], level - AL_PV_NBITS, freefn);
				}
				memset(node, 0, sizeof(al_pv_node));
				AL_PV_DECRC(node); // set to -1
			}
			osc_mem_free(node);
		}
	}
	AL_PV_RETURN;
}
*/

void **al_pv_node_getArray(al_pv_node *node)
{
	AL_PV_ENTER
	if(node){
		AL_PV_RETURN_OBJ(node->array);
	}
	AL_PV_RETURN_OBJ(NULL);
}

#pragma pack(push)
#pragma pack(4)
struct _al_pv
{
	int32_t refcount;
	int count;
	int shift;
	al_pv_node *root;
	int taillength;
	void **tail;
	void (*freefn)(void *);
};

struct _al_pv2
{
	int32_t refcount;
	int length;
	struct _al_pv *head;
	struct _al_pv *tail;
};
#pragma pack(pop)

//al_pv _al_pv_empty = {-1, 0, AL_PV_NBITS, &_al_pv_node_empty, 0, NULL, NULL};
//al_pv *al_pv_empty = &_al_pv_empty;

al_pv_node *al_pv_doAssoc(al_region r, al_pv *pvec, int level, al_pv_node *node, int i, void *val);
al_pv *al_pv_cons(al_region r, al_pv *pvec, void *val);
al_pv_node *al_pv_pushTail(al_region r, al_pv *pvec, int level, al_pv_node *parent, al_pv_node *tailnode);
al_pv_node *al_pv_newPath(al_region r, int level, al_pv_node *node);
al_pv_node *al_pv_popTail(al_region r, al_pv *pvec, int level, al_pv_node *node);
void al_pv_setCount(al_pv *pvec, int count);
int al_pv_getShift(al_pv *pvec);
void al_pv_setShift(al_pv *pvec, int shift);
al_pv_node *al_pv_getRoot(al_pv *pvec);
void al_pv_setRoot(al_pv *pvec, al_pv_node *root);
int al_pv_getTailLength(al_pv *pvec);
void al_pv_setTailLength(al_pv *pvec, int taillength);
void **al_pv_getTail(al_pv *pvec);
void al_pv_setTail(al_pv *pvec, void **tail);
void (*al_pv_getFreeFn(al_pv *pvec))(void *);
void **al_pv_cloneTail(al_region r, int len, void **array);
void **al_pv_copyTail(int len, void **array);

//al_pv_node *al_pv_newPath_m(int level, al_pv_node *node);
al_pv_node *al_pv_doAssoc_m(al_region r, al_pv *pvec, int level, al_pv_node *node, int i, void *val);
al_pv *al_pv_cons_m(al_region r, al_pv *pvec, void *val);
al_pv_node *al_pv_pushTail_m(al_region r, al_pv *pvec, int level, al_pv_node *parent, al_pv_node *tailnode);
al_pv_node *al_pv_newPath_m(al_region r, int level, al_pv_node *node);

al_pv *_al_pv_alloc(al_region r, int count, int shift, al_pv_node *root, int taillength, void **tail, void (*freefn)(void *))
{
	AL_PV_ENTER
	al_pv *pvec = al_region_getBytes(r, sizeof(al_pv));
	pvec->count = count;
	pvec->shift = shift;
	pvec->root = root;
	pvec->taillength = taillength;
	pvec->tail = tail;//al_pv_copyTail(taillength, tail);
	pvec->refcount = 1;
	pvec->freefn = freefn;
	AL_PV_RETURN_OBJ(pvec, al_pv_printRefCount);
}

al_pv *al_pv_alloc(al_region r, void (*freefn)(void *))
{
	AL_PV_ENTER
	al_pv *pvec = _al_pv_alloc(r, 0, AL_PV_NBITS, al_pv_node_alloc(r), 0, NULL, freefn);
	AL_PV_RETURN_OBJ(pvec, al_pv_printRefCount);
}
/*
void al_pv_release(al_pv *pvec)
{
	return;
	AL_PV_ENTER
	if(pvec){
		P("%s: rc = %d", __func__, AL_PV_GETRC(pvec));
		if(AL_PV_GETRC(pvec) < 0){
			AL_PV_RETURN;
		}
		int rc = AL_PV_DECRC(pvec);
		if(rc == 0){
			P("count = %d", al_pv_length(pvec));
			P("freeing pvec %p", pvec);
			void (*freefn)(void *) = al_pv_getFreeFn(pvec);
			P("freeing root %p", al_pv_getRoot(pvec));
			al_pv_node_free(al_pv_getRoot(pvec), al_pv_getShift(pvec), freefn);
			P("done freeing root");
			int taillength = al_pv_getTailLength(pvec);
			void **tail = al_pv_getTail(pvec);
			P("freeing tail %p", tail);
			if(tail && taillength > 0){
				for(int i = 0; i < taillength; i++){
					P("%d: freeing %p", i, tail[i]);
					al_pv_ud_free(tail[i], freefn);
					P("done");
				}
				memset(tail, 0, taillength * sizeof(void *));
				osc_mem_free(tail);
			}
			P("done freeing tail");
			memset(pvec, 0, sizeof(al_pv));
			AL_PV_DECRC(pvec); // set to -1
			osc_mem_free(pvec);
		}
	}
	AL_PV_RETURN;
}
*/
al_pv *al_pv_copy(al_pv *pvec)
{
	AL_PV_ENTER
	if(pvec){
		AL_PV_INCRC(pvec);
		AL_PV_RETURN_OBJ(pvec);
	}
	AL_PV_RETURN_OBJ(NULL);
}

al_pv *al_pv_clone(al_pv *pvec)
{
	AL_PV_ENTER
	AL_PV_RETURN_OBJ(NULL);
}

void **al_pv_cloneTail(al_region r, int len, void **tail)
{
	AL_PV_ENTER
	// just increments the refcounts
	void **copy = al_pv_copyTail(len, tail);
	// now make a copy and return
	void **newtail = (void **)al_region_getBytes(r, len * sizeof(void *));
	memcpy(newtail, copy, len * sizeof(void *));
	AL_PV_RETURN_OBJ(newtail);
}

void **al_pv_copyTail(int len, void **tail)
{
	AL_PV_ENTER
	if(tail){
		for(int i = 0; i < len; i++){
			AL_PV_INCRC(tail[i]);
		}
	}
	AL_PV_RETURN_OBJ(tail);
}

// The 0th element of the tail is what index?
int al_pv_tailOffset(al_pv *pvec)
{
	AL_PV_ENTER
	if(al_pv_length(pvec) < AL_PV_NBRANCHES){
		AL_PV_RETURN_OBJ(0);
	}
	int to = ((al_pv_length(pvec) - 1) >> AL_PV_NBITS) << AL_PV_NBITS;
	AL_PV_RETURN_OBJ(to);
}

// return the array that contains the ith element
void **al_pv_getArrayFor(al_pv *pvec, int i)
{
	AL_PV_ENTER
	if(i >= 0 && i < al_pv_length(pvec)){
		if(i >= al_pv_tailOffset(pvec)){
			P("%d is in the tail", i);
			AL_PV_RETURN_OBJ(al_pv_getTail(pvec));
		}else{
			al_pv_node *node = al_pv_getRoot(pvec);
			for(int level = al_pv_getShift(pvec); level > 0; level -= AL_PV_NBITS){
				P("level = %d, node = %p", level, node);
				node = al_pv_node_getArray(node)[(i >> level) & AL_PV_MASK];
			}
			P("level = %d, node = %p", 0, node);
			AL_PV_RETURN_OBJ(al_pv_node_getArray(node));
		}
	}
	AL_PV_RETURN_OBJ(NULL);
}

// get the ith element
void *al_pv_nth(al_pv *pvec, int i)
{
	AL_PV_ENTER
	void **node = al_pv_getArrayFor(pvec, i);
	if(node){
		P("found %p", node[i & AL_PV_MASK]);
		al_pv_ud *ud = node[i & AL_PV_MASK];
		if(ud){
			AL_PV_RETURN_OBJ(al_pv_ud_getData(ud));
		}else{
			AL_PV_RETURN_OBJ(NULL);
		}
	}else{
		P("found nothing");
		AL_PV_RETURN_OBJ(NULL);
	}
}

// stick val in the ith slot. list must currently be i - 1 elements long.
al_pv *al_pv_assocN(al_region r, al_pv *pvec, int i, void *val)
{
	AL_PV_ENTER
	int count = al_pv_length(pvec);
	int shift = al_pv_getShift(pvec);
	al_pv_node *root = al_pv_getRoot(pvec);
	int taillength = al_pv_getTailLength(pvec);
	void **tail = al_pv_getTail(pvec);
	void (*freefn)(void *) = al_pv_getFreeFn(pvec);
	{
		al_pv_ud *ud = al_pv_ud_alloc(r, val);
		if(i >= 0 && i < al_pv_length(pvec)){
			P("i(%d) is less than count(%d)", i, count);
			if(i >= al_pv_tailOffset(pvec)){
				P("%d is in the tail", i);
				
				void **newtail = (void **)al_region_getBytes(r, taillength * sizeof(void *));
				memcpy(newtail, tail, taillength * sizeof(void *));
				for(int j = 0; j < taillength; j++){
					AL_PV_INCRC(newtail[j]);
				}
				al_pv_ud *old = newtail[i & AL_PV_MASK];
				if(old){
					//al_pv_ud_free(old, freefn);
				}
				newtail[i & AL_PV_MASK] = ud;
				
				//void **newtail = al_pv_insertIntoTail(taillength, tail, i & AL_PV_MASK, ud);
				al_pv *ret = _al_pv_alloc(r, count, shift, al_pv_node_copy(root), taillength, newtail, freefn);
				AL_PV_RETURN_OBJ(ret);
			}
			P("i(%d) is not in the tail", i);
			al_pv *pv = _al_pv_alloc(r, count, shift, al_pv_doAssoc(r, pvec, shift, al_pv_node_copy(root), i, ud), taillength, tail, freefn);
			AL_PV_RETURN_OBJ(pv);
		}
		if(i == al_pv_length(pvec)){
			P("i(%d) == count(%d)", i, al_pv_length(pvec));
			al_pv *pv = al_pv_cons(r, pvec, ud);
			AL_PV_RETURN_OBJ(pv)
		}
		P("i(%d) is out of bounds (%d)", i, al_pv_length(pvec));
	}
	AL_PV_RETURN_OBJ(NULL);
}

al_pv *al_pv_assocN_m(al_region r, al_pv *pvec, int i, void *val)
{
	AL_PV_ENTER;
	/*
	if(AL_PV_GETRC(pvec) != 1){
		al_pv *pv = al_pv_assocN(r, pvec, i, val);
		al_pv_release(pvec);
		AL_PV_RETURN_OBJ(pv);
	}
	*/
	int shift = al_pv_getShift(pvec);
	al_pv_node *root = al_pv_getRoot(pvec);
	void **tail = al_pv_getTail(pvec);
	void (*freefn)(void *) = al_pv_getFreeFn(pvec);
	{
		al_pv_ud *ud = al_pv_ud_alloc(r, val);
		if(i >= 0 && i < al_pv_length(pvec)){
			P("i(%d) is less than count(%d)", i, al_pv_length(pvec));
			if(i >= al_pv_tailOffset(pvec)){
				P("%d is in the tail", i);

				void **newtail = tail;
				//void **newtail = (void **)al_region_getBytes(r, taillength * sizeof(void *));
				//memcpy(newtail, tail, taillength * sizeof(void *));
				//for(int j = 0; j < taillength; j++){
				//AL_PV_INCRC(newtail[j]);
				//}
				al_pv_ud *old = newtail[i & AL_PV_MASK];
				if(old){
					//al_pv_ud_free(old, freefn);
				}
				newtail[i & AL_PV_MASK] = ud;
				
				//al_pv *ret = _al_pv_alloc(count, shift, al_pv_node_copy(root), taillength, newtail, freefn);
				//AL_PV_RETURN_OBJ(ret);
				AL_PV_RETURN_OBJ(pvec);
			}
			P("i(%d) is not in the tail", i);
			//al_pv *pv = _al_pv_alloc(count, shift, al_pv_doAssoc_m(r, pvec, shift, al_pv_node_copy(root), i, ud), taillength, tail, freefn);
			//AL_PV_RETURN_OBJ(pv);
			al_pv_setRoot(pvec, al_pv_doAssoc_m(r, pvec, shift, root, i, ud));
			AL_PV_RETURN_OBJ(pvec);
		}
		if(i == al_pv_length(pvec)){
			P("i(%d) == count(%d)", i, al_pv_length(pvec));
			al_pv *pv = al_pv_cons_m(r, pvec, ud);
			AL_PV_RETURN_OBJ(pv);
		}
		P("i(%d) is out of bounds (%d)", i, al_pv_length(pvec));
	}
	AL_PV_RETURN_OBJ(NULL);
}

// perform the insertion. if level is 0, insert into the newly allocated node's array.
// if level is > 0, the newly allocated node becomes an internal node and we recursively
// call this function to continue creating internal nodes until we get to level 0
al_pv_node *al_pv_doAssoc(al_region r, al_pv *pvec, int level, al_pv_node *node, int i, void *val)
{
	AL_PV_ENTER
	void **clone = (void **)al_region_getBytes(r, AL_PV_NBRANCHES * sizeof(void *));
	memcpy(clone, al_pv_node_getArray(node), AL_PV_NBRANCHES * sizeof(void *));
	al_pv_node *ret = al_pv_node_allocWithArray(r, AL_PV_NBRANCHES, clone);
	P("level = %d", level);
	if(level == 0){
		al_pv_ud **array = (al_pv_ud **)al_pv_node_getArray(ret);
		al_pv_ud *ud = array[i & AL_PV_MASK];
		if(ud){
			//al_pv_ud_free(ud, al_pv_getFreeFn(pvec));
		}
		array[i & AL_PV_MASK] = val;
	}else{
		int subidx = (i >> level) & AL_PV_MASK;
		al_pv_node_getArray(ret)[subidx] = al_pv_doAssoc(r, pvec, level - AL_PV_NBITS, (al_pv_node *)al_pv_node_getArray(node)[subidx], i, val);
	}
	AL_PV_RETURN_OBJ(ret);
}

al_pv_node *al_pv_doAssoc_m(al_region r, al_pv *pvec, int level, al_pv_node *node, int i, void *val)
{
	AL_PV_ENTER;
	/*
	if(AL_PV_GETRC(pvec) != 1){
		al_pv_node *n = al_pv_doAssoc(r, pvec, level, node, i, val);
		al_pv_node_free(node, level, al_pv_getFreeFn(pvec));
		AL_PV_RETURN_OBJ(n);
	}
	*/
	//void **clone = (void **)al_region_getBytes(r, AL_PV_NBRANCHES * sizeof(void *));
	//memcpy(clone, al_pv_node_getArray(node), AL_PV_NBRANCHES * sizeof(void *));
	//al_pv_node *ret = al_pv_node_allocWithArray(AL_PV_NBRANCHES, clone);
	al_pv_node *ret = node;
	P("level = %d", level);
	if(level == 0){
		al_pv_ud **array = (al_pv_ud **)al_pv_node_getArray(ret);
		al_pv_ud *ud = array[i & AL_PV_MASK];
		if(ud){
			//al_pv_ud_free(ud, al_pv_getFreeFn(pvec));
		}
		array[i & AL_PV_MASK] = val;
	}else{
		int subidx = (i >> level) & AL_PV_MASK;
		al_pv_node_getArray(ret)[subidx] = al_pv_doAssoc_m(r, pvec, level - AL_PV_NBITS, (al_pv_node *)al_pv_node_getArray(node)[subidx], i, val);
	}
	AL_PV_RETURN_OBJ(ret);
}

// Append val to the end of the list
al_pv *al_pv_cons(al_region r, al_pv *pvec, void *val)
{
	AL_PV_ENTER
	if(!pvec){
		AL_PV_RETURN_OBJ(NULL);
	}
	int count = al_pv_length(pvec);
	int shift = al_pv_getShift(pvec);
	al_pv_node *root = al_pv_getRoot(pvec);
	int taillength = al_pv_getTailLength(pvec);
	void **tail = al_pv_getTail(pvec);
	void (*freefn)(void *) = al_pv_getFreeFn(pvec);
	if(count - al_pv_tailOffset(pvec) < AL_PV_NBRANCHES){
		P("there's room in the tail");
		void **newtail = (void **)al_region_getBytes(r, (taillength + 1) * sizeof(void *));
		if(tail){
			memcpy(newtail, tail, (taillength + 1) * sizeof(void *));
		}else{
			memset(newtail, 0, (taillength + 1) * sizeof(void *));
		}
		for(int i = 0; i < taillength; i++){
			AL_PV_INCRC(newtail[i]);
		}
		P("inserting %p into slot %d", val, taillength);
		newtail[taillength] = val;
		AL_PV_INCRC(root);
	        al_pv *pv = _al_pv_alloc(r, count + 1, shift, root, taillength + 1, newtail, freefn);
		AL_PV_RETURN_OBJ(pv);
	}
	P("no room in the tail");
	al_pv_node *newroot;
	void **tailclone = al_pv_cloneTail(r, taillength, tail);
	al_pv_node *tailnode = al_pv_node_allocWithArray(r, taillength, tailclone);
	//////////////////////////////////////////////////
	// this line was commented out when region support was added
	//////////////////////////////////////////////////
	//osc_mem_free(tailclone); 
	int newshift = shift;
	if((count >> AL_PV_NBITS) > (1 << shift)){
		P("no room at this level: construct a new path");
		newroot = al_pv_node_alloc(r);
		AL_PV_INCRC(root);
		al_pv_node_getArray(newroot)[0] = root;//al_pv_node_copy(root);
		al_pv_node_getArray(newroot)[1] = al_pv_newPath(r, shift, tailnode);
		newshift += AL_PV_NBITS;
	}else{
		P("room at the current level: push tail");
		//newroot = al_pv_node_copy(al_pv_pushTail(pvec, shift, root, tailnode));
		newroot = al_pv_pushTail(r, pvec, shift, root, tailnode);
		AL_PV_INCRC(newroot);
	}
	void **array = (void **)al_region_getBytes(r, AL_PV_NBRANCHES * sizeof(void *));
	memset(array, 0, AL_PV_NBRANCHES * sizeof(void *));
	array[0] = val;
	al_pv *pv = _al_pv_alloc(r, count + 1, newshift, newroot, 1, array, freefn);
	AL_PV_RETURN_OBJ(pv);
}

al_pv *al_pv_cons_m(al_region r, al_pv *pvec, void *val)
{
	AL_PV_ENTER;
	if(!pvec){
		AL_PV_RETURN_OBJ(NULL);
	}
	/*
	if(AL_PV_GETRC(pvec) != 1){
		al_pv *pv = al_pv_cons(r, pvec, val);
		al_pv_release(pvec);
		AL_PV_RETURN_OBJ(pv);
	}
	*/
	int count = al_pv_length(pvec);
	int shift = al_pv_getShift(pvec);
	al_pv_node *root = al_pv_getRoot(pvec);
	int taillength = al_pv_getTailLength(pvec);
	void **tail = al_pv_getTail(pvec);
	if(count - al_pv_tailOffset(pvec) < AL_PV_NBRANCHES){
		P("there's room in the tail");
		//void **newtail = (void **)al_region_getBytes(r, (taillength + 1) * sizeof(void *));
		//if(tail){
		//memcpy(newtail, tail, (taillength + 1) * sizeof(void *));
		//}else{
		//memset(newtail, 0, (taillength + 1) * sizeof(void *));
		//}
		//for(int i = 0; i < taillength; i++){
		//AL_PV_INCRC(newtail[i]);
		//}
		void **newtail = tail;
		if(!newtail){
			//newtail = (void **)al_region_getBytes(r, (taillength + 1) * sizeof(void *));
			newtail = (void **)al_region_getBytes(r, AL_PV_NBRANCHES * sizeof(void *));
			memset(newtail, 0, (taillength + 1) * sizeof(void *));
			al_pv_setTail(pvec, newtail);
		}
		P("inserting %p into slot %d", val, taillength);
		newtail[taillength] = val;
	        //al_pv *pv = _al_pv_alloc(count + 1, shift, al_pv_node_copy(root), taillength + 1, newtail, freefn);
		al_pv_setTailLength(pvec, taillength + 1);
		al_pv_setCount(pvec, count + 1);
		AL_PV_RETURN_OBJ(pvec);
	}
	P("no room in the tail");
	al_pv_node *newroot;
	//void **tailclone = al_pv_cloneTail(taillength, tail);
	//al_pv_node *tailnode = al_pv_node_allocWithArray(taillength, tailclone);
	al_pv_node *tailnode = al_pv_node_allocWithArray(r, taillength, tail);
	//AL_PV_INCRC(tailnode);
	//osc_mem_free(tailclone);
	int newshift = shift;
	if((count >> AL_PV_NBITS) > (1 << shift)){
		P("no room at this level: construct a new path");
		newroot = al_pv_node_alloc(r);
		//al_pv_node_getArray(newroot)[0] = al_pv_node_copy(root);
		//AL_PV_INCRC(root);
		al_pv_node_getArray(newroot)[0] = root;
		al_pv_node_getArray(newroot)[1] = al_pv_newPath_m(r, shift, tailnode);
		newshift += AL_PV_NBITS;
	}else{
		P("room at the current level: push tail");
		//newroot = al_pv_node_copy(al_pv_pushTail(pvec, shift, root, tailnode));
		newroot = al_pv_pushTail_m(r, pvec, shift, root, tailnode);
		//AL_PV_INCRC(newroot);
	}
	void **array = tail;
	if(!array){
		array = (void **)al_region_getBytes(r, AL_PV_NBRANCHES * sizeof(void *));
	}
	memset(array, 0, AL_PV_NBRANCHES * sizeof(void *));
	array[0] = val;
	//al_pv *pv = _al_pv_alloc(count + 1, newshift, newroot, 1, array, freefn);
	al_pv_setCount(pvec, count + 1);
	al_pv_setShift(pvec, newshift);
	al_pv_setRoot(pvec, newroot);
	al_pv_setTailLength(pvec, 1);
	al_pv_setTail(pvec, array);
	AL_PV_RETURN_OBJ(pvec);
}

// create a new tail and construct a new path to the old one
al_pv_node *al_pv_pushTail(al_region r, al_pv *pvec, int level, al_pv_node *parent, al_pv_node *tailnode)
{
	AL_PV_ENTER
	int subidx = ((al_pv_length(pvec) - 1) >> level) & AL_PV_MASK;
	//void *parent_clone = (void **)al_region_getBytes(r, AL_PV_NBRANCHES * sizeof(void *));
	//P("al_pv_node_getArray(parent) = %p\n", al_pv_node_getArray(parent));
	//memcpy(parent_clone, al_pv_node_getArray(parent), AL_PV_NBRANCHES * sizeof(void *));
	//al_pv_node *ret = al_pv_node_allocWithArray(parent_clone);
	//al_pv_node *ret = al_pv_node_clone(parent);
	al_pv_node *ret = parent;
	al_pv_node *nodeToInsert = NULL;
	if(level == AL_PV_NBITS){
		nodeToInsert = tailnode;
	}else{
		al_pv_node *child = (al_pv_node *)al_pv_node_getArray(parent)[subidx];
		if(child == NULL){
			P("no child");
			nodeToInsert = al_pv_newPath(r, level - AL_PV_NBITS, tailnode);
		}else{
			P("have child");
			nodeToInsert = al_pv_pushTail(r, pvec, level - AL_PV_NBITS, child, tailnode);
		}
	}
	al_pv_node_getArray(ret)[subidx] = nodeToInsert;//al_pv_node_copy(nodeToInsert);
	AL_PV_RETURN_OBJ(ret);
}

al_pv_node *al_pv_pushTail_m(al_region r, al_pv *pvec, int level, al_pv_node *parent, al_pv_node *tailnode)
{
	AL_PV_ENTER;
	if(AL_PV_GETRC(pvec) != 1){
		al_pv_node *n = al_pv_pushTail(r, pvec, level, parent, tailnode);
		AL_PV_RETURN_OBJ(n);
	}
	int subidx = ((al_pv_length(pvec) - 1) >> level) & AL_PV_MASK;
	//void *parent_clone = (void **)al_region_getBytes(r, AL_PV_NBRANCHES * sizeof(void *));
	//P("al_pv_node_getArray(parent) = %p\n", al_pv_node_getArray(parent));
	//memcpy(parent_clone, al_pv_node_getArray(parent), AL_PV_NBRANCHES * sizeof(void *));
	//al_pv_node *ret = al_pv_node_allocWithArray(parent_clone);
	al_pv_node *ret = parent;
	al_pv_node *nodeToInsert = NULL;
	if(level == AL_PV_NBITS){
		nodeToInsert = tailnode;
	}else{
		al_pv_node *child = (al_pv_node *)al_pv_node_getArray(parent)[subidx];
		if(child == NULL){
			P("no child");
			nodeToInsert = al_pv_newPath_m(r, level - AL_PV_NBITS, tailnode);
		}else{
			P("have child");
			nodeToInsert = al_pv_pushTail_m(r, pvec, level - AL_PV_NBITS, child, tailnode);
		}
	}
	al_pv_node_getArray(ret)[subidx] = nodeToInsert;//al_pv_node_copy(nodeToInsert);
	AL_PV_RETURN_OBJ(ret);
}

// construct a new path to node
al_pv_node *al_pv_newPath(al_region r, int level, al_pv_node *node)
{
	AL_PV_ENTER
	if(level == 0){
		AL_PV_RETURN_OBJ(node);
	}
	al_pv_node *ret = al_pv_node_alloc(r);
	al_pv_node_getArray(ret)[0] = al_pv_newPath(r, level - AL_PV_NBITS, node);
	AL_PV_RETURN_OBJ(ret);
}

al_pv_node *al_pv_newPath_m(al_region r, int level, al_pv_node *node)
{
	AL_PV_ENTER;
	if(AL_PV_GETRC(node) != 1){
		al_pv_node *n = al_pv_newPath(r, level, node);
		AL_PV_RETURN_OBJ(n);
	}
	if(level == 0){
		AL_PV_RETURN_OBJ(node);
	}
	al_pv_node *ret = al_pv_node_alloc(r);
	//al_pv_node *ret = node;
	al_pv_node_getArray(ret)[0] = al_pv_newPath_m(r, level - AL_PV_NBITS, node);
	AL_PV_RETURN_OBJ(ret);
}

// remove the last element
al_pv *al_pv_pop(al_region r, al_pv *pvec)
{
	AL_PV_ENTER
	int count = al_pv_length(pvec);
	int shift = al_pv_getShift(pvec);
	al_pv_node *root = al_pv_getRoot(pvec);
	int taillength = al_pv_getTailLength(pvec);
	void **tail = al_pv_getTail(pvec);
	void (*freefn)(void *) = al_pv_getFreeFn(pvec);
	if(count == 0){
		AL_PV_RETURN_OBJ(NULL);
	}
	if(count == 1){
		al_pv *pv = al_pv_alloc(r, freefn); // al_pv_empty
		AL_PV_RETURN_OBJ(pv);
	}
	if(count - al_pv_tailOffset(pvec) > 1){
		P("removing element from tail");
		void **newTail = (void **)al_region_getBytes(r, (taillength - 1) * sizeof(void *));
		memcpy(newTail, tail, (taillength - 1) * sizeof(void *));
		al_pv *pv = _al_pv_alloc(r, count - 1, shift, root, taillength - 1, newTail, freefn);
		AL_PV_RETURN_OBJ(pv);
	}
	P("removing last element from tail and poping tail");
	void **newTail = al_pv_getArrayFor(pvec, taillength - 1);
	al_pv_node *newroot = al_pv_popTail(r, pvec, shift, root);
	int newshift = shift;
	if(newroot == NULL){
		newroot = al_pv_node_alloc(r);//al_pv_node_empty;
	}
	if(shift > AL_PV_NBITS && al_pv_node_getArray(newroot)[1] == NULL){
		newroot = (al_pv_node *)al_pv_node_getArray(newroot)[0];
		newshift -= AL_PV_NBITS;
	}
	al_pv *pv = _al_pv_alloc(r, count - 1, newshift, newroot, taillength - 1, newTail, freefn);
	AL_PV_RETURN_OBJ(pv);
}

// pop the empty tail off
al_pv_node *al_pv_popTail(al_region r, al_pv *pvec, int level, al_pv_node *node)
{
	AL_PV_ENTER
	int subidx = ((al_pv_length(pvec) - 2) >> level) & AL_PV_MASK;
	if(level > AL_PV_NBITS){
		P("We're not at the bottom level---keep digging");
		al_pv_node *newchild = al_pv_popTail(r, pvec, level - AL_PV_NBITS, (al_pv_node *)al_pv_node_getArray(node)[subidx]);
		if(newchild == NULL && subidx == 0){
			AL_PV_RETURN_OBJ(NULL);
		}else{
			void **clone = (void **)al_region_getBytes(r, AL_PV_NBRANCHES * sizeof(void *));
			memcpy(clone, al_pv_node_getArray(node), AL_PV_NBRANCHES * sizeof(void *));
			al_pv_node *ret = al_pv_node_allocWithArray(r, AL_PV_NBRANCHES, clone);
			al_pv_node_getArray(ret)[subidx] = newchild;
			AL_PV_RETURN_OBJ(ret);
		}
	}else if(subidx == 0){
		P("subidx = 0");
		AL_PV_RETURN_OBJ(NULL);
	}else{
		P("else");
		void **clone = (void **)al_region_getBytes(r, AL_PV_NBRANCHES * sizeof(void *));
		memcpy(clone, al_pv_node_getArray(node), AL_PV_NBRANCHES * sizeof(void *));
		al_pv_node *ret = al_pv_node_allocWithArray(r, AL_PV_NBRANCHES, clone);
		al_pv_node_getArray(ret)[subidx] = NULL;
		AL_PV_RETURN_OBJ(ret);
	}
}

int al_pv_length(al_pv *pvec)
{
	if(pvec){
		return pvec->count;
	}
	return 0;
}

void al_pv_setCount(al_pv *pvec, int count)
{
	if(pvec){
		pvec->count = count;
	}
}

int al_pv_getShift(al_pv *pvec)
{
	if(pvec){
		return pvec->shift;
	}
	return 0;
}

void al_pv_setShift(al_pv *pvec, int shift)
{
	if(pvec){
		pvec->shift = shift;
	}
}

al_pv_node *al_pv_getRoot(al_pv *pvec)
{
	if(pvec){
		return pvec->root;
	}
	return NULL;
}

void al_pv_setRoot(al_pv *pvec, al_pv_node *root)
{
	if(pvec){
		pvec->root = root;
	}
}

int al_pv_getTailLength(al_pv *pvec)
{
	if(pvec){
		return pvec->taillength;
	}
	return 0;
}

void al_pv_setTailLength(al_pv *pvec, int taillength)
{
	if(pvec){
		pvec->taillength = taillength;
	}
}

void **al_pv_getTail(al_pv *pvec)
{
	if(pvec){
		return pvec->tail;
	}
	return NULL;
}

void al_pv_setTail(al_pv *pvec, void **tail)
{
	if(pvec){
		pvec->tail = tail;
	}
}

void (*al_pv_getFreeFn(al_pv *pvec))(void *)
{
	if(pvec){
		return pvec->freefn;
	}
	return NULL;
}

void print_node(al_pv_node *node, int level, int branch)
{
	if(node == NULL){
		return;
	}
	void **array = al_pv_node_getArray(node);
	if(level == 0){
		for(int i = 0; i < AL_PV_NBRANCHES; i++){
			al_pv_ud *ud = (al_pv_ud *)(array[i]);
			void *data = al_pv_ud_getData(ud);
			printf("%*p: %p (%d) (rc : %d)\n", branch * 10, ud, data, (int)(data), AL_PV_GETRC(ud));
		}
	}else{
		for(int i = 0; i < AL_PV_NBRANCHES; i++){
			printf("%*d (%p):\n", branch * 10, i, array[i]);
			print_node(array[i], level - AL_PV_NBITS, branch + 1);
		}
	}
}

void al_pv_print(al_pv *pvec)
{
	int debug = al_pv_debug;
	al_pv_debug = 0;
	printf("%p: count = %d (rc : %d)\n", pvec, al_pv_length(pvec), AL_PV_GETRC(pvec));
	al_pv_node *root = al_pv_getRoot(pvec);
	print_node(root, al_pv_getShift(pvec), 0);
	void **tail = al_pv_getTail(pvec);
	printf("tail %p:\n", tail);
	for(int i = 0; i < al_pv_getTailLength(pvec); i++){
		al_pv_ud *ud = tail[i];
		void *data = al_pv_ud_getData(ud);
		printf("%p: %p (%d) (rc : %d)\n", ud, data, (int)data, AL_PV_GETRC(ud));
	}
	al_pv_debug = debug;
}

void myfree(void *obj)
{
	P("%p", obj);
}

al_pv2 *_al_pv2_alloc(al_region r, al_pv *head, al_pv *tail)
{
	al_pv2 *pvec2 = al_region_getBytes(r, sizeof(al_pv2));
	if(pvec2){
		pvec2->refcount = 1;
		pvec2->length = 0;
		pvec2->head = head;
		pvec2->tail = tail;
	}
	return pvec2;
}

al_pv2 *al_pv2_alloc(al_region r, void (*freefn)(void *))
{
	AL_PV_ENTER
	AL_PV_RETURN_OBJ(_al_pv2_alloc(r, al_pv_alloc(r, freefn), al_pv_alloc(r, freefn)), al_pv_printRefCount);
}
/*
void al_pv2_release(al_pv2 *pvec2)
{
	printf("%s\n", __func__);
	AL_PV_ENTER
	if(pvec2){
		P("%s: rc = %d", __func__, AL_PV2_GETRC(pvec2));
		if(AL_PV2_GETRC(pvec2) < 0){
			AL_PV_RETURN;
		}
		int rc = AL_PV2_DECRC(pvec2);
		if(rc == 0){
			al_pv_release(pvec2->head);
			al_pv_release(pvec2->tail);
			memset(pvec2, 0, sizeof(al_pv2));
			AL_PV2_DECRC(pvec2);
			osc_mem_free(pvec2);
		}
	}
	AL_PV_RETURN;
}
*/
al_pv2 *al_pv2_copy(al_pv2 *pvec2)
{
	AL_PV_ENTER
	if(pvec2){
		AL_PV2_INCRC(pvec2);
		AL_PV_RETURN_OBJ(pvec2);
	}
	AL_PV_RETURN_OBJ(NULL);
}

al_pv2 *al_pv2_retain(al_pv2 *pvec2)
{
	return al_pv2_copy(pvec2);
}

al_pv2 *al_pv2_clone(al_pv2 *pvec2)
{
	AL_PV_ENTER
	AL_PV_RETURN_OBJ(NULL);
}

void *al_pv2_nth(al_pv2 *pvec2, int i)
{
	if(!pvec2){
		return NULL;
	}
	al_pv *head = pvec2->head;
	int headcount = al_pv_length(head);
	al_pv *tail = pvec2->tail;
	if(i < headcount){
		return al_pv_nth(head, headcount - i - 1);
	}else{
		return al_pv_nth(tail, i - headcount);
	}
}

al_pv2 *al_pv2_assocN(al_region r, al_pv2 *pvec2, int i, void *val)
{
	if(!pvec2){
		return NULL;
	}
	al_pv *head = pvec2->head;
	int headcount = al_pv_length(head);
	al_pv *tail = pvec2->tail;
	int tailcount = al_pv_length(tail);
	if(i <= headcount + tailcount){
		if(i < headcount){
			return _al_pv2_alloc(r, al_pv_assocN(r, head, headcount - i - 1, val), al_pv_copy(tail));
		}else{
			return _al_pv2_alloc(r, al_pv_copy(head), al_pv_assocN(r, tail, i - headcount, val));
		}
	}else{
		return pvec2;
	}
}

al_pv2 *al_pv2_assocN_m(al_region r, al_pv2 *pvec2, int i, void *val)
{
	if(!pvec2){
		return NULL;
	}
	al_pv *head = pvec2->head;
	int headcount = al_pv_length(head);
	al_pv *tail = pvec2->tail;
	int tailcount = al_pv_length(tail);
	if(i <= headcount + tailcount){
		if(i < headcount){
			al_pv_assocN_m(r, head, headcount - i - 1, val);
			return pvec2;
		}else{
			al_pv_assocN_m(r, tail, i - headcount, val);
			return pvec2;
		}
	}else{
		return pvec2;
	}
}

al_pv2 *al_pv2_append(al_region r, al_pv2 *pvec2, void *val)
{
	if(!pvec2){
		return NULL;
	}
	al_pv *head = pvec2->head;
	al_pv *tail = pvec2->tail;
	return _al_pv2_alloc(r, al_pv_copy(head), al_pv_assocN(r, tail, al_pv_length(tail), val));
}

al_pv2 *al_pv2_append_m(al_region r, al_pv2 *pvec2, void *val)
{
	if(!pvec2){
		return NULL;
	}
	al_pv *tail = pvec2->tail;
	al_pv_assocN_m(r, tail, al_pv_length(tail), val);
	return pvec2;
}

al_pv2 *al_pv2_prepend(al_region r, al_pv2 *pvec2, void *val)
{
	if(!pvec2){
		return NULL;
	}
	al_pv *head = pvec2->head;
	al_pv *tail = pvec2->tail;
	return _al_pv2_alloc(r, al_pv_assocN(r, head, al_pv_length(head), val), al_pv_copy(tail));
}

al_pv2 *al_pv2_prepend_m(al_region r, al_pv2 *pvec2, void *val)
{
	if(!pvec2){
		return NULL;
	}
	al_pv *head = pvec2->head;
	al_pv_assocN_m(r, head, al_pv_length(head), val);
	return pvec2;
}

al_pv2 *al_pv2_popFirst(al_region r, al_pv2 *pvec2)
{
	if(!pvec2){
		return NULL;
	}
	al_pv *head = pvec2->head;
	al_pv *tail = pvec2->tail;
	//return _al_pv2_alloc(r, al_pv_pop(r, head), al_pv_copy(tail));
	if(al_pv_length(head)){
		return _al_pv2_alloc(r, al_pv_pop(r, head), tail);
	}else if(al_pv_length(tail)){
		return _al_pv2_alloc(r, head, al_pv_pop(r, tail));
	}else{
		return NULL;
	}
}

al_pv2 *al_pv2_popLast(al_region r, al_pv2 *pvec2)
{
	if(!pvec2){
		return NULL;
	}
	al_pv *head = pvec2->head;
	al_pv *tail = pvec2->tail;
	return _al_pv2_alloc(r, al_pv_copy(head), al_pv_pop(r, tail));
}

int al_pv2_length(al_pv2 *pvec2)
{
	if(pvec2){
		return al_pv_length(pvec2->head) + al_pv_length(pvec2->tail);
	}else{
		return 0;
	}
}

/*
int main(int argc, char **argv)
{
	long n = 5;
	al_pv **pvec = al_region_getBytes(r, n * sizeof(al_pv *));;
	al_pv *pvec_empty = al_pv_alloc(r, myfree);
	pvec[0] = al_pv_assocN(r, pvec_empty, 0, (void *)0);
	//al_pv_node *node = al_pv_nth(pvec[0], 0);
	for(long i = 1; i < n; i++){
		printf("**************************************************\n");
		pvec[i] = al_pv_assocN(r, pvec[i - 1], i, (void *)i);
	}
	printf("**************************************************\n");
	printf("HERE\n");
	//pvec[n - 1] = al_pv_assocN(r, pvec[n - 1], 0, (void *)10);
	al_pv *p = pvec[n - 1];
	pvec[n - 1] = al_pv_pop(r, p);
	printf("**************************************************\n");
	printf("FREE\n");
	al_pv_release(p);
	print_pvec(pvec[n - 1]);
	print_pvec(pvec[n - 2]);
	//osc_mem_free(pvec);
	for(int i = 0; i < n; i++){
		printf("Freeing %d\n", i);
		al_pv_release(pvec[i]);
		printf("**************************************************\n");
	}
	while(1){
		sleep(1);
	}
	return 0;
}
*/
