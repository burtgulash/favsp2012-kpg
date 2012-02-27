#include "filters.h"


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
