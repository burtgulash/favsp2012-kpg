import java.awt.Graphics2D;
import java.awt.image.BufferedImage;
import java.awt.Color;

import java.io.IOException;
import java.io.File;

import javax.imageio.ImageIO;


public class Fractal {
    static double xlo, xhi, ylo, yhi;
    static int iterations = 300;

    final static int[][] CMAP = new int[][] 
         {{0, 15, 30, 60, 120, 255},
           {0, 0xFFFFFF, 0xb1e70c, 0x26e70c, 0x0db8c3, 0}};

    private static Color blendColor(int[][] colorMap, int iters) {
        int i = 0;

        for (; i < colorMap[0].length; i++) {
            if (colorMap[0][i] == iters)
                return new Color(colorMap[1][i]);
            if (colorMap[0][i] > iters) {
                int s = iters - colorMap[0][i - 1];
                int r = colorMap[0][i] - colorMap[0][i - 1];
                int color1 = colorMap[1][i - 1];
                int color2 = colorMap[1][i];

                int R1 = 0xFF & (color1 >> 16);
                int R2 = 0xFF & (color2 >> 16);
                int G1 = 0xFF & (color1 >> 8);
                int G2 = 0xFF & (color2 >> 8);
                int B1 = 0xFF & (color1);
                int B2 = 0xFF & (color2);

                int R = Math.min(R1, R2) 
                        + Math.abs(R1 - R2) * (R1 < R2 ? s : r - s) / r;
                int G = Math.min(G1, G2) 
                        + Math.abs(G1 - G2) * (G1 < G2 ? s : r - s) / r;
                int B = Math.min(B1, B2) 
                        + Math.abs(B1 - B2) * (B1 < B2 ? s : r - s) / r;

                return new Color(R, G, B);
            }
        }

        return new Color(colorMap[1][i]);
    }


    private static void mandelbrot(double[][] buf) {
        final double LOG_2 = Math.log(2);
        double a, b, re, im, tmp, abs;
        int i;

        int w = buf[0].length;
        int h = buf.length;
        double re_diff = xhi - xlo;
        double im_diff = yhi - ylo; 
        abs = 1.0;

        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                a = re = xlo + (re_diff * (double) x) / (w - 1);
                b = im = ylo + (im_diff * (double) (h - 1 - y)) / (h - 1);

                for (i = 0; i < iterations - 1; i++) {
                    abs = re * re + im * im;
                    if (abs >= 16.0)
                        break;
                    tmp = re * re - im * im + a;
                    im = 2 * re * im + b;
                    re = tmp;
                }

                buf[y][x] = i + 1 - Math.log(Math.log(abs + 1.0)) / LOG_2;
            }
        }
    }


    private static void renderImage(Graphics2D g, double[][]buf) {
        int w = buf[0].length;
        int h = buf.length;

        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                double logColor = Math.log(buf[y][x] + 1.0) * 255.0
                                / Math.log(iterations + 2);
                int c = Math.min(0xFF, Math.max(0, (int) logColor));
                g.setPaint(blendColor(CMAP, c));
                g.drawLine(x, y, x, y);
            }
        }
    }


    public static void main (String[] argv) throws IOException {
        xlo = -2.2;
        xhi = 1.0;
        ylo = -0.9;
        yhi = 0.9;

        int width = 1920;
        int height = (int) ((double) width * (yhi - ylo) / (xhi - xlo));

        double[][] buf = new double[height][width];
        mandelbrot(buf);

        BufferedImage img = new BufferedImage(width, height, 
                                              BufferedImage.TYPE_INT_RGB);
        Graphics2D g = img.createGraphics();

        renderImage(g, buf);
        ImageIO.write(img, "png", new File(argv[0]));
    }
}
