import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Dimension;
import java.awt.Color;

import javax.swing.JPanel;
import javax.swing.JFrame;


public class Main extends JPanel {
    Graphics2D g;
    int w, h, lvls;

    public Main(int width, int height, int lvls) {
        w = width;
        h = height;
        this.lvls = lvls;
    }

    void draw_line(int a, int b, int c, int d, int x, int y, int dx, int dy)
    {
        g.drawLine(x, h - 1 - (y),
                   x + a * dx + b * dy,
                   h - 1 - (y + c * dx + d * dy));
    }

    void triomino(int count, int size, int a, int b, int c, int d, int x, int y)
    {
        if (count < lvls) {
            int half = size / 2;
            count ++;
            triomino(count, half, a, b, c, d, x, y);
            triomino(count, half, a, b, c, d, x + a * half + b * half,
                                       y + c * half + d * half);
            triomino(count, half, c, d, -a, -b, x + a * 0 + b * 2 * size,
                                         y + c * 0 + d * 2 * size);
            triomino(count, half, -c, -d, a, b, x + a * 2 * size + b * 0,
                                         y + c * 2 * size + d * 0);
        } else {
            draw_line(a, b, c, d, x, y, 2 * size, 0);
            draw_line(a, b, c, d, x, y, 0, 2 * size);
            draw_line(a, b, c, d, x + a * size + b * size,
                                  y + c * size + d * size,
                                  0, size);
            draw_line(a, b, c, d, x + a * size + b * size,
                                  y + c * size + d * size,
                                  size, 0);
            draw_line(a, b, c, d, x + a * 2 * size + b * 0,
                                  y + c * 2 * size + d * 0,
                                  0, size);
            draw_line(a, b, c, d, x + a * 0 + b * 2 * size,
                                  y + c * 0 + d * 2 * size,
                                  size, 0);
        }
    }

    public void paintComponent(Graphics gg) {
        super.paintComponent(gg);
        g = (Graphics2D) gg;

        int size = w;
        int x = 0;
        int y = 0;

        // Get nearest power of 2.
        while ((size & (size - 1)) != 0)
            size &= size - 1;

        draw_line(1, 0, 0, 1, x, y, size, 0);
        draw_line(1, 0, 0, 1, x, y, 0, size);
        draw_line(1, 0, 0, 1, x + size, y, 0, size);
        draw_line(1, 0, 0, 1, x, y + size, size, 0);

        for (int c = 0; c < lvls; c++) {
            triomino(c, size, 1, 0, 0, 1, x, y);


            x += size;
            y += size;
            size /= 2;
        }
    }


    public static void main(String[] argv) {
        int lvls, width, height;
        lvls = 4;
        width = height = 512;
        try {
            lvls = Integer.parseInt(argv[0]);
            width = Integer.parseInt(argv[1]);
        } catch (NumberFormatException e) {
            System.err.println("Usage: triomino SUBDIVISIONS SIZE");
            System.exit(1);
        }
        height = width;

        JFrame frame = new JFrame("Triomino fractal");
        Main m = new Main(width, height, lvls);
        m.setBackground(Color.WHITE);

        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        m.setPreferredSize(new Dimension(width, height));

        frame.add(m);
        frame.pack();
        frame.setVisible(true);
    }
}
