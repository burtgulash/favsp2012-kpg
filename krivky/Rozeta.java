import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.geom.GeneralPath;
import java.awt.Color;
import java.awt.image.BufferedImage;

import java.io.File;
import java.io.IOException;
import javax.imageio.ImageIO;


public class Rozeta {
	private static int width = 1600, height = 1600;
	private static final double xlo = 0;
	private static final double xhi = 100;
	private static final double ylo = 0;
	private static final double yhi = 100;

	private static int x(double x) { return (int) (x / (xhi - xlo) * width); }
	private static int y(double y) { return (int) (y / (yhi - ylo) * height); }

	public static void main (String... args) throws IOException {
		BufferedImage img = new BufferedImage(width, height, 
                                              BufferedImage.TYPE_INT_RGB);
		Graphics2D gg = img.createGraphics();
		gg.setPaint(Color.WHITE);
		gg.fillRect(0, 0, width, height);
		gg.setPaint(Color.BLACK);

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
		ImageIO.write(img, "png", new File("test_rozeta.png"));
	}
}
