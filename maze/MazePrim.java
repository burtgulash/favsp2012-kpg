import java.util.PriorityQueue;
import java.util.Random;

import java.awt.Graphics2D;
import java.awt.Graphics;
import java.awt.Color;
import java.awt.Dimension;

import javax.swing.JPanel;
import javax.swing.JFrame;


class Movement implements Comparable<Movement> {
    int r1, c1, r2, c2, dirn, w;

    Movement(int r1, int c1, int r2, int c2, int dirn, int w) {
        this.r1 = r1;
        this.c1 = c1;
        this.r2 = r2;
        this.c2 = c2;
        this.dirn = dirn;
        this.w = w;
    }

    public int compareTo(Movement m) {
        if (w > m.w)
            return 1;
        if (w == m.w)
            return 0;
        return -1;
    }
}


public class Maze extends JPanel {
    static int rows = 0, cols = 0;
    static int width, height;
    static int boxSize, innerBoxSize, wallThickness;

    static boolean[][] visited;
    static boolean[][][] wall;

    static final Random gorila = new Random();

    static void generate(int rows, int cols) {
        final int MAXW = 10000;
        int w;
        Movement c;

        visited = new boolean[rows][cols];
        wall    = new boolean[rows][cols][2];

        PriorityQueue<Movement> q = new PriorityQueue<Movement>();

        // start from (0, 0), go to (1, 0), that is direction '3', weight = 0
        q.add(new Movement(0, 0, 1, 0, 3, 0));
        visited[0][0] = true;

        main:
        while (true) {
            do {
                c = q.poll();
                if (c == null)
                    break main;
            } while (visited[c.r2][c.c2]);

            visited[c.r2][c.c2] = true;
            switch (c.dirn) {
                case 0:
                    wall[c.r1][c.c1][0] = true; break;
                case 1:
                    wall[c.r1 - 1][c.c1][1] = true; break;
                case 2:
                    wall[c.r1][c.c1 - 1][0] = true; break;
                case 3:
                    wall[c.r1][c.c1][1] = true; break;
            }

            if (c.c2 < cols - 1 && !visited[c.r2][c.c2 + 1]) {
                w = gorila.nextInt(MAXW);
                q.add(new Movement(c.r2, c.c2, c.r2, c.c2 + 1, 0, w));
            } if (c.r2 > 0 && !visited[c.r2 - 1][c.c2]) {
                w = gorila.nextInt(MAXW);
                q.add(new Movement(c.r2, c.c2, c.r2 - 1, c.c2, 1, w));
            } if (c.c2 > 0 && !visited[c.r2][c.c2 - 1]) {
                w = gorila.nextInt(MAXW);
                q.add(new Movement(c.r2, c.c2, c.r2, c.c2 - 1, 2, w));
            } if (c.r2 < rows - 1 && !visited[c.r2 + 1][c.c2]) {
                w = gorila.nextInt(MAXW);
                q.add(new Movement(c.r2, c.c2, c.r2 + 1, c.c2, 3, w));
            }
        }
    }


    public void paintComponent(Graphics gg) {
        super.paintComponent(gg);
        Graphics2D g = (Graphics2D) gg;
        setBackground(Color.BLACK);
        setSize(width, height);

        g.setPaint(Color.WHITE);
        for (int i = 0; i < cols; i++) {
            for (int j = 0; j < rows; j++) {
                g.fillRect(j * boxSize + wallThickness, 
                            i * boxSize + wallThickness, 
                            innerBoxSize, innerBoxSize);
                if (wall[i][j][0])
                    g.fillRect((j + 1) * boxSize, 
                                 i * boxSize + wallThickness, 
                                 wallThickness, innerBoxSize);
                if (wall[i][j][1])
                    g.fillRect(j * boxSize + wallThickness,
                                (i + 1) * boxSize, 
                                innerBoxSize, wallThickness);
            }
        }

        g.fillRect(wallThickness, 0, innerBoxSize, wallThickness);
        g.fillRect((cols - 1) * boxSize + wallThickness, 
                    rows * boxSize, innerBoxSize, wallThickness);
    }


    static final String usage = "Usage: maze ROWS COLUMNS";

    public static void main (String[] argv) {
        if (argv.length != 2) {
            System.out.println(usage);
            System.exit(1);
        }

        try {
            rows = Integer.parseInt(argv[0]);
            cols = Integer.parseInt(argv[1]);
        } catch (NumberFormatException nfe) {
            System.out.println(usage);
            System.exit(1);
        }

        boxSize = 8;
        wallThickness = boxSize / 2;
        innerBoxSize = boxSize - wallThickness;

        width = wallThickness + cols * boxSize;
        height = wallThickness + rows * boxSize;

        // Generate maze.
        generate(rows, cols);

        JFrame frame = new JFrame("Maze generator ("+rows+" x "+cols+")");
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.setResizable(false);

        Maze m = new Maze();
        m.setPreferredSize(new Dimension(width, height));

        frame.add(m);
        frame.pack();
        frame.setVisible(true);
    }
}
