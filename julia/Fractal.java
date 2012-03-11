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


    int choose(double y, double y1, double y2) {
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

                return (R << 16) | (G << 8) | B;
            }
        }

        return colors[colors.length - 1];
    }

    int choose(double y, double y1, double y2, double eps) {
        double c = (xs[1] - x1) / (x2 - x1);
        double first = y1 + (xs[1] - x1) * (y2 - y1) / (x2 - x1);

        while (y < first && y2 - y1 > eps) {
            first = y1 + c * (first - y1);
            y2 = y1 + c * (y2 - y1);
        }

        return choose(y, y1, y2);
    }
}

class IterAbs {
    int i;
    double abs;

    IterAbs(int i, double abs) {
        this.i = i;
        this.abs = abs;
    }
}


public class Fractal implements Runnable {
    static int iterations = 5000;
    static int width, height;

    static ColorRange cr;

    final static double ESCAPE = 16.0;
    final static double LOG2 = Math.log(2.0);

    // Instance fields.
    int z; // zoom iteration.
    double angle;
    double c_x, c_y; // Center x, y.
    double size_x, size_y;
    double xlo, xhi, ylo, yhi;


    public Fractal(int z, double angle, double c_x, double c_y,
                   double size_x, double size_y,
                   double xlo, double xhi, double ylo, double yhi)
    {
        this.z = z;
        this.angle = angle;
        this.c_x = c_x;
        this.c_y = c_y;

        this.xlo = xlo;
        this.xhi = xhi;
        this.ylo = ylo;
        this.yhi = yhi;

        this.size_x = size_x;
        this.size_y = size_y;
    }


    private IterAbs iterate(double a, double b) {
        double tmp, abs;
        double re, im, re2, im2;
        int i;

        re = a;
        im = b;
        abs = 1.0;

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

        return new IterAbs(i, abs);
    }

    private void mandelbrot(double[][] buf, double angle,
                                   double c_x, double c_y,
                                   int sx, int ex, int sy, int ey) {
        final double sin = Math.sin(angle);
        final double cos = Math.cos(angle);
        boolean all_black = true;

        double a, b, tmp;
        int i;
        int x, y;
        int mx, my;    // middle_x, middle_y

        IterAbs ia;

        int w = buf[0].length;
        int h = buf.length;

        // Check border, if all pixels black.
        for (y = sy, x = sx; y < ey; y++) {
            a = xlo + ((xhi - xlo) * (double) x) / (w - 1);
            b = ylo + ((yhi - ylo) * (double) (h - 1 - y)) / (h - 1);
            tmp = c_x
                + cos * (a - c_x) - sin * (b - c_y);
            b = c_y + sin * (a - c_x) + cos * (b - c_y);
            a = tmp;

            ia = iterate(a, b);
            buf[y][x] = ia.i + 1
                      - Math.log(Math.log(ia.abs + Math.E)) / LOG2;

            if (ia.i < iterations)
                all_black = false;
        }
        for (y = sy, x = ex - 1; y < ey; y++) {
            a = xlo + ((xhi - xlo) * (double) x) / (w - 1);
            b = ylo + ((yhi - ylo) * (double) (h - 1 - y)) / (h - 1);
            tmp = c_x
                + cos * (a - c_x) - sin * (b - c_y);
            b = c_y + sin * (a - c_x) + cos * (b - c_y);
            a = tmp;

            ia = iterate(a, b);
            buf[y][x] = ia.i + 1
                      - Math.log(Math.log(ia.abs + Math.E)) / LOG2;

            if (ia.i < iterations)
                all_black = false;
        }
        for (x = sx, y = sy; x < ex; x++) {
            a = xlo + ((xhi - xlo) * (double) x) / (w - 1);
            b = ylo + ((yhi - ylo) * (double) (h - 1 - y)) / (h - 1);
            tmp = c_x
                + cos * (a - c_x) - sin * (b - c_y);
            b = c_y + sin * (a - c_x) + cos * (b - c_y);
            a = tmp;

            ia = iterate(a, b);
            buf[y][x] = ia.i + 1
                      - Math.log(Math.log(ia.abs + Math.E)) / LOG2;

            if (ia.i < iterations)
                all_black = false;
        }
        for (x = sx, y = ey - 1; x < ex; x++) {
            a = xlo + ((xhi - xlo) * (double) x) / (w - 1);
            b = ylo + ((yhi - ylo) * (double) (h - 1 - y)) / (h - 1);
            tmp = c_x
                + cos * (a - c_x) - sin * (b - c_y);
            b = c_y + sin * (a - c_x) + cos * (b - c_y);
            a = tmp;

            ia = iterate(a, b);
            buf[y][x] = ia.i + 1
                      - Math.log(Math.log(ia.abs + Math.E)) / LOG2;

            if (ia.i < iterations)
                all_black = false;
        }


        // Fill black, if all borders are black.
        if (all_black) {
            tmp = (double) (iterations + 1);
            for (y = sy; y < ey; y++)
                for (x = sx; x < ex; x++)
                    buf[y][x] = tmp;
            return;
        }


        // 20 = width of subdivision box.
        if (ex - sx > 20) {
            mx = (sx + ex) / 2;
            my = (sy + ey) / 2;

            mandelbrot(buf, angle, c_x, c_y, sx, mx, sy, my);
            mandelbrot(buf, angle, c_x, c_y, mx, ex, sy, my);
            mandelbrot(buf, angle, c_x, c_y, sx, mx, my, ey);
            mandelbrot(buf, angle, c_x, c_y, mx, ex, my, ey);
        } else {
            // Border pixels already filled.
            for (y = sy + 1; y < ey - 1; y++) {
                for (x = sx + 1; x < ex - 1; x++) {
                    a = xlo + ((xhi - xlo) * (double) x) / (w - 1);
                    b = ylo + ((yhi - ylo) * (double) (h - 1 - y)) / (h - 1);
                    tmp = c_x
                        + cos * (a - c_x) - sin * (b - c_y);
                    b = c_y + sin * (a - c_x) + cos * (b - c_y);
                    a = tmp;

                    ia = iterate(a, b);
                    buf[y][x] = ia.i + 1
                              - Math.log(Math.log(ia.abs + Math.E)) / LOG2;
                }
            }
        }
    }


