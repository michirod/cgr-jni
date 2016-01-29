#Define java includes
J_INCLUDES = -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux
#includes
INCLUDES = -Iinclude -I../jni_interface/include
#lib
LIB = -L.
#Options
OPT = -g -Wall -fPIC $(INCLUDES)

all : libici_jni.so

libici_jni.so : utils.o library.o sdr.o 
	gcc -shared $(OPT) $(J_INCLUDES) $(LIB) -o $@ *.o -lpthread
	
library.o : 
	gcc -c $(OPT) $(J_INCLUDES) library/*.c

sdr.o : 
	gcc -c $(OPT) $(J_INCLUDES) sdr/*.c
	
utils.o :
	gcc -c $(OPT) $(J_INCLUDES) utils/*.c
	
clean:
	rm -rf *.so *.o