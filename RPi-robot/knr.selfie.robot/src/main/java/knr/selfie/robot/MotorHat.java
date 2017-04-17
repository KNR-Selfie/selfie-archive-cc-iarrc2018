package knr.selfie.robot;

import java.io.IOException;
import java.util.concurrent.TimeUnit;

import com.pi4j.io.i2c.I2CFactory.UnsupportedBusNumberException;
import com.pi4j.platform.PlatformAlreadyAssignedException;

public class MotorHat {
	// PCA9685 device - I2C address
    public static final int PCA9685_ADDR = 0x60;
    private static I2CController i2c;
    Motor mA;
	Motor mB;
    
	public MotorHat() throws InterruptedException, PlatformAlreadyAssignedException, IOException, UnsupportedBusNumberException{
        
        i2c = new I2CController(PCA9685_ADDR);
        
        mA = new Motor(I2CController.IN_A1,I2CController.IN_A2,I2CController.PWMA);
        mB = new Motor(I2CController.IN_B1,I2CController.IN_B2,I2CController.PWMB);

	    //i2c.reset();	        	
            
	}
	
	void motorGo(Motor motor, boolean dir, int speedValue){
		i2c.digitalWrite(motor.in1, dir);
		i2c.digitalWrite(motor.in2, !dir);
		i2c.analogWrite(motor.pwmChannel, speedValue);
	}
	
	void motorStop(Motor motor){
		i2c.digitalWrite(motor.in1, false);
		i2c.digitalWrite(motor.in2, false);
		i2c.analogWrite(motor.pwmChannel, 0);
	}
	
	void motorHardStop(Motor motor){
		i2c.digitalWrite(motor.in1, true);
		i2c.digitalWrite(motor.in2, true);
		i2c.analogWrite(motor.pwmChannel, 0);
	}
	
	class Motor{
		
		int in1;
		int in2;
		int pwmChannel;
		int speedValue; //0-4096
		
		public Motor(int in1, int in2, int pwmChannel){
			this.in1=in1;
			this.in2=in2;
			this.pwmChannel=pwmChannel;
		}
	}
	
}
