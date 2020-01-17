# Author: Maximilian Müller (11810852)
# Programm Name: UE03

CC = gcc
DEFS = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SVID_SOURCE -D_POSIX_C_SOURCE=200809L
CFLAGS = -Wall -g -std=c99 -pedantic $(DEFS)
LDFLAGS = -pthread -lrt
S_OBJECTS = supervisor.o ringBuffer.o
G_OBJECTS = generator.o ringBuffer.o graph.o
SRC = ./src/
NAME = "11810852_$(shell basename $(CURDIR))"
TILAB_COMPUTER = ti17
.PHONY: all clean compress run scp

#run: main
#	@./$^

all: supervisor generator

scp: clean
	-@ssh tilab 'ssh $(TILAB_COMPUTER) "make -C ~/$(shell basename $(CURDIR))/ clean || mkdir ~/$(shell basename $(CURDIR))/"'
	@scp -r ./ tilab:~/$(shell basename $(CURDIR))/
	@ssh tilab 'ssh $(TILAB_COMPUTER) "make -C ~/$(shell basename $(CURDIR))/ all"'

compress: clean
	@tar -cvzf /tmp/$(NAME).tgz src/ makefile
	@mv /tmp/$(NAME).tgz ./


supervisor: $(S_OBJECTS)
generator: $(G_OBJECTS)
ringBuffer.o: $(SRC)ringBuffer.c $(SRC)ringBuffer.h
graph.o: $(SRC)graph.c $(SRC)graph.h

%:
	@$(CC) -o $@ $^ $(LDFLAGS) 

%.o: $(SRC)%.c
	@$(CC) $(CFLAGS) -c -o $@ $<

clean:
	@rm -rf *.o supervisor generator *.tgz
