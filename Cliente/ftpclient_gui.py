import gi
gi.require_version("Gtk", "3.0")
from gi.repository import Gtk

from JanelaPrincipal import JanelaPrincipal
from JanelaLogin import JanelaLogin
import socket
import ftpclient as ftpc
import sys

sair = False

def clientExit (self, widget, socket):
    ftpc.guicmd('5', 'arg1', 'arg2', self.socket)
    global sair
    sair = True
    Gtk.main_quit() 

def main(host, port):

    while (True):
        # Cria um socket TCP
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.setblocking(True)
        ftpc.guiconnect(
            (host, port), sock
        )

        global sair
        logado = False

        jl = JanelaLogin(sock)
        jl.connect("delete-event", clientExit, sock)
        jl.show_all()
        Gtk.main()

        logado = jl.logado
        user = jl.entradaLogin.get_text()
        senha = jl.entradaSenha.get_text()
        jl.destroy()

        if logado:
            jp = JanelaPrincipal(sock, user, senha)
            jp.connect("delete-event", clientExit, sock)
            jp.show_all()
            Gtk.main()
            jp.destroy()

        print(sair)
        
        if sair:
            break
        else:
            ftpc.guicmd('5', 'arg1', 'arg2', sock)
            sock.close()
            
    sock.close()


if __name__ == "__main__":
    if len(sys.argv) == 3:
        main(sys.argv[1], int(sys.argv[2]))
    else:
        print('Digite o host e a porta')