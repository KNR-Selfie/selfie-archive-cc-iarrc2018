package knr.selfie.motorhat;

import java.io.IOException;

import com.pi4j.io.i2c.I2CBus;
import com.pi4j.io.i2c.I2CDevice;
import com.pi4j.io.i2c.I2CFactory;
import com.pi4j.io.i2c.I2CFactory.UnsupportedBusNumberException;
import com.pi4j.platform.PlatformAlreadyAssignedException;
import com.pi4j.util.Console;

public class MotorHat {
	
	// PCA9685 device - I2C address
    public static final int PCA9685_ADDR = 0x60; 
    public static final int BROADCAST = 0x00;
    
    public static final byte MODE1 = (byte)0x00;
    public static final byte MODE2 = (byte)0x01;
    
    // PWM
    public static final byte LED2_ON_L = (byte)0x0E;
    public static final byte LED2_ON_H = (byte)0x0F;
    public static final byte LED2_OFF_L = (byte)0x10;
    public static final byte LED2_OFF_H = (byte)0x11;
    
    // IN2
    public static final byte LED3_ON_L = (byte)0x12;
    public static final byte LED3_ON_H = (byte)0x13;
    public static final byte LED3_OFF_L = (byte)0x14;
    public static final byte LED3_OFF_H = (byte)0x15;
    
    // IN3
    public static final byte LED4_ON_L = (byte)0x16;
    public static final byte LED4_ON_H = (byte)0x17;
    public static final byte LED4_OFF_L = (byte)0x18;
    public static final byte LED4_OFF_H = (byte)0x19;

    /**
     * Program Main Entry Point
     *
     * @param args
     * @throws InterruptedException
     * @throws PlatformAlreadyAssignedException
     * @throws IOException
     * @throws UnsupportedBusNumberException
     */
    public static void main(String[] args) throws InterruptedException, PlatformAlreadyAssignedException, IOException, UnsupportedBusNumberException {

        // create Pi4J console wrapper/helper
        // (This is a utility class to abstract some of the boilerplate code)
        final Console console = new Console();

        // print program title/header
        console.title("<-- The Pi4J Project -->", "I2C Example");

        // allow for user to exit program using CTRL-C
        console.promptForExit();
        
       
        // get the I2C bus to communicate on
        I2CBus i2c = I2CFactory.getInstance(I2CBus.BUS_1);

        I2CDevice device = i2c.getDevice(PCA9685_ADDR);
        
        I2CDevice broadcast = i2c.getDevice(BROADCAST);
        
        broadcast.write((byte)0x06);
        Thread.sleep(5000);
        
        int mode1 = device.read(MODE1);
        console.println("MODE1 = " + String.format("0x%02x", mode1));
        
        if((byte)mode1==0x11){
        	console.println("USTAWIANIE MODÓW");
	        int mode2 = device.read(MODE2);
	        console.println("MODE2 = " + String.format("0x%02x", mode2));
	        
	        device.write(MODE2,(byte)(mode2 | 0x08));
	        Thread.sleep(500);
	        
	        mode2 = device.read(MODE2);
	        console.println("MODE2 = " + String.format("0x%02x", mode2));
	        
	        device.write(MODE1,(byte)(mode1 & ~0x10));
	        Thread.sleep(500);
	        
	        mode1 = device.read(MODE1);
	        console.println("MODE1 = " + String.format("0x%02x", mode1));
        }
        console.println("USTAWIANIE LEDÓW");
        	//*****************LED2
        	device.write(LED2_ON_L, (byte)0xFF);
        	device.write(LED2_ON_H, (byte)0x10);
        	Thread.sleep(500);
        	
        	device.write(LED2_OFF_L, (byte)0x00);
        	device.write(LED2_OFF_H, (byte)0x00);
        	Thread.sleep(500);
        	
        	mode1 = device.read(LED2_ON_L);
            console.println("LED2_ON_L = " + String.format("0x%02x", mode1));
            mode1 = device.read(LED2_ON_H);
            console.println("LED2_ON_H = " + String.format("0x%02x", mode1));
            
            mode1 = device.read(LED2_OFF_L);
            console.println("LED2_OFF_L = " + String.format("0x%02x", mode1));
            mode1 = device.read(LED2_OFF_H);
            console.println("LED2_OFF_H = " + String.format("0x%02x", mode1));
            //************************LED3
            device.write(LED3_ON_L, (byte)0xFF);
        	device.write(LED3_ON_H, (byte)0x10);
        	Thread.sleep(500);
        	
        	device.write(LED3_OFF_L, (byte)0x00);
        	device.write(LED3_OFF_H, (byte)0x00);
        	Thread.sleep(500);
        	
        	mode1 = device.read(LED3_ON_L);
            console.println("LED3_ON_L = " + String.format("0x%02x", mode1));
            mode1 = device.read(LED3_ON_H);
            console.println("LED3_ON_H = " + String.format("0x%02x", mode1));
            
            mode1 = device.read(LED3_OFF_L);
            console.println("LED3_OFF_L = " + String.format("0x%02x", mode1));
            mode1 = device.read(LED3_OFF_H);
            console.println("LED3_OFF_H = " + String.format("0x%02x", mode1));
          //************************LED4
            device.write(LED4_ON_L, (byte)0xFF);
        	device.write(LED4_ON_H, (byte)0x10);
        	Thread.sleep(500);
        	
        	device.write(LED4_OFF_L, (byte)0x00);
        	device.write(LED4_OFF_H, (byte)0x00);
        	Thread.sleep(500);
        	
        	mode1 = device.read(LED4_ON_L);
            console.println("LED4_ON_L = " + String.format("0x%02x", mode1));
            mode1 = device.read(LED4_ON_H);
            console.println("LED4_ON_H = " + String.format("0x%02x", mode1));
            
            mode1 = device.read(LED4_OFF_L);
            console.println("LED4_OFF_L = " + String.format("0x%02x", mode1));
            mode1 = device.read(LED4_OFF_H);
            console.println("LED4_OFF_H = " + String.format("0x%02x", mode1));
            
            Thread.sleep(2000);
            
            i2c.close();
        	
        //}else{
        //	System.out.println("dupa");
        //}
        
        

  
    }
}
