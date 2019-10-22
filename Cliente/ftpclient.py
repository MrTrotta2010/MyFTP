import socket
import sys
import os

MAX = 256

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

    aux = entrada.replace('\n', '')
    aux = aux.split()
    comando = -1

    if aux[0] == 'login':
        comando = 1

    elif aux[0] == 'put':
        comando = 2

    elif aux[0] == 'get':
        comando = 3

    elif aux[0] == 'ls':
        comando = 4

    elif aux[0] == 'exit':
        comando = 5

    elif aux[0] == 'adduser':
        comando = 6

    elif aux[0] == 'rmuser':
        comando = 7

    elif aux[0] == 'passwd':
        comando = 8

    else:
        return comando, ' ', ' '

    if len(aux) == 1:
        if comando != 4 and comando != 5:
            comando = -2
        aux += [' ', ' ']

    elif len(aux) == 2:
        if comando in [3, 8, 7]:
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
        if comando not in [1, 6]:
            comando = -2
    
    else:
        comando = -2

    return comando, aux[1], aux[2]

# Envia um arquivo para o servidor
def envia (arquivo, tamanho, sock):
    
    try:
        arq = open(arquivo, "rb")
        brestantes = int(tamanho)

        while brestantes > 0:
            # Leio um pedaço do arquivo
            # O tamanho do pedaço será o tamanho do meu chunk ou a quantidade de bites restantes do arquivo
            dados = arq.read(min(MAX, int(brestantes)))
            sock.sendall(dados)
            brestantes -= len(dados)
            aux = (float(tamanho)-brestantes)/float(tamanho)*100
            #print("Enviando... - "+str(aux)[:3]+"%")
            print("Enviando... - "+"{0:.2f}".format(aux)+"%", end='\r')
            

        arq.close()
    
    except Exception as e:
        print(e)
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

    brestantes = tamanho
    brecebidos = 0

    try:
        arq = open(arquivo, "wb")

        # Recebe o arquivo em pedaços e salva em outro arquivo
        while brestantes > 0:
            dados = sock.recv(min(MAX, brestantes))
            blidos = len(dados)

            # Salva os dados recebidos no arquivo criado
            arq.write(dados)
            brestantes -= blidos
            brecebidos += blidos
            aux = brecebidos/tamanho*100
            print("Recebendo... - "+"{0:.2f}".format(aux)+"%", end='\r')
    
        arq.close()

    except Exception as e:
        print(e)
        os.remove(arquivo)
        return
    
def ftp (sock):

    while (True):
        # Pega o comando
        entrada = ''
        while (entrada == ''):
            entrada = input('>> ')
        
        # Codifica o comando
        comando, arg1, arg2 = codcomando(entrada)

        # Envia os dados e agurda resposta
        try:
            #print('Comando:',comando,' - Args:', arg1, arg2)
            aux = str(comando).replace('\0', '')
            aux += ((MAX-len(aux)-1)*'\0')+'\0'
            sock.sendall(aux.encode())

            aux = arg1.replace('\0', '')
            aux += ((MAX-len(aux)-1)*'\0')+'\0'
            sock.sendall((aux).encode())

            aux = arg2.replace('\0', '')
            aux += ((MAX-len(aux)-1)*'\0')+'\0'
            sock.sendall((aux).encode())

        except:
            print("Servidor desconectado...")
            sock.close()
            break

        # Verifica o comando exit, que encerra a conexão
        if (entrada == 'exit'):
            print('Encerrando conexão...')
            break

        # Se o comando for put, envia o arquivo
        if comando == 2:
            if envia(arg1, arg2, sock) != 0:
                print("Falha na transferência!")

        # Se o comando for get, recebe o arquivo
        elif comando == 3:
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

        # Se o usuário foi removido, encerra o programa
        if comando == 7:
            break

if __name__ == "__main__":

    print('CLIENTE EM PYTHON!\n')

    # Cria um socket TCP
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.setblocking(True)

    # Conecta ao servidor
    if len(sys.argv) != 3:
        print('Modo de uso: $ python ftpclient.py <ip_servidor> <porta>')
        exit(2)
        
    endereco = (sys.argv[1], int(sys.argv[2]))

    try:
        sock.connect(endereco)

    except:
        print('Falha na conexão!')
        exit(1)

    print('Conectado a %s na porta %s' % endereco)
    ftp(sock)
