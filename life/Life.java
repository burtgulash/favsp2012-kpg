import java.awt.Color;
import java.awt.Graphics;
import java.awt.Graphics2D;

import javax.swing.JPanel;
import javax.swing.JFrame;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;

class Canvaso extends JPanel {
	final int width, height;
	final int cells;
	final int cellSize;

	int[][][] age;
	byte[][][] lives;
	int iteration = 0;

	Canvaso(int size, int cellSize) {	
		width = size;
		height = size;
		cells = size / cellSize;

		age = new int[2][cells + 2][cells + 2];
		lives = new byte[2][cells + 2][cells + 2];

		this.cellSize = cellSize;
	}

	Canvaso(int size, int cellSize, byte[][] board) {
		this(size, cellSize);
		for (int i = 1; i < Math.min(cells, board.length); i++) {
			for (int j = 1; j < Math.min(cells, board[0].length); j++) {
				age[0][i][j] = (int) board[i][j];
				lives[0][i][j] = board[i][j];
			}
		}
	}

	void run() {
		try {
			for (;;) {
				repaint();
				nextIteration();
				Thread.sleep(1);
			}
		} catch (InterruptedException ex) {}
	}


	private void plotAges(Graphics2D g) {
		for (int r = 1; r < cells; r++) {
			for (int c = 1; c < cells; c++) {
				if (lives[iteration][r][c] == 1)
					g.setPaint(Color.WHITE);
				else
					g.setPaint(Color.BLACK);

				g.fillRect(c * cellSize, r * cellSize, cellSize, cellSize);
			}
		}

		repaint();
	}


	private void nextIteration() {
		int livingNeighbours = 0;
		int next = iteration ^ 1;

		for (int r = 1; r < cells; r++) {
			for (int c = 1; c < cells; c++) {
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
						age[next][r][c] = age[iteration][r][c] + 1;
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
		setBackground(Color.BLACK);

		plotAges(g);
	}
}

public class Life {
	public static void main (String[] argv) {
		int size = 800;
		int cellSize = 8;
		byte[][] board = new byte[size / cellSize][size / cellSize];

		Canvaso ccc = null;

		if (argv.length == 1) {
			BufferedReader br = null;
			try {
				br = new BufferedReader(new FileReader(argv[0]));

				String s;
				for (int r = 1; (s = br.readLine()) != null; r++)
					for (int c = 1; c < s.length(); c++)
						board[r][c] = (byte) (s.charAt(c) == '0' ? 0 : 1);
			} catch (IOException ex) {}
			finally {
				try {
					br.close();
				} catch (IOException e) {}
			}


			ccc = new Canvaso(size, cellSize, board);
		} else
			ccc = new Canvaso(size, cellSize);
		
		JFrame frame = new JFrame("Conway's game of life");
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		frame.setSize(size, size);
		
		frame.add(ccc);
		frame.setVisible(true);

		ccc.run();
		
		frame.repaint();
	}
}
