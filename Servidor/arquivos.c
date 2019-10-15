#include "ftpserver.h"

// Esta função recebe um arquivo e retorna seu tamanho em bytes
unsigned tamarq(FILE *arq) {

	fseek(arq, 0, SEEK_END);
	unsigned tamanho = ftell(arq);
	fseek(arq, 0, SEEK_SET);
	return tamanho;	
}

// Essa função recebe o nome de um arquivo e, caso ele exista na base de dados do servidor, busca e retorna seu tamanho em bytes
char *encontrafsize(char *arquivo, unsigned *tamanho) {

	FILE *fp = fopen("Dados/files.data", "r");
	int cont = 0;
	char *aux = malloc(MAX);
	bzero(aux, MAX);

	for (int i = 0; strcmp(aux, arquivo) != 0; i++) {
		fscanf(fp, "%s\n", aux);
		cont++;
	}

	fclose(fp);
	fp = fopen("Dados/fsize.data", "r");

	for (int i = 0; i <= cont; i++)
		fscanf(fp, "%s\n", aux);
	
	fclose(fp);

	*tamanho = atoi(aux);

	return aux;
}

//Esta função adiciona uma entrada à lista de arquivos
int adcarq(char *arquivo, char *tamanho) {
	
	int existe = FALSE;
	char n[MAX];
    bzero(n, MAX);
	
	// Verifica se o arquivo já estava na lista de arquivos
	FILE *fp = fopen("Dados/files.data", "r");
	if (fp) {
		while (!feof(fp)) {
			fscanf(fp, "%s\n", n);
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
		fp = fopen("Dados/files.data", "a");
		if (fp) {
			fprintf(fp, "%s\n", arquivo);
			fclose(fp);
		} else {
			fclose(fp);
			return 1;
		}

		fp = fopen("Dados/fsize.data", "a");
		if (fp) {
			fprintf(fp, "%s\n", tamanho);
			fclose(fp);
		} else {
			fclose(fp);
			return 1;
		}
	}

	return 0;
}

// Esta função remove uma entrada na lista de arquivos
int remarq(char *arquivo) {

	FILE *alvo = fopen("Dados/files.data", "r");
	unsigned tamanho = tamarq(alvo) - strlen(arquivo);

	char arqstr[tamanho], aux[MAX];
	int cont = 0, achou = FALSE;

	bzero(arqstr, tamanho);
	bzero(aux, MAX);

	if (alvo) {
		// Percorre o arquivo antigo copiando para uma string as entradas diferentes do nome do arquivo
		while (!feof(alvo)) {
			fscanf(alvo, "%s\n", aux);
			if (strcmp(arquivo, aux) != 0 && !achou) {
				strcat(arqstr, aux);
				cont++; //Registra a linha onde estava o arquivo
			} else {
				achou = TRUE;
			}
		}
		fclose(alvo);

	} else {
		fclose(alvo);
		return 1;
	}

    if (achou) {
        // Sobrescreve o arquivo com a nova lista
        alvo = fopen("Dados/files.data", "w");
        if (alvo) {
            fprintf(alvo, "%s", arqstr);
            fclose(alvo);
        } else {
            fclose(alvo);
            return 1;
        }

        // Abre o arquivo de tamanhos
        alvo = fopen("Dados/fsize.data", "r");
        FILE *temp = fopen("Dados/temp", "w");

        if (alvo) {
            // Percorre o arquivo de tamanhos até a linha onde estáo tamanho do arquivo
            for (int i = 0; !feof(alvo); i++) {
                fscanf(alvo, "%s\n", aux);
                if (i != cont) {
                    fprintf(temp, "%s\n", aux);
                }
            }
            fclose(alvo);
            fclose(temp);
        } else {
            fclose(alvo);
            fclose(temp);
            return 1;
        }

        // Apaga o arquivo antigo e renomeia o novo
        remove("Dados/fsize.data");
        rename("Dados/temp", "Dados/fsize.data");
  
    } else {
        return 1;
    }

	return 0;
}