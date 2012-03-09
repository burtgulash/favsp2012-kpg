#include <stdlib.h>

#include "filters.h"


void sharpen(double coeff, unsigned char* dst, unsigned char *src,
             int h, int s, int n_chans)
{
    int x, y, channel, offset;
    int* tmp;

    tmp = (int*) malloc(h * s * sizeof(int));
    coeff = MAX(0.0, coeff);

    laplace(tmp, src, h, s, n_chans);

    for (channel = 0; channel < MIN(3, n_chans); channel++) {
        /* Copy border pixels from src. */
        for (x = 0; x < s; x += n_chans) {
            dst[x] = src[x];
            dst[(h - 1) * s + x] = src[(h - 1) * s + x];
        }
        for (y = 0; y < h; y++) {
            dst[y * s] = src[y * s];
            dst[y * s + s - 1] = src[y * s + s - 1];
        }

        for (y = 1; y < h - 1; y++) {
            for (x = n_chans; x < s - n_chans; x += n_chans) {
                offset = y * s + x + channel;

                dst[offset] = MIN(MAX(
                                  src[offset] + coeff * tmp[offset], 0), 0xFF);
            }
        }
    }

    free(tmp);
}
