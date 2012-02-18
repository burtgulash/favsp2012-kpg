import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.geom.GeneralPath;
import java.awt.Color;

import javax.swing.JFrame;
import javax.swing.JPanel;

interface PolarFunction {
	public double eval(double theta);
}

class Canvaso extends JPanel {
	private final int width, height;
	private final double xlo = 0, xhi = 100, ylo = 0, yhi = 50;

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


	private void makeCurve(PolarFunction f, GeneralPath p, double cx, double cy)
	{
		double step = 1e-3;
		double r;

		p.moveTo(x(cx), y(cy));
		for (double t = 0; t <= 2.0 * Math.PI; t += step) {
			r = f.eval(t);
			p.lineTo(x(cx + r * Math.cos(t)), y(cy + r * Math.sin(t)));
		}
	}
	
	public void paintComponent(Graphics g) {
		super.paintComponent(g);
		Graphics2D gg = (Graphics2D) g;
		setBackground(Color.WHITE);

		PolarFunction[] fs = new PolarFunction[] {
			new PolarFunction() {
				public double eval(double t) { 
					return 2 * t; 
				}
			},
			new PolarFunction() {
				public double eval(double t) { 
					return 6 * Math.sin(4 * t); 
				}
			},
			new PolarFunction() {
				public double eval(double t) { 
					return 6 * (3 * Math.cos(t) - Math.cos(3 * t));
				}
			},
			new PolarFunction() {
				public double eval(double t) { 
					return 6 * (1 - Math.cos(t));
				}
			}
		};

		for (int i = 0; i < fs.length; i++) {
			GeneralPath p = new GeneralPath();
			makeCurve(fs[i], p, (i + 1) * xhi / 5, 25);
			gg.draw(p);
		}
	}
}

public class Test {
	private static int width = 1000, height = 500;

	public static void main (String... args) {
		JFrame frame = new JFrame("Pokus");
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		frame.setSize(width, height);
		frame.add(new Canvaso(width, height));
		frame.setVisible(true);
	}
}
