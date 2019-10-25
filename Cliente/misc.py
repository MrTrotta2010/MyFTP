import gi
gi.require_version('Gtk', '3.0')
from gi.repository import Gtk

class BarraTarefas (Gtk.MenuBar):

    def __init__(self):

        hb = Gtk.HeaderBar()
        hb.set_show_close_button(True)
        hb.props.title = "HeaderBar example"
        self.set_titlebar(hb)

        button = Gtk.Button()
        icon = Gio.ThemedIcon(name="mail-send-receive-symbolic")
        image = Gtk.Image.new_from_gicon(icon, Gtk.IconSize.BUTTON)
        button.add(image)
        hb.pack_end(button)
        
        box = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL)
        Gtk.StyleContext.add_class(box.get_style_context(), "linked")

        button = Gtk.Button()
        button.add(Gtk.Arrow(Gtk.ArrowType.LEFT, Gtk.ShadowType.NONE))
        box.add(button)

        button = Gtk.Button()
        button.add(Gtk.Arrow(Gtk.ArrowType.RIGHT, Gtk.ShadowType.NONE))
        box.add(button)

        hb.pack_start(box)
        
        self.add(Gtk.TextView())

# Classe geral de PopUp
class PopUp (Gtk.Dialog):

    # O construtor recebe o tipo de PopUp a ser instanciado
    def __init__ (self, pai, texto):

        Gtk.Dialog.__init__(self, "MyFTP", pai, Gtk.DialogFlags.MODAL)

        self.set_default_size(200,80)
        self.set_border_width(30)

        textoLabel = Gtk.Label(texto)

        textoLabel.set_line_wrap(True)
        self.get_content_area().add(textoLabel)

        self.show_all()

class MudaSenhaDialog(Gtk.Dialog):

    # O construtor recebe o tipo de PopUp a ser instanciado
    def __init__ (self, pai):

        Gtk.Dialog.__init__(self, "Alterar senha?", pai, Gtk.DialogFlags.MODAL,
            (Gtk.STOCK_OK, Gtk.ResponseType.OK))

        self.set_default_size(200,80)
        self.set_border_width(30)

        senhaLabel = Gtk.Label("Nova senha:")
        senhaLabel2 = Gtk.Label("Repita a senha:")
        self.senhaEntry = Gtk.Entry()
        self.senhaEntry2 = Gtk.Entry()
        grid = Gtk.Grid()
        grid.attach(senhaLabel, 0, 0, 1, 1)
        grid.attach(self.senhaEntry, 1, 0, 1, 1)
        grid.attach(senhaLabel2, 0, 1, 1, 1)
        grid.attach(self.senhaEntry2, 1, 1, 1, 1)
        self.get_content_area().add(grid)

        self.show_all()

class CertezaDialog(Gtk.Dialog):

    def __init__(self, pai):

        Gtk.Dialog.__init__(self, "Aviso!", pai, 0,
            (Gtk.STOCK_CANCEL, Gtk.ResponseType.CANCEL,
             Gtk.STOCK_OK, Gtk.ResponseType.OK))

        self.set_default_size(200, 100)

        label = Gtk.Label("Deseja mesmo deletar seu usuário?")

        box = self.get_content_area()
        box.add(label)
        self.show_all()