import javax.microedition.io.Connector;
import javax.microedition.io.StreamConnection;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

/**
 * Created by Jacek on 23.03.2017.
 */

public class HC05 {

    static OutputStream os;
    static StreamConnection streamConnection;
    static InputStream is;

    //set your hc05Url
    static String hc05Url = "btspp://301412260760:1;authenticate=false;encrypt=false;master=false";


    public void go() throws IOException {
        streamConnection = (StreamConnection) Connector.open(hc05Url);
        os = streamConnection.openOutputStream();
        is = streamConnection.openInputStream();

        System.out.println("Connected to " + hc05Url);
    }
    public static void send(int a) throws Exception{
        os.write(a);
    }

    public static void close() throws IOException {
        os.close();
        is.close();
        streamConnection.close();
    }
}