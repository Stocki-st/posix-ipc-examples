all: menu compute display

menu: menu.c share.h
	gcc -Wall -o menu menu.c -lrt

compute: compute.c share.h
	gcc -Wall -o compute compute.c -lrt

display: display.c share.h
	gcc -Wall -o display display.c -lrt

clean:
	rm -f menu display compute is211818.pipe

format:
	astyle *.c *.h
