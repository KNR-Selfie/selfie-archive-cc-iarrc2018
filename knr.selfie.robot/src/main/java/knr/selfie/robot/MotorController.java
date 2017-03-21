package knr.selfie.robot;

import java.io.IOException;
import java.util.concurrent.TimeUnit;

import org.opencv.core.Core;

import com.pi4j.io.i2c.I2CBus;
import com.pi4j.io.i2c.I2CDevice;
import com.pi4j.io.i2c.I2CFactory;
import com.pi4j.io.i2c.I2CFactory.UnsupportedBusNumberException;
import com.pi4j.platform.PlatformAlreadyAssignedException;
import com.pi4j.util.Console;

public class MotorController {
	
	// PCA9685 device - I2C address
    public static final int PCA9685_ADDR = 0x60;
    
	void controlMotor() throws InterruptedException, PlatformAlreadyAssignedException, IOException, UnsupportedBusNumberException{
        
        I2CController i2c =new I2CController(PCA9685_ADDR);
        
        i2c.reset();
       
	    i2c.doShits();
	    
	    i2c.reset();	        	
	    	
        System.out.println("KONIEC");
            
	}
	

	
	
	
	

}
