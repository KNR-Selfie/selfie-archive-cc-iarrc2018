package knr.selfie.robot;

import java.io.IOException;

import org.github.perciax.opencvbasics.ImageWindow;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.videoio.VideoCapture;

import com.pi4j.io.i2c.I2CFactory.UnsupportedBusNumberException;
import com.pi4j.platform.PlatformAlreadyAssignedException;

public class Robot {
	//static ImageWindow lineDetImage;

	public static void main(String[] args) throws InterruptedException, PlatformAlreadyAssignedException, IOException, UnsupportedBusNumberException {
		
		// Load the OpenCV library.
        System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
        
        // Print OpenCV info
        System.out.println(Core.NATIVE_LIBRARY_NAME.toString());
		//System.out.println("OpenCV works fine!");
		
        VideoCapture camera = new VideoCapture(0);
        // Create MotorController
		MotorHat mh = new MotorHat();
	
		System.out.println("MotorHat created");
        
		// Create LineDetector
		LineDetector ld = new LineDetector();
		
			for(int i=0; i<1000; i++){
				
				
	            if(!camera.isOpened()){
	                System.out.println("Camera Error");
	            }
	            else{
	                //System.out.println("Camera OK");
	            }
	     
	            Mat frame = new Mat();
	            camera.read(frame);
	            
				
				ld.detectLine(frame);
				double error = ld.getError();
				System.out.println("Error: "+ error);
				//frame=ld.getProcessedFrame();
				//lineDetImage.showImage(frame);
				int k=10;
				mh.motorGo(mh.mA, false, (int)(1000+k*error));
				mh.motorGo(mh.mB, false, (int)(1000-k*error));
					
				Thread.sleep(20);
			}
		
		mh.motorGo(mh.mA, false, 0);
		mh.motorGo(mh.mB, false, 0);
		
		camera.release();
		System.out.println("KONIEC");
	}

}
