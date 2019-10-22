#include "ftpserver.h"

// Esta função verifica o número de argumentos
int argumentos(int argc, char ** argv) {

	if (argc != 2) {
		printf("Modo de uso: ./ftpserver <porta>\n");
		return 1;
	}
	
	return 0;
}

void assertfiles(char *usuario) {

	char arquivo[2048];

	// Verifica se o usuário tem os arquivos de dados
	bzero(arquivo, 2048);
	strcpy(arquivo, "Dados/");
	strcat(arquivo, usuario);
	strcat(arquivo, "_files.data");
	fclose(fopen(arquivo, "a"));
}

// Esta função realiza a autenticação do usuário no servidor
char *login(char *usuario, char *senha, char **loginuser) {

	char u[MAX], s[MAX], *msg = malloc(MAX);
	FILE *usuarios = fopen("Dados/usuarios.data", "r");
	strcpy(msg, "Usuário incorreto!");

	// Abre o arquivo de usuários e verifica se o nome de usuário e a senha existem
	while (!feof(usuarios))	{
		fscanf(usuarios, "%s %s\n", u, s);

		if (strcmp(u, usuario) == 0) {
			if (strcmp(s, senha) == 0) {
				*loginuser = malloc(2048);
				strcpy(*loginuser, usuario);
				strcpy(msg, "Login efetuado com sucesso!");
				assertfiles(usuario);
				break;
			}
			strcpy(msg, "Senha incorreta!");
			break;
		}
	}

	return msg;
}

