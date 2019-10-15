CC=gcc
CFLAGS=-g -Wall
OBJ= $(SRC:.c=.o)

all:	ftpserver

ftpserver:	server.o	ftp.o	arquivos.o
	gcc	server.o	ftp.o	arquivos.o	-o	ftpserver

server.o:	server.c  ftp.c	arquivos.c
	gcc	-c	-g	-Wall	server.c
	gcc	-c	-g	-Wall	ftp.c
	gcc	-c	-g	-Wall	arquivos.c

clean:
	rm	-rf	*.o