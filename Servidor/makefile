CC=gcc
CFLAGS=-g -Wall
OBJ= $(SRC:.c=.o)

all:	ftpserver

ftpserver:	server.o	ftp.o	arquivos.o	database.o
	gcc	server.o	ftp.o	arquivos.o	database.o	-o	ftpserver

server.o:	server.c  ftp.c	arquivos.c	database.c
	gcc	-c	-g	-Wall	server.c
	gcc	-c	-g	-Wall	ftp.c
	gcc	-c	-g	-Wall	arquivos.c
	gcc -c	-g	-Wall	database.c

clean:
	rm	-rf	*.o