#ifndef FILTERS_H
#define FILTERS_H

#include <glib.h>


/* Gray scale conversion. */
void gray_scale_luminance(guchar ps[], int h, int s, int n_chans);
void gray_scale_avg(guchar ps[], int h, int s, int n_chans);


/* Edge detection. */
void sobel(guchar news[], guchar olds[], int h, int s, int n_chans);
void prewitt(guchar news[], guchar olds[], int h, int s, int n_chans);
void roberts_cross(guchar news[], guchar olds[], int h, int s, int n_chans);
void laplace(guchar news[], guchar olds[], int h, int s, int n_chans);
void edge_detect(int c_size, int cx[], int cy[],
                 guchar dst[], guchar src[], int h, int s, int n_chans);


/* Blur. */
void gaussian_kernel(double sigma, int size, unsigned k[]);
void gaussian_blur(int size_x, int size_y, unsigned kx[], unsigned ky[],
                   guchar dst[], guchar src[], int h, int s, int n_chans);
#endif /* !FILTERS_H */
