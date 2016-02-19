/*
 * list.c
 *
 *  Created on: 28 ott 2015
 *      Author: michele
 */

#ifndef LYST_OPTIMIZATION
#include "lyst.h"

#include <stdint.h>
#include <jni.h>

#include "shared.h"

/**
 * Questo e' il file piu' incasinato
 * In pratica chiama le funzioni Java da C
 * Ho solo funzioni static per essere in linea con la sintassi del C,
 * ma si potrebbero anche usare funzioni di istanze.
 * In pratica per tutte le funzioni prima ti recuperi la Classe di
 * cui vuoi invocare il metodo,
 * poi recuperi un riferimento al metodo (GetStaticMethodId)
 * poi invochi il metodo.
 * Occhio alle stringhe delle signature, se e' sbagliato anche solo un
 * carattere ti da dei SEGFAULT come se non ci fosse un domani.
 * C'ho perso mezzora per capire che mancava un punto e virgola...
 * Le signature dei metodi di una classe java la trovi facendo
 * javap -s -cp path/to/classes pakage.nomeClasse
 * come cartella del classpath devi mettere la cartella dove cominciano
 * package, non la cartella dove sono i .class
 *
 *
 */
#define LystClass "cgr_jni/lyst/Lyst"
#define LystEltClass "cgr_jni/lyst/LystElt"

Lyst Lyst_create_using(const char * s, int n, int idx)
{
	JNIEnv * jniEnv = getThreadLocalEnv();
	jclass listClass = (*jniEnv)->FindClass(jniEnv, LystClass);
	jmethodID create_using = (*jniEnv)->GetStaticMethodID(jniEnv,
			listClass, "lyst_create_using","(I)Lcgr_jni/lyst/Lyst;");
	jobject result = (*jniEnv)->CallStaticObjectMethod(jniEnv,
			listClass, create_using, idx);
	return (Lyst) result;

}

Lyst Lyst_create(const char * s, int n)
{
	JNIEnv * jniEnv = getThreadLocalEnv();
	jclass listClass = (*jniEnv)->FindClass(jniEnv, LystClass);
	jmethodID create = (*jniEnv)->GetStaticMethodID(jniEnv,
			listClass, "lyst_create","()Lcgr_jni/lyst/Lyst;");
	jobject result = (*jniEnv)->CallStaticObjectMethod(jniEnv,
			listClass, create);
	return (Lyst) result;

}

LystElt Lyst_insert_last(const char * s, int n, Lyst list, void * data)
{
	JNIEnv * jniEnv = getThreadLocalEnv();
	jclass listClass = (*jniEnv)->FindClass(jniEnv, LystClass);
	jmethodID insert_last = (*jniEnv)->GetStaticMethodID(jniEnv,
			listClass, "lyst_insert_last",
			"(Lcgr_jni/lyst/Lyst;J)Lcgr_jni/lyst/LystElt;");
	jlong pointer = (jlong) (intptr_t) data;
	jobject result = (*jniEnv)->CallStaticObjectMethod(jniEnv,
			listClass, insert_last, list, pointer);
	return (LystElt) result;
}

LystElt
Lyst_insert_before(const char * s, int n, LystElt element, void * data)
{
	JNIEnv * jniEnv = getThreadLocalEnv();
	jclass listClass = (*jniEnv)->FindClass(jniEnv, LystClass);
	jmethodID insert_before = (*jniEnv)->GetStaticMethodID(jniEnv,
			listClass, "lyst_insert_before",
			"(Lcgr_jni/lyst/LystElt;J)Lcgr_jni/lyst/LystElt;");
	jlong pointer = (jlong) (intptr_t) data;
	jobject result = (*jniEnv)->CallStaticObjectMethod(jniEnv,
			listClass, insert_before, element, pointer);
	return (LystElt) result;
}

