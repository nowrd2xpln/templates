
OBJECTS=../common/psxapp.o 

EXTRA_CFLAGS := $(EXTRA_CFLAGS) -DDEBUG -DLINUX

CC=cc

.c.o:
	  $(CC) $(CFLAGS) $(INCLUDES) $(EXTRA_CFLAGS) -c -g $<

psxapp: $(OBJECTS) $(LIBS)
	  $(CC) -g -pthread -lrt psxapp.o -o $@
	 # $(CC) -g $(CFLAGS) $(INCLUDES) $(OBJECTS) $(LIBS) -o $@

clean:
	  rm -f spcapp *.o *core
