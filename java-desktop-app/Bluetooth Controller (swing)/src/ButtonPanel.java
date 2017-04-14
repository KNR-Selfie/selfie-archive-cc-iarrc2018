import javax.swing.*;
import javax.swing.plaf.basic.BasicArrowButton;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;


/**
 * Created by Jacek on 23.03.2017.
 */
public class ButtonPanel extends JPanel implements ActionListener {
    public static final int HEIGHT = 100;
    public static final int WIDTH = 300;
    private BasicArrowButton up;
    private BasicArrowButton down;
    private BasicArrowButton left;
    private BasicArrowButton right;
    private JButton stop;


    public ButtonPanel(){
        up = new BasicArrowButton(BasicArrowButton.NORTH);
        down = new BasicArrowButton(BasicArrowButton.SOUTH);
        left = new BasicArrowButton(BasicArrowButton.WEST);
        right = new BasicArrowButton(BasicArrowButton.EAST);
        stop = new JButton("stop");

        up.addActionListener(this);
        down.addActionListener(this);
        left.addActionListener(this);
        right.addActionListener(this);
        stop.addActionListener(this);

        setPreferredSize(new Dimension(WIDTH, HEIGHT));
        add(left);
        add(up);
        add(right);
        add(down);
        add(stop);

    }

    @Override
    public void actionPerformed(ActionEvent e) {
        Object source = e.getSource();

        if (source == up) {
            try {
                HC05.send(251);
            } catch (Exception e1) {
                e1.printStackTrace();
            }
        }
        else if(source == down){
            try {
                HC05.send(252);
            } catch (Exception e1) {
                e1.printStackTrace();
            }
        }
        else if (source == stop){
            try {
                HC05.send(250);
            } catch (Exception e1) {
                e1.printStackTrace();
            }
        }
        else if (source == left){
            try {
                HC05.send(253);
            } catch (Exception e1) {
                e1.printStackTrace();
            }
        }
        else if (source == right){
            try {
                HC05.send(254);
            } catch (Exception e1) {
                e1.printStackTrace();
            }
        }
    }
}
