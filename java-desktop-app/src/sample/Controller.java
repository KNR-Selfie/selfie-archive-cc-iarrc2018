package sample;

import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.scene.control.Slider;
import javafx.scene.input.MouseEvent;
import java.lang.String;
import java.io.*;
import javax.microedition.io.Connector;
import javax.microedition.io.StreamConnection;


public class Controller{
    @FXML

    boolean scanFinished = false;
    String hc05Url="btspp://201611226383:1;authenticate=false;encrypt=false;master=false";

    StreamConnection streamConnection;
    OutputStream outStream;
    int speed=0;
    @FXML
    Slider predkosc;
    InputStream inStream;
    long time=0;



    public void discover() throws Exception {

        streamConnection = (StreamConnection) Connector.open(hc05Url);
        System.out.println("Polączyło");
        outStream = streamConnection.openOutputStream();
        inStream=streamConnection.openInputStream();
    }

    public void up(ActionEvent event) throws IOException {
        PrintWriter pWriter=new PrintWriter(new OutputStreamWriter(outStream));
        pWriter.write(103);
        pWriter.flush();
    }

    public void down(ActionEvent event) throws IOException {
        PrintWriter pWriter=new PrintWriter(new OutputStreamWriter(outStream));
        pWriter.write(104);
        pWriter.flush();
    }

    public void left(ActionEvent event) throws IOException {
        PrintWriter pWriter=new PrintWriter(new OutputStreamWriter(outStream));
        pWriter.write(105);
        pWriter.flush();
    }

    public void right(ActionEvent event) throws IOException {
        PrintWriter pWriter=new PrintWriter(new OutputStreamWriter(outStream));
        pWriter.write(106);
        pWriter.flush();
    }

    public void speeder(MouseEvent mouseEvent) throws IOException {
        speed=(int)predkosc.getValue();
        PrintWriter pWriter=new PrintWriter(new OutputStreamWriter(outStream));
        pWriter.write(speed);
        pWriter.flush();
    }

    public void getData() throws IOException {
        for(int i=0;i<100;i++) {
            BufferedReader bReader = new BufferedReader(new InputStreamReader(inStream));
            String read = bReader.readLine();
            int reade = Integer.valueOf(read);
            System.out.println(reade);
            int tablica[]=new int[100];
            tablica[i]=reade;
        }
    }


}