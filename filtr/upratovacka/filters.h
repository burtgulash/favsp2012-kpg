#ifndef FILTERS_H
#define FILTERS_H

#include <gdk-pixbuf/gdk-pixbuf.h>


int* sobel(GdkPixbuf *buf);
int* edge_detect(int c_size, int cx[], int cy[], GdkPixbuf *buf);

#endif /* !FILTERS_H */
