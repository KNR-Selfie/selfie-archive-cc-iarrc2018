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
    
 // BIN1
    public static final byte LED5_ON_L = (byte)0x1A;
    public static final byte LED5_ON_H = (byte)0x1B;
    public static final byte LED5_OFF_L = (byte)0x1C;
    public static final byte LED5_OFF_H = (byte)0x1D;
    
 // BIN2
    public static final byte LED6_ON_L = (byte)0x1E;
    public static final byte LED6_ON_H = (byte)0x1F;
    public static final byte LED6_OFF_L = (byte)0x20;
    public static final byte LED6_OFF_H = (byte)0x21;
    
 // B PWM
    public static final byte LED7_ON_L = (byte)0x22;
    public static final byte LED7_ON_H = (byte)0x23;
    public static final byte LED7_OFF_L = (byte)0x24;
    public static final byte LED7_OFF_H = (byte)0x25;
    
    // PWMC
    public static final byte LED8_ON_L = (byte)0x26;
    public static final byte LED8_ON_H = (byte)0x27;
    public static final byte LED8_OFF_L = (byte)0x28;
    public static final byte LED8_OFF_H = (byte)0x29;
    
    // IN_C2
    public static final byte LED9_ON_L = (byte)0x2A;
    public static final byte LED9_ON_H = (byte)0x2B;
    public static final byte LED9_OFF_L = (byte)0x2C;
    public static final byte LED9_OFF_H = (byte)0x2D;
    
    // IN1_C1
    public static final byte LED10_ON_L = (byte)0x2E;
    public static final byte LED10_ON_H = (byte)0x2F;
    public static final byte LED10_OFF_L = (byte)0x30;
    public static final byte LED10_OFF_H = (byte)0x31;
	
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
        
        this.reset();
        this.init();
	}
	
	void doShits() throws InterruptedException, IOException{
			
		
		device.write(LED2_ON_L, (byte)0x00);
		device.write(LED2_ON_H, (byte)0x00);
		device.write(LED2_OFF_L, (byte)0xFF);
    	device.write(LED2_OFF_H, (byte)0x0F);
    	
    	device.write(LED3_ON_L, (byte)0x00);
		device.write(LED3_ON_H, (byte)0x00);
		device.write(LED3_OFF_L, (byte)0x00);
    	device.write(LED3_OFF_H, (byte)0x00);
    	
    	device.write(LED4_ON_L, (byte)0x00);
		device.write(LED4_ON_H, (byte)0x10);
		device.write(LED4_OFF_L, (byte)0x00);
    	device.write(LED4_OFF_H, (byte)0x00);
    	
    	device.write(LED5_ON_L, (byte)0x00);
		device.write(LED5_ON_H, (byte)0x10);
		device.write(LED5_OFF_L, (byte)0x00);
    	device.write(LED5_OFF_H, (byte)0x00);
    	
    	device.write(LED6_ON_L, (byte)0x00);
		device.write(LED6_ON_H, (byte)0x00);
		device.write(LED6_OFF_L, (byte)0x00);
    	device.write(LED6_OFF_H, (byte)0x00);
    	
    	device.write(LED7_ON_L, (byte)0x00);
		device.write(LED7_ON_H, (byte)0x00);
		device.write(LED7_OFF_L, (byte)0xFF);
    	device.write(LED7_OFF_H, (byte)0x0F);
    	
    	TimeUnit.SECONDS.sleep(5);
    	
		device.write(LED2_ON_L, (byte)0x00);
		device.write(LED2_ON_H, (byte)0x00);
		device.write(LED2_OFF_L, (byte)0xFF);
    	device.write(LED2_OFF_H, (byte)0x0F);
    	
    	device.write(LED3_ON_L, (byte)0x00);
		device.write(LED3_ON_H, (byte)0x10);
		device.write(LED3_OFF_L, (byte)0x00);
    	device.write(LED3_OFF_H, (byte)0x00);
    	
    	device.write(LED4_ON_L, (byte)0x00);
		device.write(LED4_ON_H, (byte)0x00);
		device.write(LED4_OFF_L, (byte)0x00);
    	device.write(LED4_OFF_H, (byte)0x00);
    	
    	device.write(LED5_ON_L, (byte)0x00);
		device.write(LED5_ON_H, (byte)0x00);
		device.write(LED5_OFF_L, (byte)0x00);
    	device.write(LED5_OFF_H, (byte)0x00);
    	
    	device.write(LED6_ON_L, (byte)0x00);
		device.write(LED6_ON_H, (byte)0x10);
		device.write(LED6_OFF_L, (byte)0x00);
    	device.write(LED6_OFF_H, (byte)0x00);
    	
    	device.write(LED7_ON_L, (byte)0x00);
		device.write(LED7_ON_H, (byte)0x00);
		device.write(LED7_OFF_L, (byte)0xFF);
    	device.write(LED7_OFF_H, (byte)0x0F);
    	

    	
    	TimeUnit.SECONDS.sleep(5);
    	
    	this.reset();
		
	}
	
	void init() throws IOException, InterruptedException{
		int mode1 = device.read(MODE1);
        //console.println("MODE1 = " + String.format("0x%02x", mode1));
        
        if((byte)mode1==0x11){
        	//console.println("USTAWIANIE MODÓW");
	        int mode2 = device.read(MODE2);
	        //console.println("MODE2 = " + String.format("0x%02x", mode2));
	        
	        device.write(MODE2,(byte)(mode2 | 0x08));
	        //Thread.sleep(500);
	        
	        mode2 = device.read(MODE2);
	        //console.println("MODE2 = " + String.format("0x%02x", mode2));
	        
	        device.write(MODE1,(byte)(mode1 & ~0x10));
	        //Thread.sleep(500);
	        
	        mode1 = device.read(MODE1);
	        //console.println("MODE1 = " + String.format("0x%02x", mode1));
        }
	}
	
	void reset() throws InterruptedException, IOException{
        // send broadcasting RESET command
        broadcast.write((byte)0x06);
        
        // wait from reset
        Thread.sleep(500);
	}
	