LystElt lyst_first(Lyst list)
{
	JNIEnv * jniEnv = getThreadLocalEnv();
	jclass listClass = (*jniEnv)->FindClass(jniEnv, LystClass);
	jmethodID first = (*jniEnv)->GetStaticMethodID(jniEnv,
			listClass, "lyst_first",
			"(Lcgr_jni/lyst/Lyst;)Lcgr_jni/lyst/LystElt;");
	jobject result = (*jniEnv)->CallStaticObjectMethod(jniEnv,
			listClass, first, list);
	return (LystElt) result;
}

LystElt lyst_last(Lyst list)
{
	JNIEnv * jniEnv = getThreadLocalEnv();
	jclass listClass = (*jniEnv)->FindClass(jniEnv, LystClass);
	jmethodID last = (*jniEnv)->GetStaticMethodID(jniEnv, listClass,
			"lyst_last","(Lcgr_jni/lyst/Lyst;)Lcgr_jni/lyst/LystElt;");
	jobject result = (*jniEnv)->CallStaticObjectMethod(jniEnv,
			listClass, last, list);
	return (LystElt) result;
}

LystElt lyst_next(LystElt elt)
{
	JNIEnv * jniEnv = getThreadLocalEnv();
	jclass listClass = (*jniEnv)->FindClass(jniEnv, LystClass);
	jmethodID next = (*jniEnv)->GetStaticMethodID(jniEnv,
			listClass, "lyst_next",
			"(Lcgr_jni/lyst/LystElt;)Lcgr_jni/lyst/LystElt;");
	jobject result = (*jniEnv)->CallStaticObjectMethod(jniEnv,
			listClass, next, elt);
	return (LystElt) result;
}

void * lyst_data(LystElt elt)
{
	JNIEnv * jniEnv = getThreadLocalEnv();
	jclass listClass = (*jniEnv)->FindClass(jniEnv, LystClass);
	jmethodID data = (*jniEnv)->GetStaticMethodID(jniEnv,
			listClass, "lyst_data","(Lcgr_jni/lyst/LystElt;)J");
	jlong result = (*jniEnv)->CallStaticLongMethod(jniEnv,
			listClass, data, elt);
	return (void *) (intptr_t) result;

}

void
lyst_delete_set(Lyst list, LystCallback fn, void *arg)
{
	JNIEnv * jniEnv = getThreadLocalEnv();
	jclass listClass = (*jniEnv)->FindClass(jniEnv, LystClass);
	jmethodID delete_set = (*jniEnv)->GetStaticMethodID(jniEnv,
			listClass, "lyst_delete_set","(Lcgr_jni/lyst/Lyst;JJ)V");
	(*jniEnv)->CallStaticVoidMethod(jniEnv,
			listClass, delete_set, list,
			(jlong) (intptr_t) fn, (jlong) (intptr_t) arg);
}
void
Lyst_destroy(const char *file, int line, Lyst list)
{
	lyst_clear(list);
	list = NULL;
}

void Lyst_clear(const char* file,int line , Lyst list)
{
	LystElt cur;
	while ((cur = lyst_first(list)) != NULL)
	{
		lyst_delete(cur);
	}
}

void *
lyst_data_set(LystElt elt, void *new)
{
	JNIEnv * jniEnv = getThreadLocalEnv();
	jclass listClass = (*jniEnv)->FindClass(jniEnv, LystClass);
	jmethodID data_set = (*jniEnv)->GetStaticMethodID(jniEnv,
			listClass, "lyst_data_set","(Lcgr_jni/lyst/LystElt;J)J");
	jlong result = (*jniEnv)->CallStaticLongMethod(jniEnv,
			listClass, data_set, elt, (jlong) (intptr_t) new);
	return (void *) (intptr_t) result;
}

static jobject getLyst(LystElt elt)
{
	JNIEnv * jniEnv = getThreadLocalEnv();
	jclass listClass = (*jniEnv)->FindClass(jniEnv, LystClass);
	jmethodID getLyst = (*jniEnv)->GetStaticMethodID(jniEnv,
			listClass, "getLyst",
			"(Lcgr_jni/lyst/LystElt;)Lcgr_jni/lyst/Lyst;");
	jobject result = (*jniEnv)->CallStaticObjectMethod(jniEnv,
			listClass, getLyst, elt);
	return result;
}

