import java.awt.image.BufferedImage;
import javax.imageio.ImageIO;

import java.io.IOException;
import java.io.File;

public class Sobel {
    static int arrayMin(int[] ar) {
        int min = Integer.MAX_VALUE;

        for (int i = 0; i < ar.length; i++)
            if (ar[i] < min)
                min = ar[i];

        return min;
    }

    static int arrayMax(int[] ar) {
        int max = Integer.MIN_VALUE;

        for (int i = 0; i < ar.length; i++)
            if (ar[i] > max)
                max = ar[i];

        return max;
    }

    static void sobel(int[] chan, int[] g, int w, int h) {
        int rw, gx, gy;

        for (int r = 1; r < h - 1; r ++) {
            rw = r * w;
            for (int c = 1; c < w - 1; c ++) {
                gx = 2 * chan[rw + c + 1] 
                   - 2 * chan[rw + c - 1]
                   +     chan[rw - w + c + 1]
                   +     chan[rw + w + c + 1]
                   -     chan[rw - w + c - 1]
                   -     chan[rw + w + c - 1];

                gy = 2 * chan[rw + w + c] 
                   - 2 * chan[rw - w + c]
                   +     chan[rw + w + c + 1]
                   +     chan[rw + w + c - 1]
                   -     chan[rw - w + c + 1]
                   -     chan[rw - w + c - 1];

                g[rw + c] = (int) (Math.sqrt(gx * gx + gy * gy));
                // g[rw + c] = Math.max(0, g[rw + c]);
            }
        }
    }


    public static void main(String[] argv) throws IOException {
        int w, h, min, max, rgbs[], channels[][][];
        BufferedImage in, out;

        in = ImageIO.read(new File(argv[0]));
        w = in.getWidth();
        h = in.getHeight();
        rgbs = new int[w * h];
        in.getRGB(0, 0, w, h, rgbs, 0, w);


        // Extract RGB channels.
        channels = new int[2][3][rgbs.length];
        for (int r = 0; r < h; r++) {
            int rw = r * w;
            for (int c = 0; c < w; c++) {
                channels[0][0][rw + c] = 0xFF & (rgbs[rw + c] >> 16);
                channels[0][1][rw + c] = 0xFF & (rgbs[rw + c] >> 8);
                channels[0][2][rw + c] = 0xFF & rgbs[rw + c];
            }
        }

        // Apply edge-detect to each channel.
        for (int i = 0; i < 3; i++)
            sobel(channels[0][i], channels[1][i], w, h);

        // Normalize colors.
        for (int i = 0; i < 3; i++) {
            min = arrayMin(channels[1][i]);
            max = arrayMax(channels[1][i]);

            for (int r = 0; r < h; r++) {
                int rw = r * w;
                for (int c = 0; c < w; c++) {
                    channels[1][i][rw + c] = 
                          0xFF * (channels[1][i][rw + c] - min) / (max - min);
                }
            }
        }


        // Blend channels.
        for (int r = 0; r < h; r++) {
            int rw = r * w;
            for (int c = 0; c < w; c++) {
                rgbs[rw + c] = (0xFF << 24) 
                             | (channels[1][0][rw + c] << 16) 
                             | (channels[1][1][rw + c] << 8)
                             | (channels[1][2][rw + c]);
            }
        }

        // Fill borders.
        for (int i = 0; i < w; i++) {
            rgbs[i] = 0xFF << 24;
            rgbs[(h - 1) * w + i] = 0xFF << 24;
        }
        for (int i = 0; i < h; i++) {
            rgbs[w * i] = 0xFF << 24;
            rgbs[w * i + w - 1] = 0xFF << 24;
        }


        out = new BufferedImage(w, h, BufferedImage.TYPE_INT_ARGB);
        out.setRGB(0, 0, w, h, rgbs, 0, w);

        ImageIO.write(out, "png", new File(argv[0] + "_sobel_color.png"));
    }
}
