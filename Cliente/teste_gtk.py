import gi
gi.require_version('Gtk', '3.0')
from gi.repository import Gtk

buttonClick = False

def click(widget):
    global buttonClick
    print(buttonClick)
    buttonClick = True
    Gtk.main_quit()

def main():

    janela = Gtk.Window()
    janela.connect("delete-event", Gtk.main_quit)
    button = Gtk.Button('Clique para continuar')
    button.connect("clicked", click)
    janela.add(button)
    janela.show_all()
    Gtk.main()
    janela.destroy()

    global buttonClick
    print(buttonClick)

    if buttonClick:
        janela2 = Gtk.Window()
        janela2.connect("delete-event", Gtk.main_quit)
        janela2.show_all()
        Gtk.main()

if __name__ == "__main__":
    main()