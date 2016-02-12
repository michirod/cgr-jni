
# Define a variable for classpath
CLASS_PATH = ../../bin/:../../bin/cgr_jni:$(ONE_CLASSPATH)

#Define java includes
J_INCLUDES = -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux
#Altri includes
INCLUDES = -Iinclude -I../ici/include -I../bp/include
#lib
LIB = -L. -L../ici -L../bp
#Options
OPT = -g -Wall -fPIC $(INCLUDES) -DOPPORTUNISTIC_ROUTING

# Define a virtual path for .class in the bin directory
vpath %.class $(CLASS_PATH)

ifeq ($(strip $(DEBUG)),1)
OPT += -DCGR_DEBUG=1 
endif

all : jni_int.o

libjni_int.so : jni_int.o
	gcc $(OPT) $(J_INCLUDES) $(LIB) -Wl,--no-undefined -shared -o $@ *.o -lbp_jni -lici_jni -pthread

jni_int.o : Libcgr.h Libocgr.h 
	gcc -c $(OPT) $(J_INCLUDES) *.c
	
Libcgr.h : Libcgr.class
	javah -classpath $(CLASS_PATH) cgr_jni.$*
	
Libocgr.h : Libocgr.class
	javah -classpath $(CLASS_PATH) cgr_jni.$*
	
jni_int_test.o :
	gcc -c $(OPT) $(J_INCLUDES) init_global.c jni_thread.c
	gcc $(OPT) $(J_INCLUDES) $(LIB) -Wl,--no-undefined -shared -o $@ init_global.o jni_thread.o -lici_jni -pthread


	
clean :
	rm -f *.o *.so