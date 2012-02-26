#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "filters.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))


static void edge_detect(int c_size, int cx[], int cy[],
                        unsigned char dst[], unsigned char src[],
                        int h, int s, int n_chans);

static void gaussian_create_kernel(double sigma, int size, unsigned k[]);
static void gaussian_apply_kernel(int size_x, int size_y,
                   unsigned kx[], unsigned ky[],
                   unsigned char dst[], unsigned char src[],
                   int h, int s, int n_chans);


/*
 * Gray scale conversion functions.
 */

void gray_scale_luminance(unsigned char ps[], int h, int s, int n_chans)
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
                    ps[offset + channel] = (unsigned char) sum;
            }
        }
    }
}


void gray_scale_avg(unsigned char ps[], int h, int s, int n_chans)
{
    int x, y, channel, offset;
    int sum;

    for (y = 0; y < h; y++) {
        for (x = 0; x < s; x += n_chans) {
            offset = y * s + x;

            if (n_chans >= 3) {
                sum = (ps[offset] + ps[offset + 1] + ps[offset + 2]) / 3;

                for (channel = 0; channel < MIN(3, n_chans); channel++)
                    ps[offset + channel] = (unsigned char) sum;
            }
        }
    }
}



/*
 * Edge detection functions.
 */

void sobel(unsigned char dst[], unsigned char src[],
           int h, int s, int n_chans)
{
    int sobel_x[] = {-1, 0, 1,
                     -2, 0, 2,
                     -1, 0, 1};
    int sobel_y[] = {-1,-2,-1,
                      0, 0, 0,
                      1, 2, 1};
    edge_detect(3, sobel_x, sobel_y, dst, src, h, s, n_chans);
}


void prewitt(unsigned char dst[], unsigned char src[],
             int h, int s, int n_chans)
{
    int prewitt_x[] = {-1, 0, 1,
                     -1, 0, 1,
                     -1, 0, 1};
    int prewitt_y[] = {-1,-1,-1,
                      0, 0, 0,
                      1, 1, 1};

    edge_detect(3, prewitt_x, prewitt_y, dst, src, h, s, n_chans);
}


void roberts_cross(unsigned char dst[], unsigned char src[],
                   int h, int s, int n_chans)
{
    int rc_x[] = {1, 0,
                  0, -1};
    int rc_y[] = {0, 1,
                 -1, 0};

    edge_detect(2, rc_x, rc_y, dst, src, h, s, n_chans);
}


static void edge_detect(int c_size, int cx[], int cy[],
                        unsigned char dst[], unsigned char src[],
                        int h, int s, int n_chans)
{
    int c_half;
    int channel, x, y;
    int xx, yy;
    int offset, c_off, p_off;
    int sumx, sumy;
    int min, max;
    int *tmp, tmp_value;
    unsigned char *p;

    tmp = (int*) calloc(h * s, sizeof(int));
    c_half = c_size / 2;


    for (channel = 0; channel < MIN(3, n_chans); channel++) {
        min = 0x0;
        max = 0x1;

        for (y = c_half; y < h - c_half; y++) {
            for (x = c_half * n_chans; x < s - c_half * n_chans; x += n_chans) {
                offset = y * s + x + channel;
                p = src + offset;

                sumx = sumy = 0;
                for (yy = 0; yy < c_size; yy++) {
                    for (xx = 0; xx < c_size; xx++) {
                        c_off = yy * c_size + xx;
                        p_off = (yy - c_half) * s + (xx - c_half) * n_chans;

                        sumx += cx[c_off] * p[p_off];
                        sumy += cy[c_off] * p[p_off];
                    }
                }

                tmp_value = (int) sqrt((double) (sumx * sumx + sumy * sumy));
                tmp[offset] = tmp_value;

                if (tmp_value > max)
                    max = tmp_value;
                if (tmp_value < min)
                    min = tmp_value;
            }
        }

        /* Histogram normalization. */
        for (y = 0; y < h; y++) {
            for (x = 0; x < s; x += n_chans) {
                offset = y * s + x + channel;

                dst[offset] = (unsigned char)
                              (0xFF * (tmp[offset] - min) / (max - min));
            }
        }
    }

    free(tmp);
}


/* TODO MEDIAN FILTER */
void laplace(unsigned char dst[], unsigned char src[],
             int h, int s, int n_chans)
{
    int x, y, channel, offset;
    int xx, yy;
    int c_off, p_off;
    int sum;
    unsigned char *p;
    int laplace[] = {-1, -1, -1,
                     -1, +8, -1,
                     -1, -1, -1};


    for (channel = 0; channel < MIN(3, n_chans); channel++) {
        for (y = 1; y < h - 1; y++) {
            for (x = n_chans; x < s - n_chans; x += n_chans) {
                offset = y * s + x + channel;
                p = src + offset;

                sum = 0;
                for (yy = 0; yy < 3; yy++) {
                    for (xx = 0; xx < 3; xx++) {
                        c_off = yy * 3 + xx;
                        p_off = (yy - 1) * s + (xx - 1) * n_chans;

                        sum += laplace[c_off] * p[p_off];
                    }
                }

                sum = MAX(0, sum);
                dst[offset] = sum / 8;
            }
        }
    }
}


