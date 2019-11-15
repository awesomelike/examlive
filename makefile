# change application name here (executable output name)
TARGET=ExamLive
 
# compiler
CC=gcc
# debug
DEBUG=-g
# optimisation
OPT=-O0
# warnings
WARN=-Wall
 
PTHREAD=-pthread -lpthread
 
CCFLAGS=$(DEBUG) $(OPT) $(WARN) $(PTHREAD) -pipe
 
GTKLIB=`pkg-config --cflags --libs gtk+-3.0`
MySqlLib = `mysql_config --libs`
MySqlConfig = `mysql_config -- cflags`
 
# linker
LD=gcc
LDFLAGS=$(PTHREAD) $(GTKLIB) -export-dynamic
 
OBJS=    examlive.o
 
all: $(OBJS)
	$(LD) -o $(TARGET)   $(OBJS) $(MySqlLib) $(LDFLAGS)
    
examlive.o: src/examlive.c
	$(CC) -c $(CCFLAGS) src/examlive.c $(GTKLIB)  -o examlive.o

# game.o: src/game.c
#  	$(CC) -c $(CCFLAGS) src/game.c $(GTKLIB) -o game.o
clean:
	rm -f *.o *.out