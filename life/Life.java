import java.awt.Color;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.image.BufferedImage;
import java.awt.Dimension;

import javax.swing.JPanel;
import javax.swing.JFrame;
import javax.imageio.ImageIO;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.File;
import java.io.IOException;

class Board extends JPanel {
    final int cellsX, cellsY;
    final int cellSize;

    final int maxAge = 300;
    final int[][] BLACK_WHITE = new int[][] {{0, maxAge}, {0xFFFFFF, 0xFF0000}};
    final int[][] CMAP = new int[][] 
         {{0, 15, 30, 60, 120, maxAge},
           {0xFFFFFF, 0xb1e70c, 0x26e70c, 0x0db8c3, 0x40268a, 0x453f55}};

    int[][][] age;
    byte[][][] lives;
    int iteration = 0;

    Board(byte[][] board, int cellSize) {
        cellsX = board[0].length;
        cellsY = board.length;

        this.cellSize = cellSize;

        age = new int[2][cellsY + 2][cellsX + 2];
        lives = new byte[2][cellsY + 2][cellsX + 2];

        for (int r = 0; r < cellsY; r++) 
            for (int c = 0; c < cellsX; c++)
                lives[0][r + 1][c + 1] = board[r][c];
    }


    void run() {
        try {
            for (;;) {
                repaint();
                nextIteration();
                Thread.sleep(30);
            }
        } catch (InterruptedException ex) {}
    }


    private Color blendColor(int[][] colorMap, int age) {
        int i = 0;

        for (; i < colorMap[0].length; i++) {
            if (colorMap[0][i] == age)
                return new Color(colorMap[1][i]);
            if (colorMap[0][i] > age) {
                int s = age - colorMap[0][i - 1];
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

    private void plotAges(Graphics2D g) {
        for (int r = 1; r <= cellsY; r++) {
            for (int c = 1; c <= cellsX; c++) {
                if (lives[iteration][r][c] == 1)
                    g.setPaint(blendColor(CMAP, age[iteration][r][c]));
                else 
                    g.setPaint(Color.BLACK);

                g.fillRect((c - 1) * cellSize, 
                           (r - 1) * cellSize, cellSize, cellSize);
            }
        }

        repaint();
    }


    private void nextIteration() {
        int livingNeighbours = 0;
        int next = iteration ^ 1;

        for (int r = 1; r <= cellsY; r++) {
            for (int c = 1; c <= cellsX; c++) {
                livingNeighbours = lives[iteration][r][c + 1] + 
                                   lives[iteration][r + 1][c + 1] + 
                                   lives[iteration][r + 1][c] + 
                                   lives[iteration][r + 1][c - 1] + 
                                   lives[iteration][r][c - 1] + 
                                   lives[iteration][r - 1][c - 1] +
                                   lives[iteration][r - 1][c] + 
                                   lives[iteration][r - 1][c + 1];

                if (lives[iteration][r][c] == 1) {
                    if (livingNeighbours < 2 || livingNeighbours > 3) {
                        age[next][r][c] = 0;
                        lives[next][r][c] = 0;
                    } else {
                        age[next][r][c] = 
                            Math.min(age[iteration][r][c] + 1, maxAge);
                        lives[next][r][c] = 1;
                    }
                } else {
                    if (livingNeighbours == 3) {
                        age[next][r][c] = 1;
                        lives[next][r][c] = 1;
                    } else {
                        age[next][r][c] = 0;
                        lives[next][r][c] = 0;
                    }
                }
            }
        }

        iteration = next;
    }


    public void paintComponent(Graphics gg) {
        super.paintComponent(gg);
        Graphics2D g = (Graphics2D) gg;
        plotAges(g);
    }
}

public class Life {
    private static byte[][] convertToBytes(int[] rgbs, int w, int h) {
        byte[][] board = new byte[h][w];

        for (int r = 0; r < h; r++) 
            for (int c = 0; c < w; c++)
                board[r][c] = (byte) (rgbs[r * w + c] == 0xFFFFFFFF ? 1 : 0);

        return board;
    }

    public static void main (String[] argv) throws IOException {
        if (argv.length != 1) {
            System.err.println("Usage: life BOARD.image");
            System.exit(1);
        }
        int cellSize = 2;

        BufferedImage img = ImageIO.read(new File(argv[0]));
        int w = img.getWidth();
        int h = img.getHeight();

        int [] rgbs = new int[w * h];
        img.getRGB(0, 0, w, h, rgbs, 0, w);
        byte[][] board = convertToBytes(rgbs, w, h);
        Board bb = new Board(board, cellSize);
        bb.setPreferredSize(new Dimension(cellSize * w, cellSize * h));

        JFrame frame = new JFrame("Conway's game of life ("+w+" x "+h+")");
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

        frame.add(bb);
        frame.setVisible(true);
        frame.setResizable(false);
        frame.pack();

        bb.run();
    }
}
