#@author: Maximilian Müller (11810852)

CC = gcc
DEFS = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SVID_SOURCE -D_POSIX_C_SOURCE=200809L
CFLAGS = -Wall -g -std=c99 -pedantic $(DEFS)
S_OBJECTS = server.o
C_OBJECTS = client.o
SRC = ./src/
NAME = "11810852_$(shell basename $(CURDIR))"
TILAB_COMPUTER = ti17
.PHONY: all clean compress run scp

#run: main
#	@./$^

all: client server

scp: clean
	-@ssh tilab 'ssh $(TILAB_COMPUTER) "make -C ~/$(shell basename $(CURDIR))/ clean || mkdir ~/$(shell basename $(CURDIR))/"'
	@scp -r ./ tilab:~/$(shell basename $(CURDIR))/
	@ssh tilab 'ssh $(TILAB_COMPUTER) "make -C ~/$(shell basename $(CURDIR))/ all"'

compress: clean
	@tar -cvzf /tmp/$(NAME).tgz ./
	@mv /tmp/$(NAME).tgz ./

server: $(S_OBJECTS)
	@$(CC) $(LDFLAGS) -o $@ $^

client: $(C_OBJECTS)
	@$(CC) $(LDFLAGS) -o $@ $^

%.o: $(SRC)%.c
	@$(CC) $(CFLAGS) -c -o $@ $<

clean:
	@rm -rf *.o server client *.tgz
