CC=gcc -Wall

PROGS=mutex

all: $(PROGS)

clean:
	rm -f $(PROGS)

mutex: mutex.c
	$(CC) mutex.c -o mutex -lpthread
