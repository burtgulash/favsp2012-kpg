#! /usr/bin/env python

import sys, gtk
import mandelbrator

class Mandelbrator:
	def __init__(self):
		builder = gtk.Builder()
		builder.add_from_file("mandelbrator.glade")

		builder.connect_signals(self)

		self.window = builder.get_object("window")
		self.aboutdialog = builder.get_object("aboutdialog")

		self.img = builder.get_object("img")
		self.pixbuf = gtk.gdk.Pixbuf(gtk.gdk.COLORSPACE_RGB, \
                                     False, 8, 600, 500)
		self.width = self.pixbuf.get_width()
		self.height = self.pixbuf.get_height()
		self.rowstride = self.pixbuf.get_rowstride()

		self.img.set_from_pixbuf(self.pixbuf)

	def on_window_destroy(self, widget, data=None):
		gtk.main_quit()

	def on_about_activate(self, widget, data=None):
		self.aboutdialog.run()
		self.aboutdialog.hide()

	def on_file_new(self, widget, data=None):
		new = self.pixbuf.copy()
		news = new.get_pixels()
		res = mandelbrator.mandelbrot(news, self.width, self.height)
		res_buf = gtk.gdk.pixbuf_new_from_data(res, 
                                         self.pixbuf.get_colorspace(),
                                         self.pixbuf.get_has_alpha(),
                                         self.pixbuf.get_bits_per_sample(),
                                         self.width, 
                                         self.height,
                                         self.rowstride)
		self.img.set_from_pixbuf(res_buf)
		

if __name__ == "__main__":
	gui = Mandelbrator()
	gui.window.show()
	gtk.main()