    private void renderImage(Graphics2D g, double[][]buf, double eps) {
        int color, R, G, B, s2;
        double logColor;

        int w = buf[0].length;
        int h = buf.length;

        for (int y = 0; y < h; y ++) {
            for (int x = 0; x < w; x ++) {
                logColor = Math.log(buf[y][x] + 1.0)
                         / Math.log(iterations + 1.0);
                logColor = 1.0 - Math.min(Math.max(logColor, 0.0), 1.0);
                color = cr.choose(logColor, 0.0, 1.0, eps);

                g.setPaint(new Color(color));
                g.drawLine(x, y, x, y);
            }
        }
    }

    @Override
    public void run() {
        long time;
        double[][] buf = new double[height][width];

        time = System.currentTimeMillis();
        mandelbrot(buf, angle, c_x, c_y, 0, width, 0, height);

        BufferedImage img = new BufferedImage(width, height,
                                              BufferedImage.TYPE_INT_RGB);
        Graphics2D g = img.createGraphics();
        renderImage(g, buf, size_x * 2.0);
        time = System.currentTimeMillis() - time;

        String zoomName = "zoom" + z + ".png";
        try {
            ImageIO.write(img, "png", new File("zoom/" + zoomName));
            System.out.printf("%s finished. %dms%n", zoomName, time);
        } catch (IOException e) {
            System.out.printf("%s failed.%n", zoomName);
        }
    }


    public static void main (String[] argv) throws InterruptedException {
        width = 1280;
        height = 720;
        int samples = 4;

        double xlo, xhi, ylo, yhi;

        width *= samples;
        height *= samples;

        new File("zoom").mkdir();

        double c_x = -.167560053622672480;
        double c_y = 1.041333250530667298;


        cr = new ColorRange(new double[] {0, 40, 45, 50, 58, 65, 100},
             new int[]{0, 0, 0x5f104f, 0xffffff, 0xe819a6, 0x5f104f, 0});


        double ratio = (double) height / (double) width;
        double size_x = 4.2;
        double size_y = size_x * ratio;
        double angle = 0.0;

        Thread[] t;
        Fractal f;

        t = new Thread[4];
        for (int i = 0; i < t.length; i++)
            t[i] = new Thread();

        // zoom loop
        for (int z = 0; z < 420; z++) {
            xlo = c_x - size_x;
            xhi = c_x + size_x;
            ylo = c_y - size_y;
            yhi = c_y + size_y;
            size_x *= .92;
            size_y *= .92;

            f = new Fractal(z, angle, c_x, c_y, size_x, size_y,
                            xlo, xhi, ylo, yhi);
            t[z % t.length].join();
            t[z % t.length] = new Thread(f);
            t[z % t.length].start();
            angle += .5e-2;
        }

        for (int i = 0; i < t.length; i++)
            t[i].join();
    }
}
