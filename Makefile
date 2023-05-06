CC = gcc
CFLAGS = -Wall -Wextra -Wextra

all: singleshell multishell

singleshell: singleshell.c
	$(CC) $(CFLAGS) -o singleshell singleshell.c

multishell: multishell.c singleshell
	$(CC) $(CFLAGS) -o multishell multishell.c
clean:
	rm -f singleshell multishell
.PHONY: all clean
