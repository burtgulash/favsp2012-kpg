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
int* edge_detect(int c_size, int cx[], int cy[], 
                 guchar ps[], int h, int s, int n_chans);

#endif /* !FILTERS_H */