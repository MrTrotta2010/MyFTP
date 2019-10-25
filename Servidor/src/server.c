#include "ftpserver.h"

// Função que estabelece as conexões
int main(int argc, char **argv) { 
	int sockfd, connfd;
	unsigned len; 
	struct sockaddr_in servaddr, cli; 

	printf("SERVIDOR EM C!\n\n");

	if (connectsql() != 0) exit(1);

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

	while(TRUE){
		// Agora, o server aguarda conexões 
		if ((listen(sockfd, 5)) != 0) { 
			printf("Falha na conexão!\n"); 
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
		else {
			struct in_addr ipCliente = cli.sin_addr;
			char ip[32];
			inet_ntop(AF_INET, &ipCliente, ip, 32);
			printf("O servidor aceitou o cliente!\nIP: %s\n", ip);
		}

		// Agora o servidor aguarda os comandos do cliente 
		ftp(connfd); 

		printf("Conexão encerrada\n\n");

	}
	//close(sockfd);
}