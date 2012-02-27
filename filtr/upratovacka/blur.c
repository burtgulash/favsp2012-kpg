#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "filters.h"


static void gaussian_create_kernel(double sigma, int size, unsigned k[]);
static void gaussian_apply_kernel(int size_x, int size_y,
                   unsigned kx[], unsigned ky[],
                   unsigned char dst[], unsigned char src[],
                   int h, int s, int n_chans);


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
