package org.github.perciax.opencvbasics;


import java.awt.image.BufferedImage;
import java.awt.image.DataBufferByte;

import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;

import org.opencv.core.Mat;

/**
 * @author perciax
 *
 */
public class ImageWindow {
	JFrame window;
	ImageIcon image;
	JLabel label;
	
	/**
	 * 
	 */
	public ImageWindow(){
		window = new JFrame();
		image = new ImageIcon();
		label = new JLabel();
		label.setIcon(image);
		window.getContentPane().add(label);
		window.setResizable(false);
		window.setTitle("title");
		window.setResizable(false);
		window.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
	}
	
	/**
	 * @param buffImage
	 */
	public void showImage(BufferedImage buffImage){
		image.setImage(buffImage);
		window.pack();
		label.updateUI();
		window.setVisible(true);
	}
	
	/**
	 * @param mat
	 */
	public void showImage(Mat mat){
		BufferedImage buffImage = mat2BufferedImage(mat);
		image.setImage(buffImage);
		window.pack();
		label.updateUI();
		window.setVisible(true);
	}
	
	/**
	 * @param mat
	 * @return
	 */
	private BufferedImage mat2BufferedImage(Mat mat){
		BufferedImage buffImage = null;
		try {
			int type = BufferedImage.TYPE_BYTE_GRAY;
			if (mat.channels() > 1) {
				type = BufferedImage.TYPE_3BYTE_BGR;
			}
			int bufferSize = mat.channels() * mat.cols() * mat.rows();
			byte[] b = new byte[bufferSize];
			mat.get(0, 0, b); // get all the pixels
			buffImage = new BufferedImage(mat.cols(), mat.rows(), type);
			final byte[] targetPixels = ((DataBufferByte) buffImage.getRaster()
					.getDataBuffer()).getData();
			System.arraycopy(b, 0, targetPixels, 0, b.length);
			
		} catch (Exception e) {
			e.printStackTrace();
		}
		return buffImage;
	}

}
