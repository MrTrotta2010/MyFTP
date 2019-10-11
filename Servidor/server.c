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

// Esta função realiza a autenticação do usuário no servidor
char *login(char *usuario, char *senha, int *logado) {

	char u[MAX], s[MAX], *msg = malloc(MAX);
	FILE *usuarios = fopen("Dados/usuarios.data", "r");
	strcpy(msg, "Usuário incorreto!");

	// Abre o arquivo de usuários e verifica se o nome de usuário e a senha existem
	while (!feof(usuarios))	{
		fscanf(usuarios, "%s %s\n", u, s);

		if (strcmp(u, usuario) == 0) {
			if (strcmp(s, senha) == 0) {
				*logado = TRUE;
				strcpy(msg, "Login efetuado com sucesso!");
				break;
			}
			strcpy(msg, "Senha incorreta!");
			break;
		}
	}

	return msg;
}

// Esta função lista todos os arquivos do servidor
char *ls() {

	FILE *arquivos = fopen("Dados/files.data", "r");
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

//Esta função adiciona uma entrada à lista de arquivos
int adcarq(char *arquivo) {
	
	int existe = FALSE;
	char aux[MAX];
	printf("->Arquivo: %s\n", arquivo);
	
	// Verifica se o arquivo já estava na lista de arquivos
	FILE *fp = fopen("Dados/files.data", "r");
	if (fp) {
		while (!feof(fp)) {
			fscanf(fp, "%s", aux);
			if (strcmp(aux, arquivo) == 0) {
				existe = TRUE;
				puts("Já tem");
				fclose(fp);
				break;
			}
		}
	} else {
		fclose(fp);
		return 1;
	}

	// Caso não esteja, adiciona
	if (!existe) {
		puts("Ainda n tem");
		fp = fopen("Dados/files.data", "a");
		if (fp) {
			fprintf(fp, "%s\n", arquivo);
			fclose(fp);
		} else {
			fclose(fp);
			return 1;
		}
	}

	return 0;
}

// Esta função transfere um arquivo do cliente para o servidor
char *put(char *arquivo, int tamanho, int sockfd) {
	
	// O buffer tem o tamanho do arquivo+1 para contar com o EOF
	char *buff = malloc(tamanho+1), *msg = malloc(MAX);

	// Recebe o arquivo
	read(sockfd, buff, tamanho+1);

	// Cria um arquivo de mesmo nome na pasta de arquivos do servidor
	char novoarq[MAX];
	bzero(novoarq, MAX);
	strcat(novoarq, "Arquivos/");
	strcat(novoarq, arquivo);
	FILE *arq = fopen(novoarq, "w");

	// Salva o arquivo recebido no arquivo criado
	if (arq) {
		fprintf(arq, "%s", buff);
		if (adcarq(arquivo) == 0) {
			strcpy(msg, "Arquivo enviado com sucesso!");
			fclose(arq);
		} else {
			fclose(arq);
			remove(novoarq);
		}
	} else {
		strcpy(msg, "Falha na transferência!");
		fclose(arq);
	}

	return msg;
}

// Esta função faz o controle de início de sessão do usuário
char *trataLogin(int logado) {

	char *msg = malloc(MAX);
	printf("%d\n", logado);

	if (logado)
		strcpy(msg, "O usuário já está logado!");
	else
		strcpy(msg, "Falha na autenticação!");

	return msg;
}

// Esta função decodifica o comando enviado pelo cliente
char *decodcmd(Comando cmd, int sockfd, int *logado) {

	//printf("logado? %d comando %d\n", *logado, cmd.comando);
	// Verifica se o usuário já se autenticou com o servidor
	if (/* (*logado) ==  */TRUE) {
		switch(cmd.comando) {
			case 1: // login
				//return trataLogin(TRUE);
				return login(cmd.arg1, cmd.arg2, logado);
			
			case 2: // put
				return put(cmd.arg1, atoi(cmd.arg2), sockfd);

			case 3: // get
				break;
			
			case 4: // ls
				return ls();
		}
	}
	// else {
	// 	switch(cmd.comando) {
	// 		case 1: // login
	// 			return login(cmd.arg1, cmd.arg2, logado);

	// 		default:			
	// 			return trataLogin(FALSE);
	// 	}
	// }
}

// Esta função recebe e processa as requisições do cliente 
void ftp(int sockfd) {

	char buff[MAX]; 
	int logado = FALSE;
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
		
		// Verifica se o comando é exit
		if (strcmp("exit", buff) == 0) {
			printf("Cliente encerrou a conexão...\n");
			break;
		}

		// Decodifica o comando recebido
		char *resposta = decodcmd(cmd, sockfd, &logado);
		printf("%s\n", resposta);
		// Envia uma resposta ao cliente
		write(sockfd, resposta, strlen(resposta));
		free(resposta);
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