void difference_of_gaussians(double sigma_first, double sigma_second,
                             unsigned char dst[], unsigned char src[],
                             int h, int s, int n_chans)
{
    int x, y, channel, offset, size;
    unsigned char *first, *second;


    size = h * s;
    first = (unsigned char*) malloc(size * sizeof(unsigned char*));
    second = (unsigned char*) malloc(size * sizeof(unsigned char*));
    memcpy(first, src, size);
    memcpy(second, src, size);

    gaussian_blur(sigma_first, sigma_first, first, first, h, s, n_chans);
    gaussian_blur(sigma_second, sigma_second, second, second, h, s, n_chans);

    for (channel = 0; channel < MIN(3, n_chans); channel++) {
        for (y = 0; y < h; y++) {
            for (x = 0; x < s; x += n_chans) {
                offset = y * s + x + channel;

                dst[offset] = MIN(MAX(first[offset] - second[offset], 0), 0xFF);
            }
        }
    }

    free(first);
    free(second);
}



/*
 * Gaussian blur functions.
 */

static void gaussian_create_kernel(double sigma, int size, unsigned k[])
{
    int i, half;

    half = size / 2;

#define SQRT_2_PI 2.5066282746
    for (i = 0; i < MAX(half, 1); i++) {
        double gauss = exp(-.5 * (double) (i * i) / (sigma * sigma))
                     / (sigma * SQRT_2_PI);
        k[half + i] = k[half - i] = (unsigned) (gauss * 0xFFFF);
    }
}


static void gaussian_apply_kernel(int size_x, int size_y,
                   unsigned kx[], unsigned ky[],
                   unsigned char dst[], unsigned char src[],
                   int h, int s, int n_chans)
{
    int x, y, ys, i;
    int channel, chans;
    int offset;
    int half_x, half_y;
    unsigned sum, kx_sum, ky_sum;
    unsigned char *tmp;

    tmp = (unsigned char*) malloc(h * s * sizeof(unsigned char));
    chans = MIN(3, n_chans);

    half_x = size_x / 2;
    half_y = size_y / 2;

    kx_sum = ky_sum = 0;
    for (i = 0; i < size_x; i++)
        kx_sum += kx[i];
    for (i = 0; i < size_y; i++)
        ky_sum += ky[i];

    /* Horizontal pass. */
    for (channel = 0; channel < chans; channel++) {
        for (y = 0; y < h; y++) {
            ys = y * s;
            for (x = 0; x < s; x += n_chans) {
                offset = ys + x + channel;

                sum = 0;
                for (i = 0; i < size_x; i++)
                    sum += kx[i] * src[ys + channel
                                          + MAX(0,
                                            MIN(s - n_chans,
                                                x + (i - half_x) * n_chans))];

                tmp[offset] = (unsigned char) (sum / kx_sum);
            }
        }
    }

    /* Vertical pass. */
    for (channel = 0; channel < chans; channel++) {
        for (y = 0; y < h; y++) {
            ys = y * s;
            for (x = 0; x < s; x += n_chans) {
                offset = ys + x + channel;

                sum = 0;
                for (i = 0; i < size_y; i++)
                    sum += ky[i] * tmp[x + channel
                                         + s * MAX(0,
                                               MIN(h - 1,
                                                   y + i - half_y))];

                dst[offset] = (unsigned char) (sum / ky_sum);
            }
        }
    }

    free(tmp);
}


void gaussian_blur(double sigma_x, double sigma_y,
                   unsigned char dst[], unsigned char src[],
                   int h, int s, int n_chans)
{
    unsigned size_x, size_y;
    unsigned *kernel_x, *kernel_y;

    /* Compute size of kernel matrix according to sigmas. */
    size_x = 8 * (unsigned) sigma_x + 8;
    size_y = 8 * (unsigned) sigma_y + 8;

    /* Make size odd number. */
    size_x += size_x & 1;
    size_x -= 1;
    size_y += size_y & 1;
    size_y -= 1;


    /* Ensure that sigma is reasonably large. */
    sigma_x = MIN(MAX(sigma_x, .02), 10.0);
    sigma_y = MIN(MAX(sigma_y, .02), 10.0);

    kernel_x = calloc(size_x, sizeof(unsigned));
    kernel_y = calloc(size_y, sizeof(unsigned));

    gaussian_create_kernel(sigma_x, size_x, kernel_x);
    gaussian_create_kernel(sigma_y, size_y, kernel_y);

    gaussian_apply_kernel(size_x, size_y, kernel_x, kernel_y,
                          dst, src, h, s, n_chans);

    free(kernel_x);
    free(kernel_y);
}


/*
 * Sharpen.
 */
void sharpen(double sigma, unsigned char* dst, unsigned char *src,
             int h, int s, int n_chans)
{
    int x, y, channel, offset;
    unsigned char* tmp;

    tmp = (unsigned char*) malloc(h * s * sizeof(unsigned char));

    /* difference_of_gaussians(sigma, 0, dst, src, h, s, n_chans); */
    sobel(tmp, src, h, s, n_chans);
    laplace(dst, tmp, h, s, n_chans);

    for (channel = 0; channel < MIN(3, n_chans); channel++) {
        for (y = 0; y < h; y++) {
            for (x = 0; x < s; x += n_chans) {
                offset = y * s + x + channel;

                dst[offset] = MAX(0, src[offset] - dst[offset]);
            }
        }
    }

    free(tmp);
}
