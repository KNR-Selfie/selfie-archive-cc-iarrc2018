package knr.selfie.robot;

import java.io.IOException;

import org.opencv.core.Core;

import com.pi4j.io.i2c.I2CFactory.UnsupportedBusNumberException;
import com.pi4j.platform.PlatformAlreadyAssignedException;

public class Robot {
	


	public static void main(String[] args) {
		
		// Load the OpenCV library.
        System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
        
        // Print OpenCV info
        System.out.println(Core.NATIVE_LIBRARY_NAME.toString());
		System.out.println("OpenCV works fine!");
		
		// Create MotorController
		try {
			MotorHat mh = new MotorHat();

		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (PlatformAlreadyAssignedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (UnsupportedBusNumberException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		
		
		System.out.println("KONIEC");
	}

}
