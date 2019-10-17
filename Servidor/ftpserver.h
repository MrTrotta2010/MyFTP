#include <netdb.h> 
#include <netinet/in.h> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h> 
#include <sys/types.h>

#define MAX 256
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
char *decodcmd(Comando cmd, int sockfd, char **logado);
char *trataLogin(char *logado, int comando, int sockfd);
char *login(char *usuario, char *senha, char **loginuser);
char *ls(char *login);
char *put(char *arquivo, char *tam, int sockfd, char *login);
char *get(char *arquivo, int sockfd, char *login);
char *errmsg(int cmd);

/*FUNÇÕES DE MANIPULAÇÃO DE ARQUIVOS*/
unsigned tamarq(FILE *arq);
char *encontrafsize(char *login, char *arquivo, unsigned *tamanho);
int adcarq(char *login, char *arquivo, char *tamanho);
int remarq(char *login, char *arquivo);