static LystCallback lyst_getDeleteFunction(Lyst list)
{
	JNIEnv * jniEnv = getThreadLocalEnv();
	jclass listClass = (*jniEnv)->FindClass(jniEnv, LystClass);
	jmethodID getDeleteFunction = (*jniEnv)->GetStaticMethodID(jniEnv,
			listClass, "getDeleteFunction","(Lcgr_jni/lyst/Lyst;)J");
	jlong result = (*jniEnv)->CallStaticLongMethod(jniEnv,
			listClass, getDeleteFunction, list);
	return (LystCallback) (intptr_t) result;
}

static void * lyst_getDeleteUserdata(Lyst list)
{
	JNIEnv * jniEnv = getThreadLocalEnv();
	jclass listClass = (*jniEnv)->FindClass(jniEnv, LystClass);
	jmethodID getDeleteUserdata = (*jniEnv)->GetStaticMethodID(jniEnv,
			listClass, "getDeleteUserdata","(Lcgr_jni/lyst/Lyst;)J");
	jlong result = (*jniEnv)->CallStaticLongMethod(jniEnv,
			listClass, getDeleteUserdata, list);
	return (void *) (intptr_t) result;
}

void
Lyst_delete(const char *file, int line, LystElt elt)
{
	LystCallback fn = lyst_getDeleteFunction(getLyst(elt));
	void *userdata = lyst_getDeleteUserdata(getLyst(elt));
	if (fn !=  NULL)
		fn(elt, userdata);
	JNIEnv * jniEnv = getThreadLocalEnv();
	jclass listClass = (*jniEnv)->FindClass(jniEnv, LystClass);
	jmethodID delete = (*jniEnv)->GetStaticMethodID(jniEnv,
			listClass, "lyst_delete","(Lcgr_jni/lyst/LystElt;)V");
	(*jniEnv)->CallStaticVoidMethod(jniEnv, listClass, delete, elt);
}

#else

#include "platform.h"
#include "memmgr.h"
#include "lystP.h"

/*
 * prototypes for private functions
 */

static void lyst__clear(Lyst);
static int lyst__inorder(Lyst,void *,void *);
static LystElt lyst__elt_create(const char *, int, Lyst, void *);
static void lyst__elt_clear(LystElt);
static char *lyst__alloc(const char *, int, int, unsigned int);
static void lyst__free(const char *, int, int, char *);

/*
 * public functions -- create and destroy list objects
 */

Lyst
Lyst_create(const char *file, int line)
{
  return Lyst_create_using(file, line, 0);
}

Lyst
Lyst_create_using(const char *file, int line, int idx)
{
  Lyst list;

  if ((list = (Lyst) lyst__alloc(file, line, idx, sizeof(*list))) == NULL)
  {
	putErrmsg("Can't create list.", NULL);
	return NULL;
  }

  lyst__clear(list);
  list->alloc_idx = idx;
  return list;
}

static void
wipe_lyst(const char *file, int line, Lyst list, int destroy)
{
  LystElt cur;
  LystElt next;
  int alloc_idx;

  if (list == NULL)
  {
	  return;
  }

  alloc_idx = list->alloc_idx;
  for (cur = list->first; cur != NULL; cur = next)
  {
     next = cur->next;
     if (list->delete_cb != NULL) list->delete_cb(cur,list->delete_arg);
     lyst__elt_clear(cur); /* just in case user mistakenly accesses later... */
     lyst__free(file, line, alloc_idx, (char *) cur);
  }

  if (destroy)
  {
     lyst__clear(list); /* just in case user mistakenly accesses later... */
     lyst__free(file, line, alloc_idx, (char *) list);
  }
  else
  {
     list->first = NULL;
     list->last = NULL;
     list->length = 0;
  }
}

