

# Define a variable for classpath
CLASS_PATH = ../../bin/cgr

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
VPATH = ../../bin/cgr

all : libcgr_jni.so

libcgr_jni.so : bp.o ici.o jni_interface.o
	gcc $(LIB) -Wl,--no-undefined -Wall -fPIC -shared -o $@ bp/*.o ici/*.o jni_interface/*.o -pthread
	
# $@ matches the target, $< matches the first dependancy
libtest.so : JNITest.o libnormal.so
	gcc $(LIB) -Wl,--no-undefined -Wall -fPIC -shared -lnormal -lcgr -o $@ $^ 
	
# $@ matches the target, $< matches the first dependancy
JNITest.o : jni_test_JNITest.c jni_test_JNITest.h
	gcc -c $(OPT) $(J_INCLUDES) -o $@ $< 
	
libnormal.so : libcgr.so
	gcc $(OPT) $(J_INCLUDES) $(LIB) -Wl,--no-undefined -shared -o $@ *.c -lcgr -lpthread
	
bp.o : 
	make -C bp 
	
ici.o : 
	make -C ici
	
jni_interface.o :
	make -C jni_interface
	
# $* matches the target filename without the extension
JNITest.h : JNITest.class
	javah -classpath ../bin jni.test.$*
	
clean:
	make -C bp clean
	make -C ici clean
	make -C jni_interface clean
	rm -rf *.so *.o