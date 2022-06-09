all: menu display

menu: menu.c share.h
	gcc -Wall -o menu menu.c

display: display.c share.h
	gcc -Wall -o display display.c

clean:
	rm -f menu display is211818

format:
	astyle *.c *.h
