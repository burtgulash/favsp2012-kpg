#include <Python.h>
#include <stdlib.h>


static PyObject *
mandelbrot(PyObject *self, PyObject *args);


static PyMethodDef mandelbrator_methods[] = {
	{"mandelbrot", mandelbrot, METH_VARARGS, "Mandelbrot plot"},
	{NULL, NULL, 0, NULL}
};



PyMODINIT_FUNC
initmandelbrator(void)
{
	(void) Py_InitModule("mandelbrator", mandelbrator_methods);
}


int
main(int argc, char **argv)
{
	Py_SetProgramName(argv[0]);
	Py_Initialize();
	initmandelbrator();
	Py_Exit(0);
	return 0;
}

static void
mandelbrot_iterations(unsigned *buf, int w, int h,
                      double xlo, double xhi, double ylo, double yhi)
{
#define ITERATIONS 200
#define ESCAPE 16

	int x, y;
	double re_0, im_0, re, im;
	double abs, tmp;
	unsigned i;

	for (y = 0; y < h; y++) {
		for (x = 0; x < w; x++) {
			re_0 = re = xlo + (((xhi - xlo) * (double) x) / (double) w);
			im_0 = im = ylo + (((yhi - ylo) * (double) y) / (double) h);

			i = 0;
			abs = re * re + im * im;
			while (i < ITERATIONS && abs < ESCAPE) {
				tmp = re * re - im * im + re_0;
				im = 2 * re * im + im_0;
				re = tmp;

				abs = re * re + im * im;
				i++;
			}

			buf[y * w + x] = i;
		}
	}
}


static PyObject *
mandelbrot(PyObject *self, PyObject *args)
{
	int x, y, channel;
	int len, width, height, n_chans;
	unsigned char *buf;
	unsigned *fractal_buffer;

	if (!PyArg_ParseTuple(args, "s#ii", &buf, &len, &width, &height))
		return NULL;
	n_chans = 3;
	
	fractal_buffer = (unsigned*) malloc(height * width * sizeof(unsigned));
	mandelbrot_iterations(fractal_buffer, width, height, -1.5, 1.0, -1.0, 1.0);

	for (channel = 0; channel < 3; channel++)
		for (y = 0; y < height; y++)
			for (x = 0; x < width; x++)
				buf[(y * width + x) * n_chans + channel] = 
                             fractal_buffer[y * width + x];

	free(fractal_buffer);

	return Py_BuildValue("s#", buf, len);
}
