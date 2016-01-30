#Define java includes
J_INCLUDES = -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux
#includes
INCLUDES = -I. -I.. -Iinclude -I../include
#Options
OPT = -c -g -Wall -fPIC $(INCLUDES)
# Manage overbooking disabled
OPT = $(OPT) -DMANAGE_OVERBOOKING=0

all : libcgr.o

libcgr.a : libcgr.o
	gcc -g -Wall -fPIC $(INCLUDES) $(J_INCLUDES) include/typedef.h ../psm.o ../smlist.o ../list.o ../utils.o *.c
	ar rcs $@ *.o ../psm.o ../smlist.o ../list.o ../utils.o

libcgr.so :
	gcc -shared -g -Wall -fPIC  $(INCLUDES) $(J_INCLUDES) -o $@ *.c

libcgr.o : libcgr.c
	gcc $(OPT) $(J_INCLUDES) -o $@ $<
	
clean :
	rm -rf *.o *.so *.a