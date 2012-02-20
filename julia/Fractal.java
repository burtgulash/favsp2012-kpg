import java.awt.Graphics2D;
import java.awt.image.BufferedImage;
import java.awt.Color;

import java.io.IOException;
import java.io.File;

import javax.imageio.ImageIO;

class ColorRange {
    double[] xs;
    double x1, x2;
    int[] colors;

    ColorRange(double[] xs, int[] colors) {
        this.xs = xs;
        this.colors = colors;
        assert xs.length == colors.length;

        x1 = xs[0];
        x2 = xs[xs.length - 1];
    }


    java.awt.Color choose(double y, double y1, double y2) {
        double x = x1 + (x2 - x1) * (y - y1) / (y2 - y1);

        for (int i = 1; i < xs.length; i++) {
            if (xs[i] >= x) {
                double rate = (x - xs[i - 1]) / (xs[i] - xs[i - 1]);
                int c1 = colors[i - 1];
                int c2 = colors[i];

                int R1 = 0xFF & (c1 >> 16);
                int R2 = 0xFF & (c2 >> 16);
                int G1 = 0xFF & (c1 >> 8);
                int G2 = 0xFF & (c2 >> 8);
                int B1 = 0xFF & c1;
                int B2 = 0xFF & c2;

                int R = Math.min(R1, R2)
                     + (int) (Math.abs(R1 - R2) * (R1 < R2 ? rate : 1 - rate));
                int G = Math.min(G1, G2)                                
                     + (int) (Math.abs(G1 - G2) * (G1 < G2 ? rate : 1 - rate));
                int B = Math.min(B1, B2)                                
                     + (int) (Math.abs(B1 - B2) * (B1 < B2 ? rate : 1 - rate));

                return new Color(R, G, B);
            }
        }

        return new Color(colors[colors.length - 1]);
    }

    java.awt.Color choose(double y, double y1, double y2, double eps) {
        double c = (xs[1] - x1) / (x2 - x1);
        double first = y1 + (xs[1] - x1) * (y2 - y1) / (x2 - x1);

        while (y < first && y2 - y1 > eps) {
            first = y1 + c * (first - y1);
            y2 = y1 + c * (y2 - y1);
        }

        return choose(y, y1, y2);
    }
}


public class Fractal {
    static double xlo, xhi, ylo, yhi;
    static int iterations = 5000;

    static ColorRange cr;


    private static void mandelbrot(double[][] buf, double angle, 
                                   double center_x, double center_y) {
        final double ESCAPE = 16.0;
        final double LOG2 = Math.log(2.0);
        final double sin = Math.sin(angle);
        final double cos = Math.cos(angle);

        double a, b, re, im, tmp, abs;
        double re2, im2;
        int i;

        int w = buf[0].length;
        int h = buf.length;
        double re_diff = xhi - xlo;
        double im_diff = yhi - ylo; 
        abs = 1.0;

        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                a = xlo + (re_diff * (double) x) / (w - 1);
                b = ylo + (im_diff * (double) (h - 1 - y)) / (h - 1);
                tmp = center_x + cos * (a - center_x) - sin * (b - center_y);
                b = center_y + sin * (a - center_x) + cos * (b - center_y);
                a = tmp;

                re = a;
                im = b;

                for (i = 0; i < iterations; i++) {
                    re2 = re * re;
                    im2 = im * im;
                    abs = re2 + im2;
                    if (abs >= ESCAPE)
                        break;
                    tmp = re2 - im2 + a;
                    im = 2 * re * im + b;
                    re = tmp;
                }

                buf[y][x] = i + 1 - Math.log(Math.log(abs + Math.E)) / LOG2;
            }
        }
    }


    private static void renderImage(Graphics2D g, double[][]buf, double eps) {
        int w = buf[0].length;
        int h = buf.length;

        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                double logColor = Math.log(buf[y][x] + 1.0)
                                / Math.log(iterations + 1.0);
                logColor = 1.0 - Math.min(1.0, Math.max(0.0, logColor));

                g.setPaint(cr.choose(logColor, 0.0, 1.0, eps));
                g.drawLine(x, y, x, y);
            }
        }
    }


    public static void main (String[] argv) throws IOException {
        int width = 800;
        int height = 600;

        new File("zoom").mkdir();

        // double center_x = 0.013438871094;
        // double center_y = 0.655614211997;
        double center_x = -.167560053621562480;
        double center_y = 1.041333250530746298;

        double[][] buf = new double[height][width];

        cr = new ColorRange(new double[] {0, 50, 60, 70, 85, 100},
             new int[]{0, 0xffffff, 0xe3b6ff, 0xaa1188, 0xffca22, 0xffffff});
        cr = new ColorRange(new double[] {0, 40, 45, 50, 60, 65, 100},
             new int[]{0, 0, 0x5f104f, 0xffffff, 0xe819a6, 0x5f104f, 0});
        cr = new ColorRange(new double[] {0, 20, 45, 50, 55, 65, 100},
             new int[]{0, 0, 0x5f104f, 0xffffff, 0xe819a6, 0x5f104f, 0});


        double ratio = (double) height / (double) width;
        double size_x = 2.0;
        double size_y = size_x * ratio;
        double angle = 0.0;

        // zoom loop
        for (int z = 0; z < 100; z++) {
            xlo = center_x - size_x;
            xhi = center_x + size_x;
            ylo = center_y - size_y;
            yhi = center_y + size_y;
            size_x *= .9;
            size_y *= .9;

            mandelbrot(buf, angle, center_x, center_y);

            BufferedImage img = new BufferedImage(width, height, 
                                                  BufferedImage.TYPE_INT_RGB);
            Graphics2D g = img.createGraphics();
            renderImage(g, buf, size_x * 2.0);

            String zoomName = "zoom" + z + ".png";
            ImageIO.write(img, "png", new File("zoom/" + zoomName));
            System.out.println(zoomName + " finished.");
            angle += 1e-2;
        }
    }
}
