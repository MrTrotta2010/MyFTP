import gi
gi.require_version("Gtk", "3.0")
from gi.repository import Gtk

p = 0

def clicou(widget, progress):
    global p
    p += 0.1
    if p > 1:
        p = 0
    progress.set_fraction(p)

def main ():
    w = Gtk.Window()
    w.set_default_size(300, 200)
    w.connect("delete-event", Gtk.main_quit)

    p = Gtk.ProgressBar()
    p.set_fraction(0.0)
    b = Gtk.Box()
    b2 = Gtk.Button('Click')
    b2.connect("clicked", clicou, p)

    b.add(p)
    b.add(b2)
    w.add(b)

    w.show_all()
    Gtk.main()

if __name__ == "__main__":
    main()