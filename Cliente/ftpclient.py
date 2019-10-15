import socket
import sys
import os

MAX = 80

# Esta função recebe um arquivo e retorna seu tamanho
def tamarq (arquivo):

    tam = 0
    try:
        tam = os.path.getsize(arquivo)
    except:
        tam = -1

    return tam

# Função que recebe a entrada do usuário, faz o parsing e devolve o comando e os argumentos
def codcomando (entrada):

    aux = entrada.split()
    comando = -1

    if aux[0] == 'login':
        comando = 1

    elif aux[0] == 'put':
        comando = 2

    elif aux[0] == 'get':
        comando = 3

    elif aux[0] == 'ls':
        comando = 4

    else:
        return comando, ' ', ' '

    if len(aux) == 1:
        if comando != 4:
            comando = -2
        aux += [' ', ' ']

    elif len(aux) == 2:
        if comando == 3:
            aux.append(' ')
        elif comando == 2:
            tam = tamarq(aux[1])
            if tam < 0:
                comando = -3
                aux.append(' ')
            else:
                aux.append(str(tam))
        else:
            comando = -2
            aux.append(' ')
    
    elif len(aux) == 3:
        if comando > 1:
            comando = -2
    
    else:
        comando = -2

    return comando, aux[1], aux[2]

# Envia um arquivo para o servidor
def envia (arquivo, tamanho, sock):
    
    try:
        arq = open(arquivo, "rb")
        dados = arq.read()
        arq.close()
        sock.sendall(dados)
        os.remove(arquivo)
    
    except:
        return 1
    
    return 0

# Recebe um arquivo do servidor
def recebe (arquivo, sock):

    # Recebe o tamanho do arquivo
    tamanho = (sock.recv(MAX)).decode()

    # Gambiarra para tirar os caracteres inválidos e deixar somente o tamanho do arquivo
    tamanho = tamanho.split('\x00')[0]
    tamanho = int(tamanho)

    if tamanho == -1:
        return

    # Recebe o arquivo como uma string e salva num arquivo
    arqstr = (sock.recv(tamanho)).decode()
    
    try:
        arq = open(arquivo, "w")
        arq.write(arqstr)
        arq.close()
    except:
        os.remove(arquivo)
        return
    

def ftp (sock):

    while (True):
        # Pega o comando
        entrada = ''
        while (entrada == ''):
            entrada = input('>> ')

        # Verifica o comando exit, que encerra a conexão
        if (entrada == 'exit'):
            print('Encerrando conexão...')
            sock.close()
            break
        
        # Codifica o comando
        comando, arg1, arg2 = codcomando(entrada)

        # Envia os dados e agurda resposta
        try:
            #print('Comando:',comando,' - Args:', arg1, arg2)
            sock.sendall((str(comando)).encode())
            sock.recv(MAX)

            sock.sendall((arg1).encode())
            sock.recv(MAX)

            sock.sendall((arg2).encode())
            sock.recv(MAX)

        except:
            print("Servidor desconectado...")
            sock.close()
            break

        # Se o comando for put, envia o arquivo
        if comando == 2:
            if envia(arg1, arg2, sock) != 0:
                print("Falha na transferência!")

        # Se o comando for get, recebe o arquivo#include <sys/_types.h>
        if comando == 3:
            recebe(arg1, sock)

        resposta = (sock.recv(MAX)).decode()

        if comando == 4:
            temp = ['\u251C '+x for x in (resposta.split('\n'))[:-1]]
            if len(temp) > 0:
                print('\u256D\n'+'\n'.join(temp)+'\n\u2570\n')
            else:
                print(resposta, '\n')

        else:
            print(resposta, '\n')

if __name__ == "__main__":

    print('CLIENTE EM PYTHON!\n')

    # Cria um socket TCP
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    # Conecta ao servidor
    endereco = (sys.argv[1], int(sys.argv[2]))

    try:
        sock.connect(endereco)

    except:
        print('Falha na conexão!')
        exit(1)

    print('Conectado a %s na porta %s' % endereco)
    ftp(sock)
