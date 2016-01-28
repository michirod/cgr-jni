
# Define a variable for classpath
CLASS_PATH = ../bin/jni/test

#Define java includes
J_INCLUDES = -I"/usr/lib/jvm/java-8-oracle/include" -I"/usr/lib/jvm/java-8-oracle/include/linux"
#Altri includes
INCLUDES = -Iinclude -I../ici/include -I../jni_interface/include
#lib
LIB = -L. -L../ici
#Options
OPT = -g -Wall -fPIC $(INCLUDES)

# Define a virtual path for .class in the bin directory
vpath %.class $(CLASS_PATH)

all : libbp_jni.so

libbp_jni.so : cgr.o library.o
	gcc -shared $(OPT) $(J_INCLUDES) $(LIB) -o $@ *.o
	
library.o : 
	gcc -c $(OPT) $(J_INCLUDES) library/*.c

cgr.o : 
	gcc -c $(OPT) $(J_INCLUDES) cgr/*.c
	
clean:
	rm -rf *.so *.o