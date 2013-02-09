LDFLAGS=-lcurses -lm 
CFLAGS=
SOURCES=main.c
EXECUTABLE=game

all:
	gcc $(SOURCES) $(LDFLAGS) -o $(EXECUTABLE)
debug:	
	gcc -g $(SOURCES) $(LDFLAGS) -o $(EXECUTABLE)
	gdb $(EXECUTABLE) -tty /dev/pts/1
dist:
	gcc -static $(SOURCES) $(LDFLAGS) -o $(EXECUTABLE)
windows:
	gcc $(SOURCES) $(LDFLAGS) -o $(EXECUTABLE).exe
