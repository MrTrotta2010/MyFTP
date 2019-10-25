import gi
gi.require_version("Gtk", "3.0")

from gi.repository import Gtk, GdkPixbuf
import ftpclient
from misc import PopUp

class JanelaLogin(Gtk.Window):

    def __init__(self, socket):

        Gtk.Window.__init__(self, title="MyFTP")
        self.set_default_size(320, 240)
        self.set_icon_from_file("Imagens/icon.png")

        self.socket = socket
        self.logado = False
    
        self.entradaLogin = Gtk.Entry()
        self.entradaLogin.set_text("trotta")
        self.entradaLogin.set_hexpand(True)
        self.entradaLogin.set_valign(Gtk.Align.CENTER)
        self.entradaSenha = Gtk.Entry()
        self.entradaSenha.set_text("3.141592")
        self.entradaSenha.set_hexpand(True)
        self.entradaSenha.set_valign(Gtk.Align.CENTER)

        self.botaoLogin = Gtk.Button("Entrar")
        self.botaoLogin.set_valign(Gtk.Align.CENTER)
        self.botaoCadastro = Gtk.Button("Cadastrar-se")
        self.botaoCadastro.set_valign(Gtk.Align.CENTER)
        self.botaoLogin.connect("clicked", self.click, socket)
        self.botaoCadastro.connect("clicked", self.click, socket)

        self.grid = Gtk.Grid()
        self.grid.set_column_spacing(10)
        self.grid.set_row_spacing(5)
        label1 = Gtk.Label("Usuário: ")
        label1.set_valign(Gtk.Align.CENTER)
        self.grid.set_row_spacing(5)
        label2 = Gtk.Label("Senha: ")
        label2.set_valign(Gtk.Align.CENTER)
        self.grid.attach(label1, 1, 1, 1, 1)
        self.grid.attach(self.entradaLogin, 2, 1, 1, 1)
        self.grid.attach(label2, 1, 2, 1, 1)
        self.grid.attach(self.entradaSenha, 2, 2, 1, 1)
        self.grid.attach(self.botaoLogin, 3, 1, 1, 1)
        self.grid.attach(self.botaoCadastro, 3, 2, 1, 1)

        #Preenchendo espaço
        p1 = Gtk.Label('')
        p1.set_vexpand(True)
        #Preenchendo espaço
        p2 = Gtk.Label('')
        p2.set_vexpand(True)
        #Preenchendo espaço
        p3 = Gtk.Label('')
        p3.set_hexpand(True)
        #Preenchendo espaço
        p4 = Gtk.Label('')
        p4.set_hexpand(True)
        self.grid.attach(p1, 0, 0, 3, 1)
        self.grid.attach(p2, 0, 3, 3, 1)
        self.grid.attach(p3, 0, 0, 1, 2)
        self.grid.attach(p4, 4, 0, 1, 2)

        box = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=10, expand=True)
        label3 = Gtk.Label("")
        label3.set_vexpand(True)
        box.add(label3)
        img = Gtk.Image.new_from_file("Imagens/logo.png")
        box.add(img)
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