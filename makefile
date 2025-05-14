CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lSDL -lSDL_image -lSDL_mixer -lSDL_ttf -lm

all: prog

prog: main.o entity.o ennemy.o stage2.o
	$(CC) $(CFLAGS) -o prog main.o entity.o ennemy.o stage2.o $(LDFLAGS)

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

entity.o: entity.c
	$(CC) $(CFLAGS) -c entity.c

ennemy.o: ennemy.c
	$(CC) $(CFLAGS) -c ennemy.c

stage2.o: stage2.c
	$(CC) $(CFLAGS) -c stage2.c

clean:
	rm -f *.o prog
