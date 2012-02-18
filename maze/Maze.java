import java.util.Stack;
import java.util.Random;

import java.awt.image.BufferedImage;
import java.awt.Graphics2D;

import java.io.File;
import java.io.IOException;
import javax.imageio.ImageIO;

public class Maze {
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

	static final String usage = "Usage: maze FILE.png ROWS COLUMNS";

	public static void main (String[] argv) {
		
		if (argv.length != 3) {
			System.out.println(usage);
			System.exit(1);
		}

		int rows = 0, cols = 0;
		try {
			rows = Integer.parseInt(argv[1]);
			cols = Integer.parseInt(argv[2]);
		} catch (NumberFormatException nfe) {
			System.out.println(usage);
			System.exit(1);
		}

		int width = rows * 2;
		int height = width * rows / cols;

		int boxSize = width / rows;
		int wallThick = boxSize / 2;
		int innerBoxSize = boxSize - wallThick;
		

		BufferedImage img = new BufferedImage(width + wallThick, 
                                              height + wallThick, 
                                              BufferedImage.TYPE_BYTE_GRAY);
		Graphics2D gg = img.createGraphics();
		
		generate(0, rows, cols);
		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < cols; j++) {
				gg.fillRect(j * boxSize + wallThick, 
                            i * boxSize + wallThick, 
                            innerBoxSize, innerBoxSize);

				if (wall[i][j][0])
					gg.fillRect((j + 1) * boxSize, 
                                 i * boxSize + wallThick, 
                                 wallThick, innerBoxSize);
				if (wall[i][j][1])
					gg.fillRect(j * boxSize + wallThick,
                                (i + 1) * boxSize, 
                                innerBoxSize, wallThick);
			}
		}

		gg.fillRect(wallThick, 0, innerBoxSize, wallThick);
		gg.fillRect((cols - 1) * boxSize + wallThick, 
                    rows * boxSize, innerBoxSize, wallThick);

		
		try {
			ImageIO.write(img, "png", new File(argv[0]));
		} catch (IOException ioe) {
			System.out.println(usage);
			System.exit(1);
		}
	}
}
