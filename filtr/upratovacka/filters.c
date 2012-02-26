#include <glib.h>

#include <math.h> /* sqrt */
#include <stdlib.h>

#include "filters.h"


void gray_scale_luminance(guchar ps[], int h, int s, int n_chans)
{
    int x, y, channel, offset;
    double sum;

    for (y = 0; y < h; y++) {
        for (x = 0; x < s; x += n_chans) {
            offset = y * s + x;

            if (n_chans >= 3) {
                sum = .2125 * ps[offset]
                    + .7152 * ps[offset + 1]
                    + .0722 * ps[offset + 2];

                for (channel = 0; channel < 3; channel++)
                    ps[offset + channel] = (guchar) sum;
            }
        }
    }
}


void gray_scale_avg(guchar ps[], int h, int s, int n_chans)
{
    int x, y, channel, offset;
    int sum;

    for (y = 0; y < h; y++) {
        for (x = 0; x < s; x += n_chans) {
            offset = y * s + x;

            if (n_chans >= 3) {
                sum = (ps[offset] + ps[offset + 1] + ps[offset + 2]) / 3;

                for (channel = 0; channel < MIN(3, n_chans); channel++)
                    ps[offset + channel] = (guchar) sum;
            }
        }
    }
}


int* sobel(guchar ps[], int h, int s, int n_chans)
{
    int sobel_x[] = {-1, 0, 1,
                     -2, 0, 2,
                     -1, 0, 1};
    int sobel_y[] = {-1,-2,-1,
                      0, 0, 0,
                      1, 2, 1};

    return edge_detect(3, sobel_x, sobel_y, ps, h, s, n_chans);
}

int* prewitt(guchar ps[], int h, int s, int n_chans)
{
    int prewitt_x[] = {-1, 0, 1,
                     -1, 0, 1,
                     -1, 0, 1};
    int prewitt_y[] = {-1,-1,-1,
                      0, 0, 0,
                      1, 1, 1};

    return edge_detect(3, prewitt_x, prewitt_y, ps, h, s, n_chans);
}

int* roberts_cross(guchar ps[], int h, int s, int n_chans)
{
    int rc_x[] = {1, 0,
                  0, -1};
    int rc_y[] = {0, 1,
                 -1, 0};

    return edge_detect(2, rc_x, rc_y, ps, h, s, n_chans);
}


int* laplace(guchar ps[], int h, int s, int n_chans)
{
    int laplace[] = {-1, -1, -1,
                     -1, +8, -1,
                     -1, -1, -1};

    return edge_detect(3, laplace, laplace, ps, h, s, n_chans);
}


int* edge_detect(int c_size, int cx[], int cy[],
                 guchar ps[], int h, int s, int n_chans)
{
    int c_half;
    int channel, x, y;
    int xx, yy;
    int offset, c_off, p_off;
    int *res;
    int sumx, sumy;
    guchar *p;

    res = (int*) g_malloc(sizeof(int) * s * h);
    c_half = c_size / 2;


    for (channel = 0; channel < MIN(3, n_chans); channel++) {
        for (y = c_half; y < h - c_half; y++) {
            for (x = c_half * n_chans; x < s - c_half * n_chans; x += n_chans) {
                offset = y * s + x + channel;
                p = ps + offset;

                sumx = sumy = 0;
                /* Convolve image with convolution matrix c. */
                for (yy = 0; yy < c_size; yy++) {
                    for (xx = 0; xx < c_size; xx++) {
                        c_off = yy * c_size + xx;
                        p_off = (yy - c_half) * s + (xx - c_half) * n_chans;

                        sumx += cx[c_off] * p[p_off];
                        sumy += cy[c_off] * p[p_off];
                    }
                }

                res[offset] = (int) sqrt((double) (sumx * sumx + sumy * sumy));
            }
        }
    }

    return res;
}

void gaussian_kernel(double sigma, int size, int k[])
{
	int i, half;

	/* Assure that size is odd. */
	size += size & 1;
	size -= 1;

	half = size / 2;
	for (i = 0; i < size; i++)
		k[i] = 0;

#define SQRT_2_PI 2.5066282746
	for (i = 0; i < half; i++) {
		double gauss = exp(-.5 * (i * i) / (sigma * sigma)) 
                     / (sigma * SQRT_2_PI);
		k[half + i] = k[half - i] = (int) (gauss * 255.0);
	}
}


void gaussian_blur(int size_x, int size_y, int kx[], int ky[],
                   guchar dst[], guchar src[], int h, int s, int n_chans)
{
    int channel, x, y, i;
    int sum, offset;
	int kx_sum, ky_sum;
	int half_x, half_y;
    guchar *p;

	half_x = size_x / 2;
	half_y = size_y / 2;

	kx_sum = ky_sum = 0;
	for (i = 0; i < size_x; i++)
		kx_sum += kx[i];
	for (i = 0; i < size_y; i++)
		ky_sum += ky[i];

	/* TODO extrapolation. */

	/* Vertical pass. */
    for (channel = 0; channel < MIN(3, n_chans); channel++) {
        for (y = half_y; y < h - half_y; y++) {
            for (x = 0; x < s; x += n_chans) {
                offset = y * s + x + channel;
                p = src + offset;

                sum = 0;
				for (i = 0; i < size_y; i++)
					sum += ky[i] * p[(i - half_y) * s];

                dst[offset] = (guchar) (sum / ky_sum);
            }
        }
    }

	/* Horizontal pass. */
    for (channel = 0; channel < MIN(3, n_chans); channel++) {
        for (y = 0; y < h; y++) {
            for (x = half_x * n_chans; x < s - half_x * n_chans; x += n_chans) {
                offset = y * s + x + channel;
                p = src + offset;

                sum = 0;
				for (i = 0; i < size_x; i++)
					sum += kx[i] * p[(i - half_x) * n_chans];

                dst[offset] = (guchar) (sum / kx_sum);
            }
        }
    }
}
