import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.geom.GeneralPath;
import java.awt.Color;

import javax.swing.JFrame;
import javax.swing.JPanel;


class Canvaso extends JPanel {
	private final int width, height;
	private final double xlo = 0, xhi = 100, ylo = 0, yhi = 100;

	Canvaso(int width, int height) {	
		this.width = width;
		this.height = height;
	}

	private int x(double x) {
		return (int) (x * (width - 1) / (xhi - xlo));
	}

	private int y(double y) {
		return (int) ((height - 1) - y * (height - 1) / (yhi - ylo));
	}
	
	public void paintComponent(Graphics g) {
		super.paintComponent(g);
		Graphics2D gg = (Graphics2D) g;
		setBackground(Color.WHITE);

		int size = 31;
		double r = 40.0;
		double[] xs = new double[size];
		double[] ys = new double[size];
		double center_x = 50.0, center_y = 50.0;

		for (int i = 0; i < size; i++) {
			xs[i] = r * Math.cos(2.0 * Math.PI * (double) i / (double) size);
			ys[i] = r * Math.sin(2.0 * Math.PI * (double) i / (double) size);

			xs[i] += center_x;
			ys[i] += center_y;
		}

		GeneralPath p = new GeneralPath();

		p.moveTo(x(xs[0]), y(ys[0]));
		for (int i = 0; i < size / 2; i++)
			for (int j = 0; j < size; j++) {
				int point = ((i + 1) * (j + 1)) % size;
				p.lineTo(x(xs[point]), y(ys[point]));
			}

		gg.draw(p);
	}
}

public class Test {
	private static int width = 800, height = 800;

	public static void main (String... args) {
		JFrame frame = new JFrame("Pokus");
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		frame.setSize(width, height);
		frame.add(new Canvaso(width, height));
		frame.setVisible(true);
	}
}
