import java.util.Stack;
import java.util.Random;

import java.awt.Graphics2D;
import java.awt.Graphics;
import java.awt.Color;
import java.awt.Dimension;

import javax.swing.JPanel;
import javax.swing.JFrame;


public class Maze extends JPanel {
    static int rows = 0, cols = 0;
    static int width, height;
    static int boxSize, innerBoxSize, wallThickness;

    static boolean[][] visited;
    static boolean[][][] wall;

    static final Random gorila = new Random();
    static final int[][] permutation = {
                {1, 2, 3, 4}, {1, 2, 4, 3}, {1, 3, 2, 4}, {1, 3, 4, 2},
                {1, 4, 2, 3}, {1, 4, 3, 2}, {2, 1, 3, 4}, {2, 1, 4, 3},
                {2, 3, 1, 4}, {2, 3, 4, 1}, {2, 4, 1, 3}, {2, 4, 3, 1},
                {3, 1, 2, 4}, {3, 1, 4, 2}, {3, 2, 1, 4}, {3, 2, 4, 1},
                {3, 4, 1, 2}, {3, 4, 2, 1}, {4, 1, 2, 3}, {4, 1, 3, 2},
                {4, 2, 1, 3}, {4, 2, 3, 1}, {4, 3, 1, 2}, {4, 3, 2, 1}};


    static void generate(int start, int rows, int cols) {
        visited = new boolean[rows][cols];
        wall    = new boolean[rows][cols][2];

        Stack<Integer> s = new Stack<Integer>();
        s.push(start);

        while (!s.empty()) {
            int pos = s.pop();
            int r = pos / cols;
            int c = pos % cols;

            int rand = Math.abs(gorila.nextInt()) % 24;
            for (int i = 1; i <= 4; i++) {
                switch(permutation[rand][i - 1]) {
                case 1: 
                    if(c + 1 < cols && !visited[r][c + 1]) {
                        visited[r][c + 1] = true;
                        wall[r][c][0] = true;
                        s.push(r * cols + c + 1);
                    }
                    break;
                case 2:
                    if(r > 0 && !visited[r - 1][c]) {
                        visited[r - 1][c] = true;
                        wall[r - 1][c][1] = true;
                        s.push(r * cols - cols + c);
                    }
                    break;
                case 3:
                    if(c > 0 && !visited[r][c - 1]) {
                        visited[r][c - 1] = true;
                        wall[r][c - 1][0] = true;
                        s.push(r * cols + c - 1);
                    }
                    break;
                case 4:
                    if(r + 1 < rows && !visited[r + 1][c]) {
                        visited[r + 1][c] = true;
                        wall[r][c][1] = true;
                        s.push(r * cols + cols + c);
                    }
                    break;
                }
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
        generate(0, rows, cols);

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
