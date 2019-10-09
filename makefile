productionLine: fork.o functions.o queue.o
	gcc fork.o functions.o queue.o -o productionLine -Wall

functions.o: functions.c functions.h
	gcc -c functions.c -Wall

queue.o: queue.c functions.h
	gcc -c queue.c -Wall

fork.o: fork.c functions.h
	gcc -c fork.c -Wall

clean:
	rm productionLine fork.o functions.o queue.o

