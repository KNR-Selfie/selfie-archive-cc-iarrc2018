package knr.selfie.robot;

import java.io.IOException;
import java.util.concurrent.TimeUnit;

import com.pi4j.io.i2c.I2CBus;
import com.pi4j.io.i2c.I2CDevice;
import com.pi4j.io.i2c.I2CFactory;
import com.pi4j.io.i2c.I2CFactory.UnsupportedBusNumberException;

public class I2CController {
	
	private static final int BROADCAST_ADDR = 0x00;
	
    public static final byte MODE1 = (byte)0x00;
    public static final byte MODE2 = (byte)0x01;
    
    // PWM channel
    public static final byte LED2_ON_L = (byte)0x0E;
    public static final byte LED2_ON_H = (byte)0x0F;
    public static final byte LED2_OFF_L = (byte)0x10;
    public static final byte LED2_OFF_H = (byte)0x11;
    
    // IN2 
    public static final byte LED3_ON_L = (byte)0x12;
    public static final byte LED3_ON_H = (byte)0x13;
    public static final byte LED3_OFF_L = (byte)0x14;
    public static final byte LED3_OFF_H = (byte)0x15;
    
    // IN1
    public static final byte LED4_ON_L = (byte)0x16;
    public static final byte LED4_ON_H = (byte)0x17;
    public static final byte LED4_OFF_L = (byte)0x18;
    public static final byte LED4_OFF_H = (byte)0x19;
	
	private static I2CBus i2c;
	private static I2CDevice device;
	private static I2CDevice broadcast;
	
	//constructor
	public I2CController(int deviceAddr) throws InterruptedException, IOException, UnsupportedBusNumberException{
		// get the I2C bus to communicate on
        i2c = I2CFactory.getInstance(I2CBus.BUS_1);
        
        // get the I2C device - PCA9685
        device = i2c.getDevice(deviceAddr);
        
        // get the I2C broadcast address
        broadcast = i2c.getDevice(BROADCAST_ADDR);
	}
	
	void doShits() throws InterruptedException, IOException{
		int mode1 = device.read(MODE1);
        //console.println("MODE1 = " + String.format("0x%02x", mode1));
        
        if((byte)mode1==0x11){
        	//console.println("USTAWIANIE MODÓW");
	        int mode2 = device.read(MODE2);
	        //console.println("MODE2 = " + String.format("0x%02x", mode2));
	        
	        device.write(MODE2,(byte)(mode2 | 0x08));
	        Thread.sleep(500);
	        
	        mode2 = device.read(MODE2);
	        //console.println("MODE2 = " + String.format("0x%02x", mode2));
	        
	        device.write(MODE1,(byte)(mode1 & ~0x10));
	        Thread.sleep(500);
	        
	        mode1 = device.read(MODE1);
	        //console.println("MODE1 = " + String.format("0x%02x", mode1));
        }
        //console.println("USTAWIANIE LEDÓW");
        
        	//*****************LED2
        	device.write(LED2_ON_L, (byte)0xFF);
        	device.write(LED2_ON_H, (byte)0x10);
        	Thread.sleep(500);
        	
        	device.write(LED2_OFF_L, (byte)0x00);
        	device.write(LED2_OFF_H, (byte)0x00);
        	Thread.sleep(500);

            //************************LED3
            device.write(LED3_ON_L, (byte)0xFF);
        	device.write(LED3_ON_H, (byte)0x10);
        	Thread.sleep(500);
        	
        	device.write(LED3_OFF_L, (byte)0x00);
        	device.write(LED3_OFF_H, (byte)0x00);
        	Thread.sleep(500);

          //************************LED4
            
            
	            device.write(LED4_ON_L, (byte)0x00);
	        	device.write(LED4_ON_H, (byte)0x00);
	        	Thread.sleep(500);
	        	
	        	device.write(LED4_OFF_L, (byte)0x00);
	        	device.write(LED4_OFF_H, (byte)0x00);
	        	Thread.sleep(500);
	        	
	        	
	        	
	        	TimeUnit.SECONDS.sleep(5);
	}
	
	void reset() throws InterruptedException, IOException{
        // send broadcasting RESET command
        broadcast.write((byte)0x06);
        
        // wait from reset
        Thread.sleep(500);
	}
	
	

}
