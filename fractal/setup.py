from distutils.core import setup, Extension

module1 = Extension("mandelbrator", sources = ["mandelbrator.c"])

setup(name = "mandelbrator",
      version = "0.9.0",
      ext_modules = [module1])
