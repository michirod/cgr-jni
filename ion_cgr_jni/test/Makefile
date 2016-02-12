
# Define a variable for classpath
CLASS_PATH = ../../bin/cgr_jni/test

#Define java includes
J_INCLUDES = -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux
#Altri includes
INCLUDES = -I../jni_interface/include -I../ici/include -I../bp/include
#lib
LIB = -L. -L../ici -L../bp
#Options
OPT = -g -Wall -fPIC $(INCLUDES)

# Define a virtual path for .class in the bin directory
vpath %.class $(CLASS_PATH)

all : test.o

# $@ matches the target, $< matches the first dependancy
libtest.so : test.o
	gcc $(LIB) -Wl,--no-undefined -Wall -fPIC -shared -lnormal -lcgr -o $@ $^ 
	
# $@ matches the target, $< matches the first dependancy
JNITest.o : jni_test_JNITest.c jni_test_JNITest.h
	gcc -c $(OPT) $(J_INCLUDES) -o $@ $< 
	
libjni_int.so : jni_int.o
	gcc $(OPT) $(J_INCLUDES) $(LIB) -Wl,--no-undefined -shared -o $@ *.o -lbp_jni -lici_jni -pthread

test.o : JNITest.h
	gcc -c $(OPT) $(J_INCLUDES) *.c
	
# $* matches the target filename without the extension
JNITest.h : JNITest.class
	javah -classpath ../../bin cgr_jni.test.$*
	
clean :
	rm -f *.o *.so