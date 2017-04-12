package knr.selfie.helloopencv;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.videoio.VideoCapture;

public class HelloOpenCV {

	public static void main(String[] args) {
  
            System.out.println("Hello, OpenCV");
            // Load the native library.
            System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
            System.out.println(Core.NATIVE_LIBRARY_NAME.toString());
            
            Mat mat = Mat.eye(3, 3, CvType.CV_8UC1);
            System.out.println("mat = " + mat.dump());
     
            VideoCapture camera = new VideoCapture(0);
            try 
            {
                Thread.sleep(1000);
            } catch (Exception e) {
                e.printStackTrace();
            }
            if(!camera.isOpened()){
                System.out.println("Camera Error");
            }
            else{
                System.out.println("Camera OK?");
            }
     
            Mat frame = new Mat();
            camera.read(frame);
            Imgcodecs.imwrite("capture.jpg", frame);
            
            camera.release();
	}

}
