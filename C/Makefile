all: menu display

menu: menu.c share.h
	gcc -Wall -o menu menu.c -lrt

display: display.c share.h
	gcc -Wall -o display display.c -lrt

clean:
	rm -f menu display is211818.pipe

format:
	astyle *.c *.h
