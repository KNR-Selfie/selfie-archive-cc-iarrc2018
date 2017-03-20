package knr.selfie.robot;

import org.opencv.core.Core;

import com.pi4j.util.Console;

public class Robot {

	public static void main(String[] args) {
		// Load the OpenCV library.
        System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
        // Print OpenCV info
        System.out.println(Core.NATIVE_LIBRARY_NAME.toString());
		System.out.println("OpenCV works fine!");
		// create Pi4J console wrapper/helper
        final Console console = new Console();

        // print program title/header
        console.title("<-- Selfie Robot -->");

        // allow for user to exit program using CTRL-C
        console.promptForExit();
        System.out.println("Pi4J works fine!");
	}

}
