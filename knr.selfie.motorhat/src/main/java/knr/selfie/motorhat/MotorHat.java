package knr.selfie.motorhat;

import java.io.IOException;

import com.pi4j.io.i2c.I2CBus;
import com.pi4j.io.i2c.I2CDevice;
import com.pi4j.io.i2c.I2CFactory;
import com.pi4j.io.i2c.I2CFactory.UnsupportedBusNumberException;
import com.pi4j.platform.PlatformAlreadyAssignedException;
import com.pi4j.util.Console;

public class MotorHat {

    public static final int PCA9685_ADDR = 0x60; // address pin not connected (FLOATING)
    
    public static final byte MODE1 = (byte)0x00;
    public static final byte MODE2 = (byte)0x01;
    
    public static final byte LED2_ON_L = (byte)0x0E;
    public static final byte LED2_ON_H = (byte)0x0F;
    public static final byte LED2_OFF_L = (byte)0x10;
    public static final byte LED2_OFF_H = (byte)0x11;
    
    public static final byte LED3_ON_L = (byte)0x12;
    public static final byte LED3_ON_H = (byte)0x13;
    public static final byte LED3_OFF_L = (byte)0x14;
    public static final byte LED3_OFF_H = (byte)0x15;
    
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
        
        device.write(MODE1, (byte)0x01);
        
        console.println("... reading MODE1 register");
        int mode1 = device.read(MODE1);
        console.println("MODE1 = " + String.format("0x%02x", mode1));
        
        device.write(MODE2, (byte)0x04);
        
        console.println("... reading MODE2 register");
        int mode2= device.read(MODE2);
        console.println("MODE2 = " + String.format("0x%02x", mode2));
        
        Thread.sleep(500);
        
        mode1 = device.read(MODE1);
        console.println("MODE1 = " + String.format("0x%02x", mode1));
        mode1 = (byte)mode1 & ~(byte)0x10;
        device.write(MODE1, (byte)mode1);
        
        Thread.sleep(500);
        mode1 = device.read(MODE1);
        console.println("MODE1 = " + String.format("0x%02x", mode1));
        
        device.write(MODE1, (byte)((byte)(mode1) | 0x80));
        mode1 = device.read(MODE1);
        console.println("MODE1 = " + String.format("0x%02x", mode1));

 

        device.write(LED4_ON_H, (byte)0x10);
        device.write(LED4_ON_L, (byte)0xFF);
        
        device.write(LED2_ON_H, (byte)0x10);
        device.write(LED2_ON_L, (byte)0xFF);
        
        device.write(LED3_OFF_H, (byte)0x10);
        device.write(LED3_OFF_L, (byte)0xFF);
        
        console.println("bxbtbxt");
        
        Thread.sleep(5000);

  
    }
}
