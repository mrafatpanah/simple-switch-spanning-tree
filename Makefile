#main: main.c switch.c system.c
#	gcc -o main main.c system.c switch.c -I.


all: main.out

main.out: main.o system.o switch.o 
 
main.o: main.c
	gcc -o main.out main.c

system.o: system.c
	gcc -pthread -o system system.c

switch.o: switch.c
	gcc -pthread -o switch switch.c


clean:
		find . -type f | xargs touch
		rm -rf *o ass4
