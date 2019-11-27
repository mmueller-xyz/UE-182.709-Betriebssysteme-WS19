# Author: Maximilian Müller (11810852)
# Programm Name: UE01A

CC = gcc
DEFS = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SVID_SOURCE -D_POSIX_C_SOURCE=200809L
CFLAGS = -Wall -g -std=c99 -pedantic $(DEFS)
OBJECTS = isopalindrom.o
SRC = ./src/
TILAB_COMPUTER = ti17
NAME = "11810852_$(shell basename $(CURDIR))"
.PHONY: all clean compress run

all: isopalindrom

run: isopalindrom
	@./$^

scp: clean
	-@ssh tilab 'ssh $(TILAB_COMPUTER) "make -C ~/$(shell basename $(CURDIR))/ clean || mkdir ~/$(shell basename $(CURDIR))/"'
	@scp -r ./ tilab:~/$(shell basename $(CURDIR))/
	@ssh tilab 'ssh $(TILAB_COMPUTER) "make -C ~/$(shell basename $(CURDIR))/ all"'

compress: clean
	@tar -cvzf /tmp/$(NAME).tgz src/ makefile 
	@mv /tmp/$(NAME).tgz ./

isopalindrom: $(OBJECTS)
	@$(CC) $(LDFLAGS) -o $@ $^

main.o: $(SRC)main.c 

%.o: $(SRC)%.c
	@$(CC) $(CFLAGS) -c -o $@ $<

clean:
	@rm -rf *.o isopalindrom *.tgz

