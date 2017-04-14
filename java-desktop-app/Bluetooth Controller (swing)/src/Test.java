import java.awt.*;
import java.io.IOException;

/**
 * Created by Jacek on 23.03.2017.
 */
public class Test {

    public static void main(String[] args) {
        EventQueue.invokeLater(new Runnable() {
            @Override
            public void run() {
                new Frame();

                try {
                    new HC05().go();
                } catch (IOException e) {
                    e.printStackTrace();
                }

            }
        });
    }
}
