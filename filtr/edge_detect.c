#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "filters.h"


static void edge_detect(int c_size, int kernel[],
                        unsigned char dst[], unsigned char src[],
                        int h, int s, int n_chans);


void sobel(unsigned char dst[], unsigned char src[],
           int h, int s, int n_chans)
{
    int kernel[] = {-1, 0, 1,
                    -2, 0, 2,
                    -1, 0, 1};
    edge_detect(3, kernel, dst, src, h, s, n_chans);
}


void prewitt(unsigned char dst[], unsigned char src[],
             int h, int s, int n_chans)
{
    int kernel[] = {-1, 0, 1,
                    -1, 0, 1,
                    -1, 0, 1};

    edge_detect(3, kernel, dst, src, h, s, n_chans);
}


void roberts_cross(unsigned char dst[], unsigned char src[],
                   int h, int s, int n_chans)
{
    int kernel[] = {1, 0,
                    0, -1};

    edge_detect(2, kernel, dst, src, h, s, n_chans);
}


static void edge_detect(int c_size, int kernel[],
                        unsigned char dst[], unsigned char src[],
                        int h, int s, int n_chans)
{
    int c_half;
    int channel, x, y;
    int xx, yy;
    int offset, p_off;
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
                        p_off = (yy - c_half) * s + (xx - c_half) * n_chans;

                        sumx += kernel[yy * c_size + xx] * p[p_off];
                        sumy += kernel[xx * c_size + yy] * p[p_off];
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
        for (y = 1; y < h - 1; y++) {
            for (x = n_chans; x < s - n_chans; x += n_chans) {
                offset = y * s + x + channel;

                dst[offset] = (unsigned char)
                              (0xFF * (tmp[offset] - min) / (max - min));
            }
        }
    }

    free(tmp);
}


void laplace(int dst[], unsigned char src[],
             int h, int s, int n_chans)
{
    int x, y, channel, offset;
    unsigned char *p;

    for (channel = 0; channel < MIN(3, n_chans); channel++) {
        for (y = 1; y < h - 1; y++) {
            for (x = n_chans; x < s - n_chans; x += n_chans) {
                offset = y * s + x + channel;
                p = src + offset;

                dst[offset] = (
                             - p[n_chans]
                             - p[-n_chans]
                             - p[s]
                             - p[-s]
                             + 4 * (*p)) 
                             / 4;
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