//	// value: 0-100%
//	void writePWM(int ledAddr, int precent) throws IOException{
//		int value = (int) precent * 4096 / 100;
//		byte lsb = (byte) (value & 0xFF);
//		byte msb = (byte) ((value >> 8) & 0xFF);
//		System.out.println(value);
//		System.out.println("lsb = " + String.format("0x%02x", lsb));
//		System.out.println("msb = " + String.format("0x%02x", msb));
//		
//		switch(ledAddr){
//		case 2:	device.write(LED2_ON_L, lsb);
//				device.write(LED2_ON_H, msb);
//				device.write(LED2_OFF_L, (byte)0x00);
//	        	device.write(LED2_OFF_H, (byte)0x00);
//		case 3:	device.write(LED3_ON_L, lsb);
//				device.write(LED3_ON_H, msb);
//				device.write(LED3_OFF_L, (byte)0x00);
//	        	device.write(LED3_OFF_H, (byte)0x00);
//		case 4:	device.write(LED4_ON_L, lsb);
//				device.write(LED4_ON_H, msb);
//				device.write(LED4_OFF_L, (byte)0x00);
//	        	device.write(LED4_OFF_H, (byte)0x00);
//		case 5:	device.write(LED5_ON_L, lsb);
//				device.write(LED5_ON_H, msb);
//				device.write(LED5_OFF_L, (byte)0x00);
//		    	device.write(LED5_OFF_H, (byte)0x00);
//		case 6:	device.write(LED6_ON_L, lsb);
//				device.write(LED6_ON_H, msb);
//				device.write(LED6_OFF_L, (byte)0x00);
//		    	device.write(LED6_OFF_H, (byte)0x00);
//		case 7:	device.write(LED7_ON_L, lsb);
//				device.write(LED7_ON_H, msb);
//				device.write(LED7_OFF_L, (byte)0x00);
//		    	device.write(LED7_OFF_H, (byte)0x00);
//		}
//	}
	
	

}