void
Lyst_clear(const char *file, int line, Lyst list)
{
  wipe_lyst(file, line, list, 0);
}

void
Lyst_destroy(const char *file, int line, Lyst list)
{
  wipe_lyst(file, line, list, 1);
}

/*
 * public functions - get and set list information
 */

void
lyst_compare_set(Lyst list, LystCompareFn fn)
{
  if (list != NULL) list->compare = fn;
}

LystCompareFn
lyst_compare_get(Lyst list)
{
  return (list == NULL) ? NULL : list->compare;
}

void
lyst_direction_set(Lyst list, LystSortDirection dir)
{
  if (list != NULL) list->dir = dir;
}

void
lyst_delete_set(Lyst list, LystCallback fn, void *arg)
{
  if (list != NULL)
  {
	list->delete_cb = fn;
	list->delete_arg = arg;
  }
}

void
lyst_delete_get(Lyst list, LystCallback *fn, void **arg)
{
  if (list != NULL)
  {
	CHKVOID(fn);
	CHKVOID(arg);
	*fn = list->delete_cb;
	*arg = list->delete_arg;
  }
}

void
lyst_insert_set(Lyst list, LystCallback fn, void *arg)
{
  if (list != NULL)
  {
	list->insert_cb = fn;
	list->insert_arg = arg;
  }
}

void
lyst_insert_get(Lyst list, LystCallback *fn, void **arg)
{
  if (list != NULL)
  {
	CHKVOID(fn);
	CHKVOID(arg);
	*fn = list->insert_cb;
	*arg = list->insert_arg;
  }
}

unsigned long
lyst_length(Lyst list)
{
  return (list == NULL) ? 0 : list->length;
}

/*
 * public functions -- add and delete elements
 */

LystElt
Lyst_insert(const char *file, int line, Lyst list, void *data)
{
  LystElt cur;

  CHKNULL(list);

  /* if not a sorted list, then just append to the end of the lyst */
  if (list->compare == NULL)
  {
      return (list->dir == LIST_SORT_ASCENDING) ?
          Lyst_insert_last(file, line, list, data) :
          Lyst_insert_first(file, line, list, data);
  }

  /* find position to insert new data into lyst */
  /* start from end of lyst to keep sort stable */
  /* because lyst__inorder returns true when both elements are equal */
  for (cur = list->last; cur != NULL; cur = cur->prev)
     if (lyst__inorder(list,cur->data,data)) break;

  /* insert into lyst */
  if (cur == NULL)
     return Lyst_insert_first(file, line, list, data);
  else
     return Lyst_insert_after(file, line, cur, data);
}

LystElt
Lyst_insert_first(const char *file, int line, Lyst list, void *data)
{
  LystElt new_elt;

  CHKNULL(list);

  /* create new element */
  if ((new_elt = lyst__elt_create(file, line, list, data)) == NULL) return NULL;

  /* insert new element at the beginning of the lyst */
  new_elt->next = list->first;
  if (list->first != NULL)
     list->first->prev = new_elt;
  else
     list->last = new_elt;
  list->first = new_elt;

  list->length += 1;

  if (list->insert_cb != NULL) list->insert_cb(new_elt,list->insert_arg);
  return new_elt;
}

LystElt
Lyst_insert_last(const char *file, int line, Lyst list, void *data)
{
  LystElt new_elt;

  CHKNULL(list);

  /* create new element */
  if ((new_elt = lyst__elt_create(file, line, list, data)) == NULL) return NULL;

  /* insert new element at the end of the lyst */
  new_elt->prev = list->last;
  if (list->last != NULL)
     list->last->next = new_elt;
  else
     list->first = new_elt;
  list->last = new_elt;

  list->length += 1;

  if (list->insert_cb != NULL) list->insert_cb(new_elt,list->insert_arg);
  return new_elt;
}

