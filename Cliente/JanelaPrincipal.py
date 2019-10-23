import gi
gi.require_version("Gtk", "3.0")
from gi.repository import Gtk
import ftpclient
import os

MAX = 256

class JanelaPrincipal(Gtk.Window):

    def __init__(self, socket):

        Gtk.Window.__init__(self, title="MyFTP")

        self.socket = socket
    
        self.entradaLogin = Gtk.Entry()
        self.entradaLogin.set_text("trotta")
        self.entradaSenha = Gtk.Entry()
        self.entradaSenha.set_text("3.141592")
        self.entradaHost = Gtk.Entry()
        self.entradaHost.set_text("localhost")
        self.entradaPorta = Gtk.Entry()
        self.entradaPorta.set_text("8080")

        self.botaoLogin = Gtk.Button("Entrar")
        self.botaoCadastro = Gtk.Button("Cadastrar-se")
        self.botaoLogin.connect("clicked", self.click, socket)
        self.botaoCadastro.connect("clicked", self.click, socket)

        self.grid = Gtk.Grid()
        self.grid.set_column_spacing(10)
        self.grid.set_row_spacing(5)
        self.grid.attach(self.entradaLogin, 0, 0, 2, 1)
        self.grid.attach(self.entradaHost, 2, 0, 2, 1)
        self.grid.attach(self.entradaSenha, 0, 1, 2, 1)
        self.grid.attach(self.entradaPorta, 2, 1, 2, 1)
        self.grid.attach(self.botaoCadastro, 0, 2, 1, 1)
        self.grid.attach(self.botaoLogin, 1, 2, 1, 1)

        self.box = Gtk.Box(spacing=6)
        self.box.pack_start(self.grid, True, True, 0)
        self.add(self.box)

    def constroiTelaPrincipal(self):

        self.arquivo = ''
        self.arquivo_path = ''

        self.botaoArquivo = Gtk.Button("Escolha um arquivo para enviar")
        self.labelArquivo = Gtk.Label("Nenhum arquivo selecionado")
        self.botaoEnviar = Gtk.Button("Enviar")

        self.botaoArquivo.connect("clicked", self.adcArquivo, self.socket)
        self.botaoEnviar.connect("clicked", self.envArquivo, self.socket)

        self.grid.remove_row(0)
        self.grid.remove_row(0)
        self.grid.remove_row(0)

        self.grid.set_column_spacing(10)
        self.grid.set_row_spacing(5)
        self.grid.attach(self.botaoArquivo, 0, 0, 2, 1)
        self.grid.attach(self.labelArquivo, 0, 1, 1, 1)
        self.grid.attach(self.botaoEnviar, 1, 1, 1, 1)

    def constroiJanelaArquivos(self):

        self.liststore = Gtk.ListStore(str, str, bool)

        self.liststore.append(["Debian", 'tamanho', False])
        self.liststore.append(["OpenSuse", 'tamanho', False])
        self.liststore.append(["Fedora", 'tamanho', False])

        self.treeview = Gtk.TreeView(model=self.liststore)

        texto1 = Gtk.CellRendererText()
        coluna1 = Gtk.TreeViewColumn("Arquivo", texto1, text=0)
        self.treeview.append_column(coluna1)

        texto2 = Gtk.CellRendererText()
        coluna2 = Gtk.TreeViewColumn("Tamanho", texto2, text=1)
        self.treeview.append_column(coluna2)

        toggle = Gtk.CellRendererToggle()
        toggle.connect("toggled", self.toggleBaixar)
        coluna3 = Gtk.TreeViewColumn("Baixar", toggle, active=2)
        self.treeview.append_column(coluna3)

        self.box.pack_start(self.treeview, True, True, 0)

    def click(self, widget, socket):

        if widget.get_label() == "Cadastrar-se":
            print('Cadastrou')

        elif widget.get_label() == "Entrar":
            
            print('Fez login')
            endereco = (self.entradaHost.get_text(), int(self.entradaPorta.get_text()))
            ftpclient.guiconnect(endereco, self.socket)

            ftpclient.guicmd('1', self.entradaLogin.get_text(), self.entradaSenha.get_text(), self.socket)
            resposta = (self.socket.recv(MAX)).decode()
            print(resposta)

            if 'Login efetuado com sucesso!' in resposta:
                self.constroiTelaPrincipal()
                self.constroiJanelaArquivos()
                self.show_all()
            
    def toggleBaixar(self, widget, path):
        self.liststore[path][2] = not self.liststore[path][2]

    def adcArquivo(self, widget, socket):
        print('Escolha o arquivo')
        escolher = Gtk.FileChooserDialog("Escolha um arquivo", self, Gtk.FileChooserAction.OPEN,
                ("Cancelar", Gtk.ResponseType.CANCEL, "Abrir", Gtk.ResponseType.OK))
        resposta = escolher.run()

        if resposta == Gtk.ResponseType.OK:
            self.arquivo_path = escolher.get_filename()
            self.arquivo = self.arquivo_path.split('/')[-1]
            print(self.arquivo)

        escolher.destroy()

        self.labelArquivo.set_text(self.arquivo)

    def envArquivo(self, widget, socket):

        if self.arquivo != '':
            tamanho = os.path.getsize(self.arquivo_path)
            ftpclient.guicmd('2', self.arquivo, str(tamanho), self.socket)

            try:
                arq = open(self.arquivo_path, "rb")
                brestantes = int(tamanho)

                while brestantes > 0:
                    # Leio um pedaço do arquivo
                    # O tamanho do pedaço será o tamanho do meu chunk ou a quantidade de bites restantes do arquivo
                    dados = arq.read(min(MAX, int(brestantes)))
                    self.socket.sendall(dados)
                    brestantes -= len(dados)
                    aux = (float(tamanho)-brestantes)/float(tamanho)*100
                    #print("Enviando... - "+str(aux)[:3]+"%")
                    print("Enviando... - "+"{0:.2f}".format(aux)+"%", end='\r')
                    

                arq.close()
            
            except Exception as e:
                print(e)

    def clientExit (self, widget, socket):
        ftpclient.guicmd('5', 'arg1', 'arg2', self.socket)
        Gtk.main_quit()  
