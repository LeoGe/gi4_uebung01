CC = gcc -m32 -lreadline -g

.PHONY: all default clean

default: myecho myenv isset fork shell

all: myecho myenv isset fork shell

myecho: myecho.c

myenv: myenv.c

isset: isset.c

fork: fork.c

shell: shell.c

clean:
	rm -f myecho myenv isset fork shell
