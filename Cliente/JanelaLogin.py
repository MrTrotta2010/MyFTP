import gi
gi.require_version("Gtk", "3.0")

from gi.repository import Gtk
import ftpclient
from misc import PopUp

class JanelaLogin(Gtk.Window):

    def __init__(self, socket):

        Gtk.Window.__init__(self, title="MyFTP")
        self.set_default_size(320, 240)

        self.socket = socket
        self.logado = False
    
        self.entradaLogin = Gtk.Entry()
        self.entradaLogin.set_text("trotta")
        self.entradaSenha = Gtk.Entry()
        self.entradaSenha.set_text("3.141592")

        self.botaoLogin = Gtk.Button("Entrar")
        self.botaoCadastro = Gtk.Button("Cadastrar-se")
        self.botaoLogin.connect("clicked", self.click, socket)
        self.botaoCadastro.connect("clicked", self.click, socket)

        self.grid = Gtk.Grid()
        self.grid.set_column_spacing(10)
        self.grid.set_row_spacing(5)
        self.grid.attach(Gtk.Label("Usuário: "), 1, 1, 1, 1)
        self.grid.attach(self.entradaLogin, 2, 1, 1, 1)
        self.grid.attach(Gtk.Label("Senha: "), 1, 2, 1, 1)
        self.grid.attach(self.entradaSenha, 2, 2, 1, 1)
        self.grid.attach(self.botaoLogin, 3, 1, 1, 1)
        self.grid.attach(self.botaoCadastro, 3, 2, 1, 1)

        #Preenchendo espaço
        self.grid.attach(Gtk.Label(''), 0, 0, 3, 1)
        self.grid.attach(Gtk.Label(''), 0, 3, 3, 1)
        self.grid.attach(Gtk.Label(''), 0, 0, 1, 2)
        self.grid.attach(Gtk.Label(''), 4, 0, 1, 2)

        box = Gtk.Box(orientation=Gtk.Orientation.VERTICAL)
        box.add(Gtk.Label(""))
        box.add(Gtk.Image.new_from_file("Imagens/logo.png"))
        box.add(self.grid)

        self.add(box)
    
    def click(self, widget, socket):

        if widget.get_label() == "Cadastrar-se":
            ftpclient.guicmd('6', self.entradaLogin.get_text(), self.entradaSenha.get_text(), self.socket)
            resposta = (self.socket.recv(ftpclient.MAX)).decode()
            resposta = resposta.split('\x00')[0]
            p = PopUp(self, resposta)
            p.run()
            p.destroy()

        elif widget.get_label() == "Entrar":
            print('Fez login')
            ftpclient.guicmd('1', self.entradaLogin.get_text(), self.entradaSenha.get_text(), self.socket)
            resposta = (self.socket.recv(ftpclient.MAX)).decode()
            resposta = resposta.split('\x00')[0]

            if 'Login efetuado com sucesso!' in resposta:
                self.logado = True
                Gtk.main_quit()
            else:
                p = PopUp(self, resposta)
                p.run()
                p.destroy()