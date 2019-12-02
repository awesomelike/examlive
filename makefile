TARGET=ExamLive
 
CC=gcc

DEBUG=-g

OPT=-O0

WARN=-w
 
PTHREAD=-pthread -lpthread
 
CCFLAGS=$(DEBUG) $(OPT) $(WARN) $(PTHREAD) -pipe
 
GTKLIB=`pkg-config --cflags --libs gtk+-3.0`
MySqlLib = `mysql_config --libs`
MySqlConfig = `mysql_config -- cflags`

LD=gcc
LDFLAGS=$(PTHREAD) $(GTKLIB) -export-dynamic
 
OBJS=    examlive.o
 
all: $(OBJS)
	$(LD) -o $(TARGET)   $(OBJS) $(MySqlLib) $(LDFLAGS)
    
examlive.o: src/examlive.c
	$(CC) -c $(CCFLAGS) src/examlive.c $(GTKLIB)  -o examlive.o

clean:
	rm -f *.o *.out