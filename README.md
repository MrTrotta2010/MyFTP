# MyFTP
Implementação de um protocolo FTP como trabalho prático para a disciplina de Redes 2019/2 da UFSJ.

## Requisitos:
* C e Python3

## Comandos disponíveis:
* login _usuario senha_ : faz login no servidor
* ls: não recebe nenhum argumento e exibe os arquivos adicionados ao servidor pelo usuário
* put _arquivo_: transefere um arquivo de nome <arquivo> ao servidor.
  **Obs**: o arquivo a ser transferido precisa estar na mesma pasta que o programa ftpclient.py
* get _arquivo_: transefere um arquivo de nome <arquivo> de volta ao cliente.
  
## Para rodar:
* Servidor: Compilar com `$ make` e rodar com `$ ./ftpserver <porta>`
* Cliente: `$ python ftpclient.py <ip_servidor> <porta>`
