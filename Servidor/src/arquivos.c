#include "ftpserver.h"

// Esta função recebe um arquivo e retorna seu tamanho em bytes
unsigned tamarq(FILE *arq) {

	fseek(arq, 0, SEEK_END);
	unsigned tamanho = ftell(arq);
	fseek(arq, 0, SEEK_SET);
	return tamanho;	
}

// Esta função verifica se um determinado arquivo existe para um determinado usuário
int exarq(char *arquivo, char *login) {

    char arqname[2048], n[MAX], t[MAX];
    memset(arqname, '\0', 2048);
    memset(n, '\0', MAX);
    memset(t, '\0', MAX);
    strcpy(arqname, "Dados/");
    strcat(arqname, login);
    strcat(arqname, "_files.data");
	FILE *fp = fopen(arqname, "r");

    if (fp) {
        while (!feof(fp)) {
            fscanf(fp, "%s %s\n", n, t);
            if (strcmp(arquivo, n) == 0) {
                fclose(fp);
                return 1;
            }
        }
    }

    return 0;
}

// Essa função recebe o nome de um arquivo e, caso ele exista na base de dados do servidor, busca e retorna seu tamanho em bytes
char *encontrafsize(char *login, char *arquivo, unsigned *tamanho) {

    char arqname[2048];
    memset(arqname, '\0', 2048);
    strcpy(arqname, "Dados/");
    strcat(arqname, login);
    strcat(arqname, "_files.data");
	FILE *fp = fopen(arqname, "r");
	char n[MAX];
	char *aux = malloc(MAX);
	memset(aux, '\0', MAX);
	memset(n, '\0', MAX);

	while (strcmp(n, arquivo) != 0) {
		memset(aux, '\0', MAX);
		memset(n, '\0', MAX);
		fscanf(fp, "%s %s\n", n, aux);
	}

	fclose(fp);

	*tamanho = atoi(aux);

	return aux;
}

//Esta função adiciona uma entrada à lista de arquivos
int adcarq(char *login, char *arquivo, char *tamanho) {
	
	int existe = FALSE;
	char n[MAX], t[MAX];
	char arqname[2048];
    memset(n, '\0', MAX);
    memset(t, '\0', MAX);

    memset(arqname, '\0', 2048);
    strcpy(arqname, "Dados/");
    strcat(arqname, login);
    strcat(arqname, "_files.data");
	
	// Verifica se o arquivo já estava na lista de arquivos
	FILE *fp = fopen(arqname, "r");
	if (fp) {
		while (!feof(fp)) {
			fscanf(fp, "%s %s\n", n, t);
			if (strcmp(n, arquivo) == 0) {
				existe = TRUE;
				//puts("Já tem");
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
		//puts("Ainda n tem");
		fp = fopen(arqname, "a");
		if (fp) {
			fprintf(fp, "%s %s\n", arquivo, tamanho);
			fclose(fp);
		} else {
			return 1;
		}
	}

	return 0;
}

// Esta função remove uma entrada na lista de arquivos
int remarq(char *login, char *arquivo) {

    char arqname[2048];
    memset(arqname, '\0', 2048);
    strcpy(arqname, "Dados/");
    strcat(arqname, login);
    strcat(arqname, "_files.data");

	FILE *alvo = fopen(arqname, "r");
	FILE *novo = fopen("Dados/temp", "w");
	
	char n[MAX], t[MAX];

	if (alvo) {
		if (novo) {
			// Percorre o arquivo antigo copiando para uma string as entradas diferentes do nome do arquivo
			while (!feof(alvo)) {
				memset(n, '\0', MAX);
				memset(t, '\0', MAX);
				fscanf(alvo, "%s %s\n", n, t);
				if (strcmp(arquivo, n) != 0) {
					fprintf(novo, "%s %s\n", n, t);
				}
			}
			fclose(alvo);
			fclose(novo);
		} else {
			fclose(alvo);
			return 1;
		}
	} else {
		if (novo) fclose(novo);
		return 1;
	}

	// Apaga o arquivo antigo e renomeia o novo
	remove(arqname);
	rename("Dados/temp", arqname);

	return 0;
}