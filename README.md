# MyFTP
Implementação de um protocolo FTP como trabalho prático para a disciplina de Redes 2019/2 da UFSJ.

## Requisitos:
* C e Python3

## Comandos disponíveis:
* login _usuario senha_ : faz login no servidor
* ls: não recebe nenhum argumento e exibe os arquivos adicionados ao servidor pelo usuário e seus tamanhos em bytes
* put _arquivo_: transefere um arquivo de nome _arquivo_ ao servidor.
  **Obs**: o arquivo a ser transferido precisa estar na mesma pasta que o programa ftpclient.py. Passe somente o nome do arquivo a ser enviado; passar o endereço completo reultará em comportamento inesperado
* get _arquivo_: transefere um arquivo de nome _arquivo_ de volta ao cliente.
  **Obs**: assim, como no comano _put_, deve-se passar somente o nome do arquivo e, não, seu caminho completo
* exit: encerra a sessão do usuário e finaiza o programa cliente.

## Para rodar:
* Servidor: Compilar com `$ make` e rodar com `$ ./ftpserver <porta>`
* Cliente: `$ python ftpclient.py <ip_servidor> <porta>`

## Funcionamento geral:
    O programa do servidor, ao ser rodado, criará um socket e o atrelará à porta passada como argumento na execução e, em seguida, aguardará conexões. Com o servidor rodando, o programa do cliente pode ser executado diversas vezes para acessar os arquivos de diferentes usuários.

    Ao iniciar o cliente, ele se conectará ao IP e à porta expecíficados na execução e aguardará o login do usuário. O usuário não será capaz de executar nenhum comando sem antes fazer login. Feito o login, o usuário poderá enviar e recuperar qualquer número de arquivos do servidor, bem como listar os arquivos que estão lá. Vale ressaltar, no entanto, que um determinado usuário terá acesso somente aos seus arquivos no servidor e seus arquivos serão invisíveis aos outros usuários, isso porque cada usuário tem uma pasta associada a si no servidor, bem como um arquivo de controle que armazena os arquivos contidos em sua pasta.

    Para encerrar a sessão, o usuário deve digitar o comando exit, que encerrará o programa do cliente. Nesta versão do MyFTP, os usuários devem ser criados manualmente, como explicado a seguir.

#### Criação de usuários:
* Escolhidos um nome de usuário e uma senha, deve-se editar o arquivo _Servidor/Dados/usuarios.data_, adicionado uma linha que contém tais informações, nessa ordem, separados por espaço. Ex: _fulano 12345_.
* Em seguida, o usuário deve adicionar à pasta _Servidor/Dados/_ um arquivo de nome "usuario_files.data". Ex: "fulano_files.data".
* Por fim, dentro do diretório _Servidor/Arquivos/_ deve ser adicionada uma pasta cujo nome seja o nome do usuário. Ex: _Servidor/Arquivos/fulano/_

Para remover o usuário, todas as alterações acima devem ser desfeitas.