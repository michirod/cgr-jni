/*
 * list.c
 *
 *  Created on: 28 ott 2015
 *      Author: michele
 */

#include "lyst.h"

#include <stdint.h>
#include <jni.h>

#include "shared.h"

/**
 * Questo e' il file piu' incasinato
 * In pratica chiama le funzioni Java da C
 * Ho solo funzioni static per essere in linea con la sintassi del C, ma si potrebbero anche usare funzioni di istanze.
 * In pratica per tutte le funzioni prima ti recuperi la Classe di cui vuoi invocare il metodo,
 * poi recuperi un riferimento al metodo (GetStaticMethodId)
 * poi invochi il metodo.
 * Occhio alle stringhe delle signature, se e' sbagliato anche solo un carattere ti da dei SEGFAULT come se non ci fosse un domani.
 * C'ho perso mezzora per capire che mancava un punto e virgola...
 * Le signature dei metodi di una classe java la trovi facendo javap -s -cp path/to/classes pakage.nomeClasse
 * come cartella del classpath devi mettere la cartella dove cominciano package, non la cartella dove sono i .class
 *
 *
 */

const char * myLystClass = "jni/test/lyst/MyLyst";
const char * myLystEltClass = "jni/test/lyst/MylystElt";

Lyst Lyst_create_using(const char * s, int n, int idx)
{
	JNIEnv * jniEnv = getThreadLocalEnv();
	jclass listClass = (*jniEnv)->FindClass(jniEnv, myLystClass);
	jmethodID create_using = (*jniEnv)->GetStaticMethodID(jniEnv, listClass, "lyst_create_using","(I)Ljni/test/lyst/MyLyst;");
	jobject result = (*jniEnv)->CallStaticObjectMethod(jniEnv, listClass, create_using, idx);
	return (Lyst) result;

}

LystElt Lyst_insert_last(const char * s, int n, Lyst list, void * data)
{
	JNIEnv * jniEnv = getThreadLocalEnv();
	jclass listClass = (*jniEnv)->FindClass(jniEnv, myLystClass);
	jmethodID insert_last = (*jniEnv)->GetStaticMethodID(jniEnv, listClass, "lyst_insert_last","(Ljni/test/lyst/MyLyst;J)Ljni/test/lyst/MyLystElt;");
	jlong pointer = (jlong) (intptr_t) data;
	jobject result = (*jniEnv)->CallStaticObjectMethod(jniEnv, listClass, insert_last, list, pointer);
	return (LystElt) result;
}

LystElt lyst_first(Lyst list)
{
	JNIEnv * jniEnv = getThreadLocalEnv();
	jclass listClass = (*jniEnv)->FindClass(jniEnv, myLystClass);
	jmethodID first = (*jniEnv)->GetStaticMethodID(jniEnv, listClass, "lyst_first","(Ljni/test/lyst/MyLyst;)Ljni/test/lyst/MyLystElt;");
	jobject result = (*jniEnv)->CallStaticObjectMethod(jniEnv, listClass, first, list);
	return (LystElt) result;
}

LystElt lyst_last(Lyst list)
{
	JNIEnv * jniEnv = getThreadLocalEnv();
	jclass listClass = (*jniEnv)->FindClass(jniEnv, myLystClass);
	jmethodID last = (*jniEnv)->GetStaticMethodID(jniEnv, listClass, "lyst_last","(Ljni/test/lyst/MyLyst;)Ljni/test/lyst/MyLystElt;");
	jobject result = (*jniEnv)->CallStaticObjectMethod(jniEnv, listClass, last, list);
	return (LystElt) result;
}

LystElt lyst_next(LystElt elt)
{
	JNIEnv * jniEnv = getThreadLocalEnv();
	jclass listClass = (*jniEnv)->FindClass(jniEnv, myLystClass);
	jmethodID next = (*jniEnv)->GetStaticMethodID(jniEnv, listClass, "lyst_next","(Ljni/test/lyst/MyLystElt;)Ljni/test/lyst/MyLystElt;");
	jobject result = (*jniEnv)->CallStaticObjectMethod(jniEnv, listClass, next, elt);
	return (LystElt) result;
}

