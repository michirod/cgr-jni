

# Define a variable for classpath
CLASS_PATH = ../bin/cgr_jni/test

#Define java includes
J_INCLUDES = -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux
#Altri includes
INCLUDES = -I. -Iinclude 
#lib
LIB = -L.
#Options
OPT = -g -Wall -fPIC $(INCLUDES)

# Define a virtual path for .class in the bin directory
vpath %.class $(CLASS_PATH)

ifeq ($(strip $(ONE_CLASSPATH)),)
all: help
else ifeq ($(strip $(JAVA_HOME)),)
all: java_home_notset
else 
all : libcgr_jni.so
endif

libcgr_jni.so : bp.o ici.o jni_interface.o
	gcc $(LIB) -Wl,--no-undefined -Wall -fPIC -shared -o $@ bp/*.o ici/*.o jni_interface/*.o -lm -pthread
	
libcgr_jni_test.so : test.o	
	gcc $(LIB) -Wl,--no-undefined -Wall -fPIC -shared -o $@ bp/*.o ici/*.o jni_interface/*.o test/*.o -lm -pthread
	
libtest.so : ici.o jni_interface_test.o test.o
	gcc $(LIB) -Wl,--no-undefined -Wall -fPIC -shared -o $@ ici/*.o jni_interface/init_global.o jni_interface/jni_thread.o test/*.o -lm -pthread
	
bp.o : 
	make -C bp DEBUG=$(DEBUG)
	
ici.o : 
	make -C ici DEBUG=$(DEBUG)
	
jni_interface.o :
	make -C jni_interface ONE_CLASSPATH=$(ONE_CLASSPATH) DEBUG=$(DEBUG)

jni_interface_test.o : ici.o
	make -C jni_interface jni_int_test.o
	
test.o : jni_interface_test.o
	make -C test
	
clean:
	make -C bp clean
	make -C ici clean
	make -C jni_interface clean
	make -C test clean
	rm -rf *.so *.o
	
help:
	@echo "Usage:"
	@echo "make ONE_CLASSPATH=<ONE_classpath> [ DEBUG=1 ]"

java_home_notset:
	@echo "ERROR:"
	@echo "env var JAVA_HOME needs to be set"

	