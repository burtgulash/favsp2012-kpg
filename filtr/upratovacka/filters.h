#ifndef FILTERS_H
#define FILTERS_H

#include <glib.h>


/* Gray scale conversion. */
void gray_scale_luminance(guchar ps[], int h, int s, int n_chans);
void gray_scale_avg(guchar ps[], int h, int s, int n_chans);


/* Edge detection. */
int* sobel(guchar ps[], int h, int s, int n_chans);
int* prewitt(guchar ps[], int h, int s, int n_chans);
int* roberts_cross(guchar ps[], int h, int s, int n_chans);
int* laplace(guchar ps[], int h, int s, int n_chans);
int* edge_detect(int c_size, int cx[], int cy[],
                 guchar ps[], int h, int s, int n_chans);


/* Blur. */
void gaussian_kernel(double sigma, int size, int k[]);
void gaussian_blur(int size_x, int size_y, int kx[], int ky[],
                   guchar dst[], guchar src[], int h, int s, int n_chans);
#endif /* !FILTERS_H */
