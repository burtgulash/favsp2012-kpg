#ifndef FILTERS_H
#define FILTERS_H


/* Gray scale conversion. */
void gray_scale_luminance(unsigned char ps[], int h, int s, int n_chans);
void gray_scale_avg(unsigned char ps[], int h, int s, int n_chans);


/* Edge detection. */
void sobel(unsigned char news[], unsigned char olds[],
           int h, int s, int n_chans);
void prewitt(unsigned char news[], unsigned char olds[],
             int h, int s, int n_chans);
void roberts_cross(unsigned char news[], unsigned char olds[],
                   int h, int s, int n_chans);
void laplace(unsigned char news[], unsigned char olds[],
             int h, int s, int n_chans);
void difference_of_gaussians(double sigma_first, double sigma_second,
                             unsigned char dst[], unsigned char src[],
                             int h, int s, int n_chans);


/* Gaussian blur. */
void gaussian_blur(double sigma_x, double sigma_y,
                   unsigned char dst[], unsigned char src[],
                   int h, int s, int n_chans);


/* Sharpen. */
void sharpen(double sigma, unsigned char* dst, unsigned char *src,
             int h, int s, int n_chans);


#endif /* !FILTERS_H */
