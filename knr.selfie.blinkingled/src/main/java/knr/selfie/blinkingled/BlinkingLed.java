package knr.selfie.blinkingled;

import com.pi4j.io.gpio.GpioController;
import com.pi4j.io.gpio.GpioFactory;
import com.pi4j.io.gpio.GpioPinDigitalOutput;
import com.pi4j.io.gpio.RaspiPin;
 
public class BlinkingLed 
{
    public static void main( String[] args ) throws InterruptedException
    {
        GpioController gpio = GpioFactory.getInstance();
        GpioPinDigitalOutput pin0 = gpio.provisionDigitalOutputPin(RaspiPin.GPIO_00);
        boolean ledOn = true;
        while(true) {
             pin0.setState(ledOn);
             ledOn = !ledOn;
             Thread.sleep(1000);
        }
    }
}