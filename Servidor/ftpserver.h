#include <netdb.h> 
#include <netinet/in.h> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h> 
#include <sys/types.h>

#define MAX 8192
#define SA struct sockaddr
#define TRUE 1
#define FALSE 0

typedef struct {
	int comando;
	char arg1[MAX];
	char arg2[MAX];
} Comando;

/*FUNÇÕES DO SERVIDOR FTP*/
void ftp(int sockfd);
int argumentos(int argc, char ** argv);
char *decodcmd(Comando cmd, int sockfd, int *logado);
char *trataLogin(int logado, int comando, int sockfd);
char *login(char *usuario, char *senha, int *logado);
char *ls();
char *put(char *arquivo, char *tam, int sockfd);
char *get(char *arquivo, int sockfd);
char *errmsg(int cmd);

/*FUNÇÕES DE MANIPULAÇÃO DE ARQUIVOS*/
unsigned tamarq(FILE *arq);
char *encontrafsize(char *arquivo, unsigned *tamanho);
int adcarq(char *arquivo, char *tamanho);
int remarq(char *arquivo);