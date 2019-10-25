import gi
gi.require_version("Gtk", "3.0")
from gi.repository import Gtk, Gdk, Gio
import ftpclient
import os
from misc import PopUp, BarraTarefas, CertezaDialog, MudaSenhaDialog

MAX = 256

class JanelaPrincipal(Gtk.Window):

    def __init__(self, socket, user, senha):

        Gtk.Window.__init__(self, title="MyFTP")
        self.maximize()
        self.set_icon_from_file("Imagens/icon.png")

        self.socket = socket   
        self.arquivo = ''
        self.arquivo_path = ''
        self.senha = senha

        hb = Gtk.HeaderBar()
        hb.set_show_close_button(True)
        hb.props.title = user
        self.set_titlebar(hb)

        button = Gtk.Button()
        icon = Gio.ThemedIcon(name="open-menu-symbolic")
        image = Gtk.Image.new_from_gicon(icon, Gtk.IconSize.BUTTON)
        button.add(image)
        button.connect("clicked", self.clickMenu)
        hb.pack_start(button)

        bt1 = Gtk.ModelButton("Alterar senha")
        bt1.connect("clicked", self.clickMenuItem)
        bt2 = Gtk.ModelButton("Excluir usuário")
        bt2.connect("clicked", self.clickMenuItem)
        bt3 = Gtk.ModelButton("Sair")
        bt3.connect("clicked", self.clickMenuItem)

        self.popover = Gtk.Popover()
        vbox = Gtk.Box(orientation=Gtk.Orientation.VERTICAL)
        vbox.pack_start(bt1, False, True, 10)
        vbox.pack_start(bt2, False, True, 10)
        vbox.pack_start(bt3, False, True, 10)
        self.popover.add(vbox)
        self.popover.set_position(Gtk.PositionType.BOTTOM)

        self.botaoArquivo = Gtk.Button("Escolha um arquivo para enviar")
        self.botaoArquivo.set_hexpand(True)
        self.labelArquivo = Gtk.Label("Nenhum arquivo selecionado")
        self.labelArquivo.set_hexpand(True)
        self.labelArquivo.set_justify(Gtk.Justification.LEFT)
        self.botaoEnviar = Gtk.Button()
        self.botaoEnviar.add(
            Gtk.Image.new_from_gicon(
                Gio.ThemedIcon(name="document-send-symbolic"),
                Gtk.IconSize.DIALOG
            )
        )
        self.botaoEnviar.set_hexpand(True)

        self.botaoArquivo.connect("clicked", self.adcArquivo, self.socket)
        self.botaoEnviar.connect("clicked", self.envArquivo, self.socket)

        arqFrame = Gtk.Frame(label=' Arquivo: ')
        arqFrame.set_label_align(0.1, 0.5)
        arqFrame.add(self.labelArquivo)

        self.dropArea = Gtk.Image()
        self.dropArea.set_vexpand(True)
        self.dropArea.set_hexpand(True)
        self.dropArea.set_from_file('Imagens/drop_area.png')
        enforce_target = Gtk.TargetEntry.new('text/plain', Gtk.TargetFlags(4), 129)
        self.dropArea.drag_dest_set(Gtk.DestDefaults.ALL, [enforce_target], Gdk.DragAction.COPY)
        self.dropArea.connect("drag-data-received", self.soltaArquivo)

        self.barraProg = Gtk.ProgressBar()
        self.barraProg.set_text('Aguardando...')
        self.barraProg.set_show_text(True)

        self.grid = Gtk.Grid()

        self.grid.set_column_spacing(10)
        self.grid.set_row_spacing(10)
        self.grid.attach(self.botaoArquivo, 1, 1, 2, 1)
        self.grid.attach(arqFrame, 1, 2, 1, 1)
        self.grid.attach(self.botaoEnviar, 2, 2, 1, 1)
        self.grid.attach(self.dropArea, 1, 3, 2, 1)
        self.grid.attach(self.barraProg, 1, 4, 2, 1)

        #Preenchendo espaço
        p1 = Gtk.Label('')
        #Preenchendo espaço
        p2 = Gtk.Label('')
        p2.set_vexpand(True)
        #Preenchendo espaço
        p3 = Gtk.Label('')
        #Preenchendo espaço
        p4 = Gtk.Label('')
        self.grid.attach(p1 , 0, 0, 2, 1)
        self.grid.attach(p2, 0, 5, 2, 1)
        self.grid.attach(p3, 0, 0, 1, 3)
        self.grid.attach(p4, 3, 0, 1, 3)

        self.pan = Gtk.Paned(orientation=Gtk.Orientation.HORIZONTAL)
        self.pan.set_position(500)
        print('pan', self.pan.get_position())
        frame1 = Gtk.Frame()
        frame2 = Gtk.Frame()
        frame1.set_shadow_type(Gtk.ShadowType.ETCHED_IN)
        frame2.set_shadow_type(Gtk.ShadowType.ETCHED_IN)
        frame1.add(self.grid)

        self.pan.pack1(frame1, shrink=False)
        self.add(self.pan)

        self.constroiJanelaArquivos(frame2)

        self.show_all()

    def constroiListaArquivos(self):
        self.liststore.clear()
        ftpclient.guicmd('4', '', '', self.socket)
        resposta = (self.socket.recv(MAX)).decode()
        resposta = [x for x in (resposta.split('\n'))[:-1]]
        if len(resposta) > 0:
            for r in resposta:
                temp = r.split(' ')
                if len(temp) > 2:
                    s = temp[-1]
                    a = ' '.join(temp[0:-1])
                    self.liststore.append([a, s, False])
                else:
                    self.liststore.append([temp[0], temp[1], False])

    def constroiJanelaArquivos(self, frame2):

        self.liststore = Gtk.ListStore(str, str, bool)

        self.constroiListaArquivos()

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

        self.boxInterna = Gtk.Box(orientation=Gtk.Orientation.VERTICAL)
        self.botaoBaixar = Gtk.Button("Baixar")
        self.botaoBaixar.connect("clicked", self.clickBaixar)

        self.boxInterna.pack_start(self.treeview, True, True, 0)
        self.boxInterna.pack_start(self.botaoBaixar, False, False, 0)

        frame2.add(self.boxInterna)
        self.pan.pack2(frame2, shrink=False)

    def clickMenuItem(self, widget):

        if widget.get_label() == "Sair":
            Gtk.main_quit()

        elif widget.get_label() == "Alterar senha":
            while (True):
                d = MudaSenhaDialog(self)
                r = d.run()
                senha1 = d.senhaEntry.get_text()
                senha2 = d.senhaEntry2.get_text()
                d.destroy()
                if r == Gtk.ResponseType.OK:
                    if senha1 == '' or senha2 == '':
                        p = PopUp(self, 'Preencha todos os campos!')
                        p.run()
                        p.destroy()
                    elif senha1 != senha2:
                        p = PopUp(self, 'As senhas digitadas são diferentes!')
                        p.run()
                        p.destroy()
                    else:
                        ftpclient.guicmd('8', senha1, '', self.socket)
                        resp = self.socket.recv(MAX).decode()
                        resp = resp.split('\x00')[0]
                        p = PopUp(self, resp)
                        p.run()
                        p.destroy()
                        break
                else:
                    break

        elif widget.get_label() == "Excluir usuário":
            d = CertezaDialog(self)
            r = d.run()
            d.destroy()
            if r == Gtk.ResponseType.OK:
                ftpclient.guicmd('7', self.senha, '', self.socket)
                resp = self.socket.recv(MAX).decode()
                resp = resp.split('\x00')[0]
                p = PopUp(self, resp)
                p.run()
                p.destroy()
                ftpclient.guicmd('5', 'arg1', 'arg2', self.socket)
                Gtk.main_quit()

    def clickMenu(self, widget):
        self.popover.set_relative_to(widget)
        self.popover.show_all()
        self.popover.popup()
            
    def toggleBaixar(self, widget, path):
        self.liststore[path][2] = not self.liststore[path][2]

    def clickBaixar(self, widget):
        lista = []
        for a in self.liststore:
            if a[2] == True:
                lista.append(a[0])

        for a in lista:
            ftpclient.guicmd('3', a, '', self.socket)
            tamanho = (self.socket.recv(MAX)).decode()
            tamanho = tamanho.split('\x00')[0]
            print(tamanho)
            tamanho = int(tamanho)

            if tamanho == -1:
                return

            brestantes = tamanho
            brecebidos = 0

            try:
                arq = open('Downloads/'+a, "wb")

                # Recebe o arquivo em pedaços e salva em outro arquivo
                while brestantes > 0:
                    dados = self.socket.recv(min(MAX, brestantes))
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
                os.remove('Downloads/'+a)

            self.socket.recv(MAX)
            self.constroiListaArquivos()
            self.show_all()

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

        self.labelArquivo.set_text(
            'Nome: '+self.arquivo+'\nTamanho: '+str(os.path.getsize(self.arquivo_path))+' bytes'
        )

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
            
            self.socket.recv(MAX)
        self.constroiListaArquivos()
        self.show_all()

    def soltaArquivo(self, widget, drag_context, x, y, data, info, time):
        self.arquivo_path = data.get_text().replace('file://', '').replace('\n', '')
        self.arquivo = self.arquivo_path.split('/')[-1]
        self.labelArquivo.set_text(self.arquivo+' - '+str(os.path.getsize(self.arquivo_path))+' bytes') 
