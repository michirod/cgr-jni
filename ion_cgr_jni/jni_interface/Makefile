
# Define a variable for classpath
CLASS_PATH = ../../bin/:../../bin/cgr_jni:/home/michele/workspacemars/ONE/bin

#Define java includes
J_INCLUDES = -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux
#Altri includes
INCLUDES = -Iinclude -I../ici/include -I../bp/include
#lib
LIB = -L. -L../ici -L../bp
#Options
OPT = -g -Wall -fPIC $(INCLUDES)

# Define a virtual path for .class in the bin directory
vpath %.class $(CLASS_PATH)

all : jni_int.o

libjni_int.so : jni_int.o
	gcc $(OPT) $(J_INCLUDES) $(LIB) -Wl,--no-undefined -shared -o $@ *.o -lbp_jni -lici_jni -pthread

jni_int.o : Libcgr.h
	gcc -c $(OPT) $(J_INCLUDES) *.c
	
Libcgr.h : Libcgr.class
	javah -classpath $(CLASS_PATH) cgr_jni.$*
	
clean :
	rm -f *.o *.so