package sample;

import javax.microedition.io.Connector;
import javax.microedition.io.StreamConnection;
import java.io.*;
import java.util.Random;

/**
 * Connects to HC-05 bluetooth module
 */
public class HC05{
    private static OutputStream os;
    private static StreamConnection streamConnection;
    private static InputStream is;
    private static int i=0;
    private int data[] = new int[1000];
    private int a =-1;
    private static int x = 1;

    //set your hc05Url

    //Bt Piotrek
    //private static String hc05Url = "btspp://201611226383:1;authenticate=false;encrypt=false;master=false";
    //bt Jacek
    private static String hc05Url = "btspp://301412260760:1;authenticate=false;encrypt=false;master=false";

    public int getData(int i){
        return this.data[i];
    }

    public void go() throws IOException {
        streamConnection = (StreamConnection) Connector.open(hc05Url);
        os = streamConnection.openOutputStream();
        is = streamConnection.openInputStream();

        System.out.println("Connected to " + hc05Url);
    }

    public void send(int a) throws Exception {
        os.write(a);
    }

    public static void close() throws IOException {
        x = 2;
        os.close();
        is.close();
        streamConnection.close();
    }

    public void getValueOfDetector() throws IOException {


        //zczytywanie danych z czujnika, zamiana na int
        BufferedReader bReader = new BufferedReader(new InputStreamReader(is));
        String read = bReader.readLine();
        int reade = Integer.valueOf(read);
        System.out.println(reade);

        //zapisanie wartosci w tablicy
        data[i] = reade;

        i++;
    }
    //Test
    public void drawTestData(){
        data[i]= new Random().nextInt(200);
        i++;
    }
}
