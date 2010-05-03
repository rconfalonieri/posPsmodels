
CC=gcc

# add here all your object  files ...
OFILES = test.o apitest.o

# ... and system libraries that your functions need
LIBS = 

# this is the 
%.o : %.c
	$(CC) -fPIC -Wall -c $<


liblparse.so : $(OFILES)
	$(CC) -shared -Wl,-soname,liblparse.so -o liblparse.so $(OFILES) \
	      $(LIBS)

clean :
	rm -f *.o liblparse.so
