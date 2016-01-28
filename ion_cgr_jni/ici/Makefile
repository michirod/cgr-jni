
# Define a variable for classpath
CLASS_PATH = ../bin/jni/test

#Define java includes
J_INCLUDES = -I"/usr/lib/jvm/java-8-oracle/include" -I"/usr/lib/jvm/java-8-oracle/include/linux"
#Altri includes
INCLUDES = -Iinclude -I../jni_interface/include
#lib
LIB = -L.
#Options
OPT = -g -Wall -fPIC $(INCLUDES)

# Define a virtual path for .class in the bin directory
vpath %.class $(CLASS_PATH)

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