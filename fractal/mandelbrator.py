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
                                     False, 8, 800, 600)
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
		self.update_fractal()

	def on_file_save(self, widget, data=None):
		if not self.filename:
			self.on_file_save_as(widget)
		else:
			save_image(filename)
		
	def on_file_save_as(self, widget, data=None):
		pass
		# run dialog

	def update_fractal(self):
		xlo = -2.0
		xhi = 1.5
		ylo = -1.2
		yhi = ylo + self.height * (xhi - xlo) / self.width

		new = self.pixbuf.copy()
		news = new.get_pixels()
		res = mandelbrator.mandelbrot(news, self.width, self.height,
                                      xlo, xhi, ylo, yhi)
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
