LDFLAGS=-lcurses
CFLAGS=
SOURCES=main.c
EXECUTABLE=game

all:
	gcc $(SOURCES) $(LDFLAGS) -o $(EXECUTABLE)
debug:	
	gcc -g $(SOURCES) $(LDFLAGS) -o $(EXECUTABLE)
	gdb $(EXECUTABLE) -tty /dev/pts/1
