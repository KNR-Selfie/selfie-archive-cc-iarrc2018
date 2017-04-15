import javax.swing.*;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import java.awt.*;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.io.IOException;

/**
 * Created by Jacek on 23.03.2017.
 */
public class Frame extends JFrame implements ChangeListener{
    private JLabel label;
    private JSlider slider;
    static int speed;
    String string;

    public Frame() {
        super("Arduino Controller");
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setSize(400,300);
        setLocation(500,250);
        setLayout(new FlowLayout());
        JPanel buttonPanel = new ButtonPanel();
        label = new JLabel("speed:");
        add(label);

        slider = new JSlider(0, 200, 100);
        slider.addChangeListener(this);
        slider.setMajorTickSpacing(50);
        slider.setPaintTicks(true);
        slider.setPaintLabels(true);
        add(slider);

        add(buttonPanel);
        setVisible(true);
        setDefaultCloseOperation(Frame.EXIT_ON_CLOSE);
        addWindowListener(new WindowAdapter() {
            @Override
            public void windowClosing(WindowEvent e) {
                try {
                    HC05.close();
                } catch (IOException e1) {
                    e1.printStackTrace();
                }
            }
        });
    }

    @Override
    public void stateChanged(ChangeEvent e) {
        speed = slider.getValue();
        try {
            HC05.send(speed);
        } catch (Exception e1) {
            e1.printStackTrace();
        }
    }
}
