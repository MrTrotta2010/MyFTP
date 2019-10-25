import gi
gi.require_version("Gtk", "3.0")
from gi.repository import Gtk

from JanelaPrincipal import JanelaPrincipal
import socket

if __name__ == "__main__":

    # Cria um socket TCP
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.setblocking(True)

    jp = JanelaPrincipal(sock)
    jp.connect("delete-event", JanelaPrincipal.clientExit, jp.socket)
    jp.show_all()

    Gtk.main()