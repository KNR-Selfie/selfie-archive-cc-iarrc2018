package knr.selfie.robot;

import java.io.IOException;

import com.pi4j.io.i2c.I2CBus;
import com.pi4j.io.i2c.I2CDevice;
import com.pi4j.io.i2c.I2CFactory;
import com.pi4j.io.i2c.I2CFactory.UnsupportedBusNumberException;

public class I2CController {
    public static final int PWMA = 2;
    public static final int IN_A1 = 3;
    public static final int IN_A2 = 4;
    public static final int IN_B2 = 5;
    public static final int IN_B1 = 6;
    public static final int PWMB = 7;
	
	private static final int BROADCAST_ADDR = 0x00;
	
    private static final byte MODE1 = (byte)0x00;
    private static final byte MODE2 = (byte)0x01;
    
    private static final byte LED0_ON_L = (byte)0x06;
    private static final byte LED0_ON_H = (byte)0x07;
    private static final byte LED0_OFF_L = (byte)0x08;
    private static final byte LED0_OFF_H = (byte)0x09;
    

    

	
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
			
		
//		this.analogWrite(2, 4096);
//    	
//    	this.digitalWrite(3, true);
//    	
//    	this.digitalWrite(4, false);
//    	
//    	this.digitalWrite(5, false);
//    	
//    	this.digitalWrite(6, true);
//    	
//    	this.analogWrite(7, 2000);
//    	
//    	TimeUnit.SECONDS.sleep(2);
//    	
//		device.write(LED2_ON_L, (byte)0x00);
//		device.write(LED2_ON_H, (byte)0x00);
//		device.write(LED2_OFF_L, (byte)0xFF);
//    	device.write(LED2_OFF_H, (byte)0x0F);
//    	
//    	this.digitalWrite(3, false);
//    	
//    	this.digitalWrite(4, true);
//    	
//    	this.digitalWrite(5, true);
//    	
//    	this.digitalWrite(6, false);
//    	
//    	device.write(LED7_ON_L, (byte)0x00);
//		device.write(LED7_ON_H, (byte)0x00);
//		device.write(LED7_OFF_L, (byte)0xFF);
//    	device.write(LED7_OFF_H, (byte)0x0F);
//    	
//    	TimeUnit.SECONDS.sleep(2);
//    	
//		device.write(LED2_ON_L, (byte)0x00);
//		device.write(LED2_ON_H, (byte)0x00);
//		device.write(LED2_OFF_L, (byte)0xFF);
//    	device.write(LED2_OFF_H, (byte)0x0F);
//    	
//    	this.digitalWrite(3, true);
//    	
//    	this.digitalWrite(4, false);
//    	
//    	this.digitalWrite(5, false);
//    	
//    	this.digitalWrite(6, true);
//    	
//    	device.write(LED7_ON_L, (byte)0x00);
//		device.write(LED7_ON_H, (byte)0x00);
//		device.write(LED7_OFF_L, (byte)0xFF);
//    	device.write(LED7_OFF_H, (byte)0x0F);
//    	
//    	TimeUnit.SECONDS.sleep(2);
//    	
//    	this.digitalWrite(3, false);
//    	
//    	this.digitalWrite(4, false);
//    	
//    	this.digitalWrite(5, false);
//    	
//    	this.digitalWrite(6, false);
//    	
//    	TimeUnit.SECONDS.sleep(1);
    	
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
	
	void digitalWrite(int ledAddr, boolean state){
		try {
			device.write(LED0_ON_L + 4*ledAddr, (byte)0x00);
			device.write(LED0_OFF_L + 4*ledAddr, (byte)0x00);
	    	device.write(LED0_OFF_H + 4*ledAddr, (byte)0x00);
	    	if(state){
	    		device.write(LED0_ON_H + 4*ledAddr, (byte)0x10);
	    	}else{
	    		device.write(LED0_ON_H + 4*ledAddr, (byte)0x00);
	    	}
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

	}
	
	void analogWrite(int ledAddr, int value){
		try {
			if(value>=4096){
				
					device.write(LED0_ON_L + 4*ledAddr, (byte)0x00);
			
				device.write(LED0_ON_H + 4*ledAddr, (byte)0x10);
				device.write(LED0_OFF_L + 4*ledAddr, (byte)0x00);
		    	device.write(LED0_OFF_H + 4*ledAddr, (byte)0x00);
			}else if(value<0){
				device.write(LED0_ON_L + 4*ledAddr, (byte)0x00);
				device.write(LED0_ON_H + 4*ledAddr, (byte)0x00);
				device.write(LED0_OFF_L + 4*ledAddr, (byte)0x00);
		    	device.write(LED0_OFF_H + 4*ledAddr, (byte)0x00);
			}else{
				byte lsb = (byte) (value & 0xFF);
				byte msb = (byte) ((value >> 8) & 0xFF);
				device.write(LED0_ON_L + 4*ledAddr, (byte)0x00);
				device.write(LED0_ON_H + 4*ledAddr, (byte)0x00);
				device.write(LED0_OFF_L + 4*ledAddr, lsb);
		    	device.write(LED0_OFF_H + 4*ledAddr, msb);
			}
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

}