// Esta função lista todos os arquivos do servidor
char *ls(char *login) {

	char arqname[2048];
	bzero(arqname, 2048);
	strcpy(arqname, "Dados/");
	strcat(arqname, login);
	strcat(arqname, "_files.data");

	FILE *arquivos = fopen(arqname, "r");
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
char *put(char *arquivo, char *tam, int sockfd, char *login) {
	unsigned tamanho = strtoul(tam, NULL, 10); 
	unsigned brestantes = tamanho;
	//getchar();

	// O buffer tem o tamanho do arquivo+1 para contar com o EOF
	char buff[MAX], *msg = malloc(MAX);

	// Cria um arquivo de mesmo nome na pasta de arquivos do servidor
	char novoarq[MAX];
	bzero(novoarq, MAX);
	strcat(novoarq, "Arquivos/");
	strcat(novoarq, login);
	strcat(novoarq, "/");
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

		if (adcarq(login, arquivo, tam) == 0) {
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
char *get(char *arquivo, int sockfd, char *login) {
	
	// Adiciona o diretório correto ao nome do arquivo
	char endarq[2048], *msg = malloc(MAX);

	strcpy(endarq, "Arquivos/");
	strcat(endarq, login);
	strcat(endarq, "/");
	strcat(endarq, arquivo);
	//printf("Arquivo: %s\n", endarq);

	// Verifica se o arquivo está no servidor
	FILE *fp = fopen(endarq, "r");

	if (fp) {
		// O servidor precisa descobrir o tamanho o arquivo
		// Primeiro, o buffer guardará o tamanho do arquivo
		char tam[MAX];
		unsigned tamanho;
		bzero(tam, MAX);
		strcpy(tam, encontrafsize(login, arquivo, &tamanho));

		// Envia o tamanho do arquivo
		write(sockfd, tam, MAX);

		char buff[MAX];
		unsigned brestantes = tamanho;

		// Transfere o arquivo em pedaços
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

		// Exclui o arquivo enviado da lista de arquivos e o deleta
		if (remarq(login, arquivo) == 0) { // Tudo ok
			remove(endarq);
			strcpy(msg, "Arquivo transferido com sucesso!");
		} else { //Erro
			adcarq(login, endarq, tam); // Readiciona o arquivo para manter a consistência
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
char *trataLogin(char *login, int comando, int sockfd) {

	char *msg = malloc(MAX);
	//printf("%d\n", logado);

	if (login != NULL)
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

// Esta função adiciona um novo usuário aos registros do servidor
char *adduser(char *usuario, char *senha) {

	char usr[MAX], pswd[MAX];
	char *msg = malloc(MAX);
	FILE *fp = fopen("Dados/usuarios.data", "r");
	memset(usr, '\0', MAX);
	memset(pswd, '\0', MAX);
	memset(msg, '\0', MAX);
	strcpy(msg, "Falha na criação do usuário!");

	if (fp) {
		while (!feof(fp)) {
			fscanf(fp, "%s %s\n", usr, pswd);
			if (strcmp(usr, usuario) == 0) {
				strcpy(msg, "O usuário já existe!");
				fclose(fp);
				return msg;
			}
		}
		fclose(fp);
		fp = fopen("Dados/usuarios.data", "a");

		if (fp) {
			fprintf(fp, "%s %s\n", usuario, senha);
			strcpy(msg, "Usuário criado com sucesso!");
			fclose(fp);
		}
	}

	return msg;
}

// Esta função altera a senha do usuário atualmente logado
char *passwd(char *senhaNova, char *usuario) {

	char usr[MAX], pswd[MAX];
	char *msg = malloc(MAX);
	memset(usr, '\0', MAX);
	memset(pswd, '\0', MAX);
	memset(msg, '\0', MAX);
	strcpy(msg, "Falha na alteração da senha!");

	FILE *fp = fopen("Dados/usuarios.data", "r");
	FILE *temp = fopen("Dados/temp", "a");

	if (fp) {
		if (temp) {
			while (!feof(fp)) {
				fscanf(fp, "%s %s\n", usr, pswd);
				if (strcmp(usr, usuario) == 0) {
					fprintf(temp, "%s %s\n", usr, senhaNova);
				} else {
					fprintf(temp, "%s %s\n", usr, pswd);
				}
			}
			fclose(fp);
			fclose(temp);
			// Apaga o arquivo antigo e renomeia o novo
			if (remove("Dados/usuarios.data") == 0) {
				if (rename("Dados/temp", "Dados/usuarios.data") == 0) {
					strcpy(msg, "Senha alterada com sucesso!");
				}
			}
		} else {
			fclose(fp);
		}
	} else {
		if (temp) fclose(temp);
	}

	return msg;
}

// Esta função remove o usuario atualmente logado
char *rmuser(char *senha, char *login, int *rmusuario) {

	char usr[MAX], pswd[MAX];
	char *msg = malloc(MAX);
	memset(usr, '\0', MAX);
	memset(pswd, '\0', MAX);
	memset(msg, '\0', MAX);
	strcpy(msg, "Falha na remoção do usuário!");

	FILE *fp = fopen("Dados/usuarios.data", "r");
	FILE *temp = fopen("Dados/temp", "a");

	if (fp) {
		if (temp) {
			while (!feof(fp)) {
				fscanf(fp, "%s %s\n", usr, pswd);
				if (strcmp(usr, login) == 0) {
					// Verifica a autenticação do usuário
					if (strcmp(pswd, senha) != 0) {
						strcpy(msg, "Falha na autenticação!");
						fclose(temp);
						remove("Dados/temp");
						return msg;
					}
				} else {
					fprintf(temp, "%s %s\n", usr, pswd);
				}
			}	
			fclose(fp);
			fclose(temp);
			// Apaga o arquivo antigo e renomeia o novo
			if (remove("Dados/usuarios.data") == 0) {
				if (rename("Dados/temp", "Dados/usuarios.data") == 0) {
					// Apaga o arquivo de arquivos do usuário
					char arqname[MAX];
					memset(arqname, '\0', MAX);
					strcpy(arqname, "Dados/");			
					strcat(arqname, login);		
					strcat(arqname, "_files.data");
					remove(arqname);
					*rmusuario = TRUE;
					strcpy(msg, "Usuário removido com sucesso!");
				}
			}
		} else {
			fclose(fp);
		}
	} else {
		if (temp) fclose(temp);
	}

	return msg;
}

// Esta função decodifica o comando enviado pelo cliente
char *decodcmd(Comando cmd, int sockfd, char **logado, int *rmusuario) {

	//printf("logado? %d\n", (*logado));

	// adduser pode ser realizado o usuário tendo ou não feito login
	if (cmd.comando == 6) { 
		// adduser
		return adduser(cmd.arg1, cmd.arg2);
	}

	// Verifica se o usuário já se autenticou com o servidor
	if (*logado != NULL) {
		switch(cmd.comando) {
			case 1: // login
				return trataLogin(*logado, cmd.comando, sockfd);
			
			case 2: // put
				return put(cmd.arg1, cmd.arg2, sockfd, *logado);

			case 3: // get
				return get(cmd.arg1, sockfd, *logado);
			
			case 4: // ls
				return ls(*logado);
			
			case 7: // rmuser
				return rmuser(cmd.arg1, *logado, rmusuario);
			
			case 8: // passwd
				return passwd(cmd.arg1, *logado);
			
			default:
				return errmsg(cmd.comando);
		}
	} else {
		switch(cmd.comando) {
			case 1: // login
				return login(cmd.arg1, cmd.arg2, logado);

			default:			
				return trataLogin(*logado, cmd.comando, sockfd);
		}
	}
}

// Esta função recebe e processa as requisições do cliente 
void ftp(int sockfd) {

	char buff[MAX]; 
	char *login = NULL;
	Comando cmd;
	int rmusuario = FALSE;

	while (TRUE) {

		bzero(buff, MAX);
		bzero(cmd.arg1, MAX);
		bzero(cmd.arg2, MAX);

		// Recebe o comando do cliente e guarda os argumentos
		read(sockfd, buff, 256);

		read(sockfd, cmd.arg1, 256);

		read(sockfd, cmd.arg2, 256);

		cmd.comando = atoi(buff);
		
		//printf("%d %s %s\n", cmd.comando, cmd.arg1, cmd.arg2);

		// Verifica se o comando é exit
		if (cmd.comando == 5) break;

		// Decodifica o comando recebido
		bzero(buff, MAX);
		strcpy(buff, decodcmd(cmd, sockfd, &login, &rmusuario));
		write(sockfd, buff, MAX);

		// Verifica se o usuário foi removido
		if (rmusuario) break;
	} 
}