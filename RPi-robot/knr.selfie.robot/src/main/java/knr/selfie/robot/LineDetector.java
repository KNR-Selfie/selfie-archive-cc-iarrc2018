package knr.selfie.robot;

import java.util.ArrayList;
import java.util.List;

import org.opencv.core.Mat;
import org.opencv.core.MatOfPoint;
import org.opencv.core.Point;
import org.opencv.core.Rect;
import org.opencv.core.Scalar;
import org.opencv.core.Size;
import org.opencv.imgproc.Imgproc;
import org.opencv.imgproc.Moments;

public class LineDetector {
	private static final int HEIGHT = 50;
	private static final int WIDTH = 640;
	
	private final Mat erodeElmt = Imgproc.getStructuringElement(Imgproc.MORPH_RECT, new Size(3, 3));
    private final Mat dilateElmt = Imgproc.getStructuringElement(Imgproc.MORPH_RECT, new Size(5, 5));
	
	private List<Point> centers;
	private Mat originalFrame;
	private Mat processedFrame;
	private Point roiP1;
	private Point roiP2;
	
	private double error=0.0; // -100...+100
	
	public LineDetector(){
		
	}
	
	void detectLine(Mat mOriginalFrame){
		this.originalFrame=mOriginalFrame;
		this.processedFrame=mOriginalFrame;
		
		//Getting originalFrame width and height;
		int origWidth=this.originalFrame.width();
		int origHeight = this.originalFrame.height();
		
		//Creating point defining ROI rectangle
				roiP1 = new Point(0, 0);
				roiP2 = new Point(origWidth, HEIGHT);;
		
		//Creating ROI rectangle
		Rect roiRect = new Rect(roiP1, roiP2);
		
		//Creating ROI
		Mat roi = new Mat(this.originalFrame, roiRect);
		
        //System.out.println("roi: " + roi.size());

        Imgproc.cvtColor(roi, roi, Imgproc.COLOR_BGR2GRAY);
        Imgproc.GaussianBlur(roi, roi, new Size(9, 9), 2, 2);
        Imgproc.threshold(roi, roi, 0, 255, Imgproc.THRESH_BINARY_INV | Imgproc.THRESH_OTSU);

        Imgproc.erode(roi, roi, erodeElmt);
        Imgproc.dilate(roi, roi, dilateElmt);
		
		//this.processedFrame=roi;
        
        //Getting contours
        List<MatOfPoint> contours = new ArrayList<>();
        Imgproc.findContours(roi, contours, new Mat(), Imgproc.RETR_LIST, Imgproc.CHAIN_APPROX_SIMPLE);

        //Calculating centers
        centers = new ArrayList<>();
        for (MatOfPoint contour : contours) {
            Moments mu = Imgproc.moments(contour);
            if (mu.get_m00() > 100) {
                int cx = (int) (mu.get_m10() / mu.get_m00());
                int cy = (int) (mu.get_m01() / mu.get_m00());
                centers.add(new Point(cx, cy));
                //Imgproc.circle(this.processedFrame, new Point(cx, cy), 5, new Scalar(0,255,0));
            }
        }
		
	}
	
	public double getError(){
		if(centers !=null && centers.size()>0){
			this.error=centers.get(0).x;
			this.error=200.0/(double)WIDTH*this.error-100.0;
			//System.out.println("error: " + error);
		}
		return this.error;
	}
	
	Mat getProcessedFrame(){
		//Draw ROI
		Imgproc.rectangle(processedFrame, roiP1, roiP2, new Scalar(0,255,0), 3);
		
		//Draw centers
        if(centers !=null && centers.size()>0){
        	for (Point p : centers){
        		Imgproc.circle(this.processedFrame, new Point(roiP1.x+p.x,roiP1.y+p.y), 5, new Scalar(0,255,0));
        	}
        }
		
		return this.processedFrame;
        
	}
	
	

}