LystElt
Lyst_insert_before(const char *file, int line, LystElt elt, void *data)
{
  Lyst list;
  LystElt new_elt;

  CHKNULL(elt);
  list = elt->lyst;
  CHKNULL(list);

  /* create new element */
  if ((new_elt = lyst__elt_create(file, line, list, data)) == NULL) return NULL;

  /* insert new element before the specified element */
  new_elt->prev = elt->prev;
  new_elt->next = elt;
  if (elt->prev != NULL)
     elt->prev->next = new_elt;
  else
     list->first = new_elt;
  elt->prev = new_elt;
  list->length += 1;
  if (list->insert_cb != NULL) list->insert_cb(new_elt,list->insert_arg);
  return new_elt;
}

LystElt
Lyst_insert_after(const char *file, int line, LystElt elt, void *data)
{
  Lyst list;
  LystElt new_elt;

  CHKNULL(elt);
  list = elt->lyst;
  CHKNULL(list);

  /* create new element */
  if ((new_elt = lyst__elt_create(file, line, list, data)) == NULL) return NULL;

  /* insert new element after the specified element */
  new_elt->prev = elt;
  new_elt->next = elt->next;
  if (elt->next != NULL)
     elt->next->prev = new_elt;
  else
     list->last = new_elt;
  elt->next = new_elt;

  list->length += 1;

  if (list->insert_cb != NULL) list->insert_cb(new_elt,list->insert_arg);
  return new_elt;
}

void
Lyst_delete(const char *file, int line, LystElt elt)
{
  Lyst list;

  if (elt == NULL)
  {
	  return;
  }

  list = elt->lyst;
  CHKVOID(list);
  CHKVOID(list->length > 0);
  if (list->delete_cb != NULL) list->delete_cb(elt,list->delete_arg);

  /* update previous pointers */
  if (elt->prev != NULL)
     elt->prev->next = elt->next;
  else
     list->first = elt->next;

  /* update following pointers */
  if (elt->next != NULL)
     elt->next->prev = elt->prev;
  else
     list->last = elt->prev;

  /* free memory associated with this element */
  lyst__elt_clear(elt); /* just in case user accesses later... */
  lyst__free(file, line, list->alloc_idx, (char *) elt);

  list->length -= 1;
}

/*
 * public functions -- traverse lysts
 */

LystElt
lyst_first(Lyst list)
{
	CHKNULL(list);
	return list->first;
}

LystElt
lyst_last(Lyst list)
{
	CHKNULL(list);
	return list->last;
}

LystElt
lyst_next(LystElt elt)
{
	CHKNULL(elt);
	return elt->next;
}

LystElt
lyst_prev(LystElt elt)
{
	CHKNULL(elt);
	return elt->prev;
}

LystElt
lyst_search(LystElt elt, void *data)
{
  Lyst list;
  LystElt cur;

  CHKNULL(elt);
  list = elt->lyst;
  CHKNULL(list);

  /* linearly search lyst */
  /* should check sorted field and bail early if possible */
  for (cur = elt; cur != NULL; cur = cur->next)
  {
     if (list->compare == NULL)
     {
        /* use "==" since no comparison function provided */
        if (cur->data == data) break;
     }
     else
     {
        /* use provided comparison function */
        if (list->compare(data,cur->data) == 0) break;
     }
  }

  return cur;
}

/*
 * public functions - get and set element information
 */

Lyst
lyst_lyst(LystElt elt)
{
	CHKNULL(elt);
	return elt->lyst;
}

void *
lyst_data(LystElt elt)
{
	CHKNULL(elt);
	return elt->data;
}

void *
lyst_data_set(LystElt elt, void *new)
{
  void *old;
  Lyst list;

  CHKNULL(elt);
  list = elt->lyst;
  CHKNULL(list);
  old = elt->data;
  elt->data = new;
  return old;
}

/*
 * public functions -- miscellaneous
 */

