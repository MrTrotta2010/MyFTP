#include "ftpserver.h"

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
	unsigned tamanho = tamarq(arquivos);

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

unsigned long min (int a, unsigned b) {
	if (a < b) return a;
	return b;
}

// Esta função transfere um arquivo do cliente para o servidor
char *put(char *arquivo, char *tam, int sockfd) {
	unsigned tamanho = strtoul(tam, NULL, 10); 
	unsigned brestantes = tamanho;
	printf("Tamanho: %u\n", brestantes);
	//getchar();

	// O buffer tem o tamanho do arquivo+1 para contar com o EOF
	char buff[MAX], *msg = malloc(MAX);

	// Cria um arquivo de mesmo nome na pasta de arquivos do servidor
	char novoarq[MAX];
	bzero(novoarq, MAX);
	strcat(novoarq, "Arquivos/");
	strcat(novoarq, arquivo);
	FILE *arq = fopen(novoarq, "w");
	unsigned brecebidos = 0;

	if (arq) {
		// Recebe o arquivo em pedaços
		while (brestantes > 0) {
			unsigned blidos;
			bzero(buff, MAX);
			blidos = read(sockfd, buff, min(MAX, brestantes));

			// Salva os dados recebidos no arquivo criado
			fwrite(buff, 1, blidos, arq);
			brestantes -= blidos;
			brecebidos += blidos;
			//printf("Restam %u bytes de %u\n", brestantes, tamanho);
		}
		fclose(arq);
		//printf("%u bytes recebido", brecebidos);

		// Salva o arquivo no arquivo de arquivos
		if (adcarq(arquivo, tam) == 0) {
			strcpy(msg, "Arquivo transferido com sucesso!");
			//printf("%u\n", tamarq(arq));
		} else {
			remove(novoarq);
		}
	} else {
		strcpy(msg, "Falha na transferência!");
	}

	return msg;
}

// Esta função transfere um arquivo do servidor para o cliente
char *get(char *arquivo, int sockfd) {
	
	// Adiciona o diretório correto ao nome do arquivo
	char endarq[MAX+9] = "Arquivos/", *msg = malloc(MAX);

	strcat(endarq, arquivo);
	//printf("Arquivo: %s\n", endarq);

	// Verifica se o arquivo está no servidor
	FILE *fp = fopen(endarq, "r");

	if (fp) {
		// O servidor precisa descobrir o tamanho o arquivo
		// Primeiro, o buffer guardará o tamanho do arquivo
		fclose (fp);
		char tam[MAX];
		unsigned tamanho;
		bzero(tam, MAX);
		strcpy(tam, encontrafsize(arquivo, &tamanho));


		// Envia o tamanho do arquivo
		write(sockfd, tam, MAX);

		char buff[MAX];
		unsigned brestantes = tamanho;

		fp = fopen(endarq, "r");
		// Transfere o arquivo em pedaços
		if (fp) {
			while (brestantes > 0) {
				bzero(buff, MAX);
				// Leio um pedaço do arquivo
				// O tamanho do pedaço será o tamanho do meu chunk ou a quantidade de bites restantes do arquivo
				fread(buff, 1, min(MAX, brestantes), fp);
				int benviados = write(sockfd, buff, min(MAX, brestantes));
				//printf("Enviados %d\n", benviados);
				brestantes -= benviados; 
			}
			fclose(fp);
		} else {
            bzero(buff, MAX);
            strcpy(buff, "-1");
    		write(sockfd, buff, MAX);
	    	strcpy(msg, "Falha na transferência!");
		}

		// Exclui o arquivo enviado da lista de arquivos e o deleta
		if (remarq(arquivo) == 0) { // Tudo ok
			remove(endarq);
			strcpy(msg, "Arquivo transferido com sucesso!");
		} else { //Erro
			adcarq(endarq, tam); // Readiciona o arquivo para manter a consistência
			strcpy(msg, "Falha na transferência!");
		}
	} else {
		char buff[MAX];
        bzero(buff, MAX);
        strcpy(buff, "-1");
		write(sockfd, buff, MAX);
		strcpy(msg, "Arquivo inexistente!");
	}

	return msg;
}

// Esta função faz o controle de início de sessão do usuário
char *trataLogin(int logado, int comando, int sockfd) {

	char *msg = malloc(MAX);
	//printf("%d\n", logado);

	if (logado)
		strcpy(msg, "Já fez login, cabeção");
	else {
		strcpy(msg, "Falha na autenticação!");
		if (comando == 3) {
			char buff[MAX];
			bzero(buff, MAX);
			strcpy(buff, "-1");
			write(sockfd, buff, MAX);
		}
	}

	return msg;
}

char *errmsg(int cmd) {

	char *erro = malloc(MAX);

	switch(cmd) {

		case -1:
			strcpy(erro, "Comando inválido!");
			break;

		case -2:
			strcpy(erro, "Argumentos incorretos!");
			break;
		
		case -3:
			strcpy(erro, "Arquivo inexistente!");
			break;
	}
	
	return erro;
}

// Esta função decodifica o comando enviado pelo cliente
char *decodcmd(Comando cmd, int sockfd, int *logado) {

	//printf("logado? %d\n", (*logado));
	// Verifica se o usuário já se autenticou com o servidor
	if ((*logado)) {
		switch(cmd.comando) {
			case 1: // login
				return trataLogin(TRUE, cmd.comando, sockfd);
			
			case 2: // put
				return put(cmd.arg1, cmd.arg2, sockfd);

			case 3: // get
				return get(cmd.arg1, sockfd);
			
			case 4: // ls
				return ls();
			
			default:
				return errmsg(cmd.comando);
		}
	} else {
		switch(cmd.comando) {
			case 1: // login
				return login(cmd.arg1, cmd.arg2, logado);

			default:			
				return trataLogin(FALSE, cmd.comando, sockfd);
		}
	}
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

		read(sockfd, cmd.arg1, MAX);

		read(sockfd, cmd.arg2, MAX);

		cmd.comando = atoi(buff);
		
		// Verifica se o comando é exit

		// Decodifica o comando recebido
		//printf("Comando: %d - Argumentos: %s, %s\n", cmd.comando, cmd.arg1, cmd.arg2);
		char *resposta = decodcmd(cmd, sockfd, &logado);
		write(sockfd, resposta, strlen(resposta));
		free(resposta);
	} 
}