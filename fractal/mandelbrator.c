#include <Python.h>
#include <math.h>
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

#define ITERATIONS 1000
static void
mandelbrot_iterations(double *buf, int w, int h,
                      double xlo, double xhi, double ylo, double yhi)
{
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

			buf[y * w + x] = -log(log(abs + 1 + 1e-2))/log(2) + i + 1;
		}
	}
}

/* static int colors[] = {0xFFFFFF, 0xa1b835, 0xe7571e, 0}; */
static int colors[] = {0, 0x888888, 0xFFFFFF};
static int col_len = sizeof colors / sizeof(int);

#define ABS(x) ((x) < 0 ? -(x) : (x))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

static void get_color(unsigned intensity, unsigned scale,
                      unsigned char *r, unsigned char *g, unsigned char *b)
{
	int pos, rate;
	int c1, c2;
	int r1, r2;
	int g1, g2;
	int b1, b2;
	int r_temp, g_temp, b_temp;

	pos = intensity * (col_len - 1) / scale;
	rate = intensity - pos * scale / (col_len - 1);

	c1 = colors[pos];
	c2 = colors[pos + 1];
	r1 = 0xFF & (c1 >> 16);
	r2 = 0xFF & (c2 >> 16);
	g1 = 0xFF & (c1 >> 8);
	g2 = 0xFF & (c2 >> 8);
	b1 = 0xFF & c1;
	b2 = 0xFF & c2;

	r_temp = r1 + rate * (r2 - r1) / scale;
	g_temp = g1 + rate * (g2 - g1) / scale;
	b_temp = b1 + rate * (b2 - b1) / scale;

	*r = MIN(MAX(r_temp, 0), 0xFF);
	*g = MIN(MAX(g_temp, 0), 0xFF);
	*b = MIN(MAX(b_temp, 0), 0xFF);
}


static PyObject *
mandelbrot(PyObject *self, PyObject *args)
{
	int x, y;
	int len, width, height, n_chans;
	unsigned intensity;
	unsigned char *buf, *position;
	double *fractal_buffer;
	double xlo, xhi, ylo, yhi;

	if (!PyArg_ParseTuple(args, "s#iidddd", &buf, &len, &width, &height,
                                            &xlo, &xhi, &ylo, &yhi))
		return NULL;
	n_chans = 3;
	
	fractal_buffer = (double*) malloc(height * width * sizeof(double));
	mandelbrot_iterations(fractal_buffer, width, height, xlo, xhi, ylo, yhi);
#define SCALE ITERATIONS

	for (y = 0; y < height; y++)
		for (x = 0; x < width; x++) {
			position = buf + ((height - 1 - y) * width + x) * n_chans;
			intensity = (log(fractal_buffer[y * width + x] + 1) 
                        / log(ITERATIONS + 2)) * SCALE;
			intensity = MIN(MAX(intensity, 0), SCALE);
			get_color(intensity, SCALE, position, position + 1, position + 2);
		}

	free(fractal_buffer);

	return Py_BuildValue("s#", buf, len);
}
