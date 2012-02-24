#include <gdk-pixbuf/gdk-pixbuf.h>
#include <glib.h>

#include <math.h> /* sqrt */
#include <stdlib.h>

#include "filters.h"


int* sobel(GdkPixbuf *buf)
{
    int sobel_x[] = {-1, 0, 1, 
                     -2, 0, 2, 
                     -1, 0, 1};
    int sobel_y[] = {-1,-2,-1, 
                      0, 0, 0, 
                      1, 2, 1};

    return edge_detect(3, sobel_x, sobel_y, buf);
}


int* edge_detect(int c_size, int cx[], int cy[], GdkPixbuf *buf)
{
    int c_center, c_out, c_sq, c_off_start;
    int channel, x, y;
    int w, h, s;
    int n_chans;
    int offset, c_off;
    int *res;
    int sumx, sumy;
    guchar *ps, *p;

    w = gdk_pixbuf_get_width(buf);
    h = gdk_pixbuf_get_height(buf);
    s = gdk_pixbuf_get_rowstride(buf);
    n_chans = gdk_pixbuf_get_n_channels(buf);

    ps = gdk_pixbuf_get_pixels(buf);
    res = (int*) malloc(sizeof(int) * w * h * n_chans);

    c_center = c_size * c_size / 2;
    c_out = c_size / 2;
    c_sq = c_size * c_size * n_chans;
    c_off_start = -c_center * n_chans;


    for (channel = 0; channel < 3; channel++) {
        for (y = c_out; y < h - c_out; y++) {
            for (x = c_out; x < s - c_out * n_chans; x++) {
                offset = y * s + x + channel;
                p = ps + offset;

                sumx = sumy = 0;
                /* Perform convolution of image with convolution matrix c. */
                for (c_off = c_off_start; c_off < c_sq; c_off += n_chans) {
                    sumx += cx[c_off] * p[c_off];
                    sumy += cy[c_off] * p[c_off];
                }

                res[offset + c_off] = (int) 
                                 sqrt((double) (sumx * sumx + sumy * sumy));
            }
        }
    }

    return res;
}
