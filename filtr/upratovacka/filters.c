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

int* prewitt(GdkPixbuf *buf)
{
    int prewitt_x[] = {-1, 0, 1, 
                     -1, 0, 1, 
                     -1, 0, 1};
    int prewitt_y[] = {-1,-1,-1, 
                      0, 0, 0, 
                      1, 1, 1};

    return edge_detect(3, prewitt_x, prewitt_y, buf);
}

int* roberts_cross(GdkPixbuf *buf)
{
    int rc_x[] = {1, 0,
                  0, -1};
    int rc_y[] = {0, 1,
                 -1, 0};

    return edge_detect(2, rc_x, rc_y, buf);
}



int* edge_detect(int c_size, int cx[], int cy[], GdkPixbuf *buf)
{
    int c_half;
    int channel, x, y;
    int xx, yy;
    int h, s, n_chans;
    int offset, c_off, p_off;
    int *res;
    int sumx, sumy;
    guchar *ps, *p;

    h = gdk_pixbuf_get_height(buf);
    s = gdk_pixbuf_get_rowstride(buf);
    n_chans = gdk_pixbuf_get_n_channels(buf);

    ps = gdk_pixbuf_get_pixels(buf);
    res = (int*) malloc(sizeof(int) * s * h);

    c_half = c_size / 2;


    for (channel = 0; channel < 3; channel++) {
        for (y = c_half; y < h - c_half; y++) {
            for (x = c_half; x < s - c_half * n_chans; x += n_chans) {
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
