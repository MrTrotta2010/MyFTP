#include <netdb.h> 
#include <netinet/in.h> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h> 
#include <sys/types.h>

#define MAX 80 
#define SA struct sockaddr
#define TRUE 1
#define FALSE 0

typedef struct {
	int comando;
	char arg1[MAX];
	char arg2[MAX];
} Comando;

// Esta função verifica o número de argumentos
int argumentos(int argc, char ** argv) {

	if (argc != 2) {
		printf("Modo de uso: ./ftpserver <porta>\n");
		return 1;
	}
	
	return 0;
}

char *login(char *usuario, char *senha) {

	char u[MAX], s[MAX], *msg = malloc(MAX);
	FILE *usuarios = fopen("usuarios.data", "r");
	strcpy(msg, "Usuário incorreto!");

	// Abre o arquivo de usuários e verifica se o nome de usuário e a senha existem
	while (!feof(usuarios))	{
		fscanf(usuarios, "%s %s\n", u, s);

		if (strcmp(u, usuario) == 0) {
			if (strcmp(s, senha) == 0) {
				strcpy(msg, "Login efetuado com sucesso!");
				break;
			}
			strcpy(msg, "Senha incorreta!");
			break;
		}
	}

	return msg;
}

char *ls() {

	FILE *arquivos = fopen("files.data", "r");
	char *lista;

	// Descobre o tamanho da lista de arquivos
	fseek(arquivos, 0, SEEK_END);
	int tamanho = ftell(arquivos);
	fseek(arquivos, 0, SEEK_SET);

	if (tamanho > 0) { // Se existem arquivos, copia a lista para a string
		lista = calloc(tamanho + 1, sizeof(char));
		fread(lista, 1, tamanho, arquivos);
		fclose(arquivos);
	} else { // Do contrário, não existem arquivos
		lista = malloc(MAX);
		strcpy(lista, "Nenhum arquivo encontrado!");
	}

	return lista;
}

char *put(char *arquivo, int tamanho, int sockfd) {
	
	char *buff = malloc(tamanho), *msg = malloc(MAX);
	printf("Recebendo arquivo: %s de tamanho %d\n", arquivo, tamanho);
	read(sockfd, buff, tamanho);
	FILE *arq = fopen("funcionou.txt", "wb");
	fprintf(arq, "%s", buff);
	fclose(arq);
	strcpy(msg, "Arquivo enviado com sucesso!");
	return msg;
}

// Esta função decodifica o comando enviado pelo cliente
char *decodcmd(Comando cmd, int sockfd) {

	switch(cmd.comando) {
		case 1: // login
			return login(cmd.arg1, cmd.arg2);
		
		case 2: // put
			return put(cmd.arg1, atoi(cmd.arg2), sockfd);

		case 3: // get
			break;
		
		case 4: // ls
			return ls();
	}
}

// Esta função recebe e processa as requisições do cliente 
void ftp(int sockfd) {

	char buff[MAX]; 
	Comando cmd; 

	while (1) {

		bzero(buff, MAX);
		bzero(cmd.arg1, MAX);
		bzero(cmd.arg2, MAX);

		// Recebe o comando do cliente e guarda os argumentos 
		read(sockfd, buff, MAX);
		write(sockfd, "Ok", 2);

		read(sockfd, cmd.arg1, MAX);
		write(sockfd, "Ok", 2);

		read(sockfd, cmd.arg2, MAX);
		write(sockfd, "Ok", 2);

		cmd.comando = atoi(buff);
		
		// Verifica se o caomando é exit
		if (strcmp("exit", buff) == 0) {
			printf("Cliente encerrou a conexão...\n");
			break;
		}

		// Decodifica o comando recebido
		char *resposta = decodcmd(cmd, sockfd);
		printf("%s\n", resposta);
		// Envia uma resposta ao cliente
		write(sockfd, resposta, strlen(resposta));
		free(resposta);

		
		// if msg contains "Exit" then server exit and chat ended. 
		if (strncmp("exit", buff, 4) == 0) { 
			printf("Server Exit...\n"); 
			break; 
		}
	} 
}

int main(int argc, char **argv) { 
	int sockfd, connfd;
	unsigned len; 
	struct sockaddr_in servaddr, cli; 

	printf("SERVIDOR EM C!\n\n");

	if (argumentos(argc, argv) != 0) exit(1);

	// Criação e verificação do socket 
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1) { 
		printf("Falha na criação do socket!\n"); 
		exit(2);
	}
	else
		printf("Socket criado com sucesso!\n"); 
	
	bzero(&servaddr, sizeof(servaddr));

	// Define o IP e a porta 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	servaddr.sin_port = htons(atoi(argv[1])); 

	// Fazendo binding do socket
	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) { 
		printf("Porta já em uso!\n"); 
		exit(3); 
	} 
	else
		printf("Conectado à porta %s!\n", argv[1]); 

	// Agora, o server aguarda conexões 
	if ((listen(sockfd, 5)) != 0) { 
		printf("Falha na escuta!\n"); 
		exit(4); 
	} 
	else
		printf("Na escuta...\n");

	len = sizeof(cli); 

	// Aceita o pacote do cliente 
	connfd = accept(sockfd, (SA*)&cli, &len); 
	if (connfd < 0) { 
		printf("O servidor recusou o cliente!\n"); 
		exit(5); 
	} 
	else
		printf("O servidor aceitou o cliente!\n\n"); 

	// Agora o servidor aguarda os comandos do cliente 
	ftp(connfd); 

	close(sockfd); 
} 

