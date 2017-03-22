package knr.selfie.robot;

import java.io.IOException;

import com.pi4j.io.i2c.I2CFactory.UnsupportedBusNumberException;
import com.pi4j.platform.PlatformAlreadyAssignedException;

public class MotorHat {
	// PCA9685 device - I2C address
    public static final int PCA9685_ADDR = 0x60;
    
    
	public MotorHat() throws InterruptedException, PlatformAlreadyAssignedException, IOException, UnsupportedBusNumberException{
        
        I2CController i2c =new I2CController(PCA9685_ADDR);
        
       
	    i2c.doShits();
	    
	    i2c.reset();	        	
            
	}
	
	
	class Motor{
		
		public Motor(int firstAddr){
			
			
		}
		
	}
}
