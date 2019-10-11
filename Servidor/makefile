CC=gcc
CFLAGS=-g -Wall
SRC=client.c server.c
OBJ= $(SRC:.c=.o)

all: $(EXEC)

@echo "Vou começar a compilação"

teste: teste.o main.o
	$(CC) -o $@ $^ 

%.o: %.c
     $(CC) -o $@ -c $< $(CFLAGS)
main.o: main.c teste.h
     $(CC) -o $@ -c $< $(CFLAGS)
.PHONY: clean mrproper
clean: 
      rm -rf *.o
 @echo "Compilaçao prontinha" 
mrproper: clean
      rm -rf $(EXEC)