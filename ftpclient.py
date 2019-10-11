import socket
import sys
import os

MAX = 80

def tamarq (arquivo):
    return os.path.getsize(arquivo)

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

    if len(aux) == 1:
        if comando != 4:
            comando = -2
        aux += [' ', ' ']

    elif len(aux) == 2:
        if comando == 3:
            aux.append(' ')
        elif comando == 2:
            aux.append(str(tamarq(aux[1])))
        else:
            comando = -2
            aux.append(' ')
    
    elif len(aux) == 3:
        if comando > 1:
            comando = -2
    
    else:
        comando = -2

    return comando, aux[1], aux[2]

def envia (arquivo, tamanho, sock):
    
    try:
        arq = open(arquivo, "rb")
        dados = arq.read()
        arq.close()
        sock.sendall(dados)
    
    except:
        return 1
    
    return 0

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
        
        if comando == -1:
            print('Comando inválido!\n')
        elif comando == -2:
            print('Argumentos incorretos!\n')

        else: 
            # Envia os dados e agurda resposta
            try:
                sock.sendall((str(comando)).encode())
                sock.recv(MAX)

                sock.sendall((arg1).encode())
                sock.recv(MAX)

                sock.sendall((arg2).encode())
                sock.recv(MAX)

            except:
                print("Servidor desconectado...")
                print('closing socket')
                sock.close()
                break

            # Se o comando for put, envia o arquivo
            if comando == 2:
                if envia(arg1, arg2, sock) != 0:
                    print("Falha no envio do arquivo!")

            resposta = (sock.recv(MAX)).decode()

            if comando == 4:
                resposta = '\u256D\n'+'\n'.join(['\u251C '+x for x in resposta.split('\n')])+'\n\u2570'

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
