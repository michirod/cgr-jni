/*
 * list.h
 *
 *  Created on: 28 ott 2015
 *      Author: michele
 */

/**
 * Questo e' l'header della libreria delle liste, praticamente ho copiato quello di Lyst in ION.
 * Ho solo cambiato le strutture Lyst e LystElt in modo che puntino a un jobject.
 */

#include <jni.h>
#ifndef JNI_LIST_H_
#define JNI_LIST_H_
/*
 * define types
 */
/* define a lyst */

typedef jobject Lyst;
typedef jobject LystElt;
//typedef struct LystStruct *Lyst;
//typedef struct LystEltStruct *LystElt;

typedef enum {
  LIST_SORT_ASCENDING,
  LIST_SORT_DESCENDING
} LystSortDirection;

typedef int  (*LystCompareFn)(void *,void *);
typedef void (*LystCallback)(LystElt,void *);

/*
 * function prototypes
 */

#define lyst_create_using(idx)	Lyst_create_using(__FILE__, __LINE__, idx)
Lyst Lyst_create_using(const char*,int,int);
#define lyst_create()		Lyst_create(__FILE__, __LINE__)
Lyst Lyst_create(const char*,int);
#define lyst_clear(list)	Lyst_clear(__FILE__, __LINE__, list)
void Lyst_clear(const char*,int,Lyst);
#define lyst_destroy(list)	Lyst_destroy(__FILE__, __LINE__, list)
void Lyst_destroy(const char*,int,Lyst);

void lyst_compare_set(Lyst,LystCompareFn);
LystCompareFn lyst_compare_get(Lyst);
void lyst_direction_set(Lyst,LystSortDirection);
void lyst_delete_set(Lyst,LystCallback,void *);
void lyst_delete_get(Lyst,LystCallback *,void **);
void lyst_insert_set(Lyst,LystCallback,void *);
void lyst_insert_get(Lyst,LystCallback *,void **);
unsigned long lyst_length(Lyst);

#define lyst_insert(list, data)	Lyst_insert(__FILE__, __LINE__, list, data)
LystElt Lyst_insert(const char*,int,Lyst,void *);
#define lyst_insert_first(list, data)	Lyst_insert_first(__FILE__, __LINE__, \
list, data)
LystElt Lyst_insert_first(const char*,int,Lyst,void *);
#define lyst_insert_last(list, data)	Lyst_insert_last(__FILE__, __LINE__, \
list, data)
LystElt Lyst_insert_last(const char*,int,Lyst,void *);
#define lyst_insert_before(elt, data)	Lyst_insert_before(__FILE__, __LINE__, \
elt, data)
LystElt Lyst_insert_before(const char*,int,LystElt,void *);
#define lyst_insert_after(elt, data)	Lyst_insert_after(__FILE__, __LINE__, \
elt, data)
LystElt Lyst_insert_after(const char*,int,LystElt,void *);
#define lyst_delete(elt)	Lyst_delete(__FILE__, __LINE__, elt)
void Lyst_delete(const char*,int,LystElt);

LystElt lyst_first(Lyst);
LystElt lyst_last(Lyst);
LystElt lyst_next(LystElt);
LystElt lyst_prev(LystElt);
LystElt lyst_search(LystElt,void *);

Lyst lyst_list(LystElt);
void *lyst_data(LystElt);
void *lyst_data_set(LystElt,void *);

void lyst_sort(Lyst);
int lyst_sorted(Lyst);
void lyst_apply(Lyst,LystCallback,void *);



#endif /* JNI_LIST_H_ */