void
lyst_sort(Lyst list)
{
  LystElt cur;
  LystElt next;
  LystElt elt;

  CHKVOID(list);
  if (list->compare == NULL)
  {
	  return;
  }

  for (cur = list->first; cur != NULL; cur = next)
  {
     next = cur->next;

     /* find place for cur element in sorted part of lyst */
     /* start from end of lyst to keep sort stable */
     /* because lyst__inorder returns true when both elements are equal */
     for (elt = cur->prev; elt != NULL; elt = elt->prev)
        if (lyst__inorder(list,elt->data,cur->data)) break;

     /* check to see if cur element was already in correct position */
     if (elt != cur->prev)
     {
        /* remove cur element from lyst */
        /* there is always at least one element before cur */
        cur->prev->next = cur->next;
        if (cur->next == NULL)
           list->last = cur->prev;
        else
           cur->next->prev = cur->prev;

        /* insert cur element after elt element */
        /* there is always at least one element after elt */
        cur->prev = elt;
        if (elt == NULL)
        {
           cur->next = list->first;
           list->first = cur;
        }
        else
        {
           cur->next = elt->next;
           elt->next = cur;
        }
        cur->next->prev = cur;
     }
  }
}

int
lyst_sorted(Lyst list)
{
  int sorted;
  LystElt cur;

  CHKZERO(list);
  if (list->compare == NULL)
  {
	  return 0;
  }

  sorted = 1;
  if ((cur = list->first) != NULL)
  {
     while (cur->next != NULL)
     {
        if (!lyst__inorder(list,cur->data,cur->next->data)) break;
        cur = cur->next;
     }
     if (cur->next != NULL) sorted = 0;
  }

  return sorted;
}

void
lyst_apply(Lyst list, LystCallback fn, void *user_arg)
{
  LystElt cur;
  LystElt next;

  CHKVOID(list);
  CHKVOID(fn);
  for (cur = list->first; cur != NULL; cur = next)
  {
     next = cur->next;
     (*fn)(cur,user_arg);
  }
}

/*
 * private functions -- zero out/initialize fields of a lyst or element
 */

static void
lyst__clear(Lyst list)
{
  list->first = NULL;
  list->last = NULL;
  list->length = 0;
  list->compare = NULL;
  list->dir = LIST_SORT_ASCENDING;
  list->delete_cb = NULL;
  list->delete_arg = NULL;
  list->insert_cb = NULL;
  list->insert_arg = NULL;
  list->alloc_idx = 0;
}

static int
lyst__inorder(Lyst list, void *data1, void *data2)
{
  return (list->dir == LIST_SORT_ASCENDING && list->compare(data1,data2) <= 0)
  || (list->dir == LIST_SORT_DESCENDING && list->compare(data1,data2) >= 0);
}

static LystElt
lyst__elt_create(const char *file, int line, Lyst list, void *data)
{
  LystElt elt;

  if ((elt = (LystElt) lyst__alloc(file, line, list->alloc_idx, sizeof(*elt)))
	== NULL)
  {
	putErrmsg("Can't create list element.", NULL);
	return NULL;
  }

  lyst__elt_clear(elt);
  elt->lyst = list;
  elt->data = data;

  return elt;
}

static void
lyst__elt_clear(LystElt elt)
{
  elt->lyst = NULL;
  elt->prev = NULL;
  elt->next = NULL;
  elt->data = NULL;
}

static char *lyst__alloc(const char *fileName, int lineNbr, int idx,
		unsigned size)
{
  //MemAllocator	take = memmgr_take(idx);
  char		*ptr;

  //CHKNULL(take);
  //ptr = take(fileName, lineNbr, size);
  ptr = malloc(size);
  if (ptr == NULL)
  {
	  putErrmsg("Lyst memory allocation failed.", utoa(size));
  }

  return ptr;
}

static void lyst__free(const char *fileName, int lineNbr, int idx, char *ptr)
{
  //MemDeallocator	release = memmgr_release(idx);

  //if (release != NULL) release(fileName, lineNbr, (void *) ptr);
	free(ptr);
}


#endif


