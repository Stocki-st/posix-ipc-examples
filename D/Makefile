all: menu display

menu: menu.c share.h
	gcc -Wall -o menu menu.c -lrt -lpthread

display: display.c share.h
	gcc -Wall -o display display.c -lrt -lpthread

clean:
	rm -f menu display

format:
	astyle *.c *.h
