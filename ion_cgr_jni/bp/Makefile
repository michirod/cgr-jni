#Define java includes
J_INCLUDES = -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux
#includes
INCLUDES = -Iinclude -I../ici/include -I../jni_interface/include
#lib
LIB = -L. -L../ici
#Options
OPT = -g -Wall -fPIC $(INCLUDES)

all : libbp_jni.so

libbp_jni.so : cgr.o library.o
	gcc -shared $(OPT) $(J_INCLUDES) $(LIB) -o $@ *.o
	
library.o : 
	gcc -c $(OPT) $(J_INCLUDES) library/*.c

cgr.o : 
	gcc -c $(OPT) $(J_INCLUDES) -DMANAGE_OVERBOOKING=0 -DONE_SIMULATION cgr/*.c
	
clean:
	rm -rf *.so *.o