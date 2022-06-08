all: menu display

menu: menu.c share.h
	gcc -Wall -o menu menu.c

disp: disp.c share.h
	gcc -Wall -o display display.c

clean:
	rm -f menu disp is211818

format:
	astyle *.c *.h