void * lyst_data(LystElt elt)
{
	JNIEnv * jniEnv = getThreadLocalEnv();
	jclass listClass = (*jniEnv)->FindClass(jniEnv, myLystClass);
	jmethodID data = (*jniEnv)->GetStaticMethodID(jniEnv, listClass, "lyst_data","(Ljni/test/lyst/MyLystElt;)J");
	jlong result = (*jniEnv)->CallStaticLongMethod(jniEnv, listClass, data, elt);
	return (void *) (intptr_t) result;

}

void
lyst_delete_set(Lyst list, LystCallback fn, void *arg)
{
	JNIEnv * jniEnv = getThreadLocalEnv();
	jclass listClass = (*jniEnv)->FindClass(jniEnv, myLystClass);
	jmethodID delete_set = (*jniEnv)->GetStaticMethodID(jniEnv, listClass, "lyst_delete_set","(Ljni/test/lyst/MyLyst;JJ)V");
	(*jniEnv)->CallStaticVoidMethod(jniEnv, listClass, delete_set, list, (jlong) (intptr_t) fn, (jlong) (intptr_t) arg);
}
void
Lyst_destroy(const char *file, int line, Lyst list)
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
	jclass listClass = (*jniEnv)->FindClass(jniEnv, myLystClass);
	jmethodID data_set = (*jniEnv)->GetStaticMethodID(jniEnv, listClass, "lyst_data_set","(Ljni/test/lyst/MyLystElt;J)J");
	jlong result = (*jniEnv)->CallStaticLongMethod(jniEnv, listClass, data_set, elt, (jlong) (intptr_t) new);
	return (void *) (intptr_t) result;
}

static jobject getLyst(LystElt elt)
{
	JNIEnv * jniEnv = getThreadLocalEnv();
	jclass listClass = (*jniEnv)->FindClass(jniEnv, myLystClass);
	jmethodID getLyst = (*jniEnv)->GetStaticMethodID(jniEnv, listClass, "getLyst","(Ljni/test/lyst/MyLystElt;)Ljni/test/lyst/MyLyst;");
	jobject result = (*jniEnv)->CallStaticObjectMethod(jniEnv, listClass, getLyst, elt);
	return result;
}

static LystCallback lyst_getDeleteFunction(Lyst list)
{
	JNIEnv * jniEnv = getThreadLocalEnv();
	jclass listClass = (*jniEnv)->FindClass(jniEnv, myLystClass);
	jmethodID getDeleteFunction = (*jniEnv)->GetStaticMethodID(jniEnv, listClass, "getDeleteFunction","(Ljni/test/lyst/MyLyst;)J");
	jlong result = (*jniEnv)->CallStaticLongMethod(jniEnv, listClass, getDeleteFunction, list);
	return (LystCallback) (intptr_t) result;
}

static void * lyst_getDeleteUserdata(Lyst list)
{
	JNIEnv * jniEnv = getThreadLocalEnv();
	jclass listClass = (*jniEnv)->FindClass(jniEnv, myLystClass);
	jmethodID getDeleteUserdata = (*jniEnv)->GetStaticMethodID(jniEnv, listClass, "getDeleteUserdata","(Ljni/test/lyst/MyLyst;)J");
	jlong result = (*jniEnv)->CallStaticLongMethod(jniEnv, listClass, getDeleteUserdata, list);
	return (void *) (intptr_t) result;
}

void
Lyst_delete(const char *file, int line, LystElt elt)
{
	LystCallback fn = lyst_getDeleteFunction(getLyst(elt));
	void *userdata = lyst_getDeleteUserdata(getLyst(elt));
	fn(elt, userdata);
	JNIEnv * jniEnv = getThreadLocalEnv();
	jclass listClass = (*jniEnv)->FindClass(jniEnv, myLystClass);
	jmethodID delete = (*jniEnv)->GetStaticMethodID(jniEnv, listClass, "lyst_delete","(Ljni/test/lyst/MyLystElt;)V");
	(*jniEnv)->CallStaticVoidMethod(jniEnv, listClass, delete, elt);
}

