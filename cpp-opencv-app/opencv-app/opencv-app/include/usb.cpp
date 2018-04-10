#include "usb.hpp"

USB::USB()
{
    std::string name = "/dev/ttyUSB0";
    strcpy(&portname[0], name.c_str());

    // Codes table
    to_send_left_line_visible.code = 0x64;
    to_send_right_line_visible.code = 0x65;
    to_send_horizontal_pos.code = 0x66;
    to_send_left_angle.code = 0x67;
    to_send_right_angle.code = 0x68;

    // Length table
    to_send_left_line_visible.length = 1;
    to_send_right_line_visible.length = 1;
    to_send_horizontal_pos.length = 4;
    to_send_left_angle.length = 4;
    to_send_right_angle.length = 4;
}

int USB::init(int speed)
{
    // Try openning ports from ttyUSB0 to ttyUSB9
    for(int i = 48; i < 58; i++)
    {
        portname[11] = i;
        fd = open(&portname[0], O_RDWR | O_NOCTTY | O_SYNC);

        if (fd < 0)
            std::cout << "Could not open serial communication on port: " << portname << std::endl;
        else
        {
            std::cout << "Opened serial communication on port: " << portname << std::endl;
            std::cout << "File descriptor: " << fd << std::endl;
            break;
        }
    }

    if (fd < 0)
    {
        std::cout << "Could not open any USB port" << std::endl;
        return -1;
    }

    // Get attributes of transmission
    struct termios tty;
    if (tcgetattr(fd, &tty) < 0)
    {
        std::cout << "Error while getting attributes!" << std::endl;
        return -2;
    }

    // Set input and output speed
    cfsetospeed(&tty, (speed_t)speed);
    cfsetispeed(&tty, (speed_t)speed);

    tty.c_cflag |= (CLOCAL | CREAD);    // program will not become owner of port
    tty.c_cflag &= ~CSIZE;              // bit mask for data bits
    tty.c_cflag |= CS8;                 // 8 bit data lenght
    tty.c_cflag |= PARENB;              // enable parity
    tty.c_cflag &= ~PARODD;             // even parity
    tty.c_cflag &= ~CSTOPB;             // 1 stop bit
    tty.c_cflag &= ~CRTSCTS;            // no hardware flowcontrol

    // non-canonical mode
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tty.c_oflag &= ~OPOST;

    // fetch bytes asap
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 1;

    // set to non blocking mode
    //fcntl(fd, F_SETFL, FNDELAY);
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);

    // Set new parameters of transmission
    if (tcsetattr(fd, TCSANOW, &tty) != 0)
    {
        std::cout << "Error while setting attributes!" << std::endl;
        return -3;
    }
    return 1;
}

bool USB::send_one_chunk(data_container &to_send)
{
    int check = 0;

    check += write(fd, &to_send.start, 1);
    check += write(fd, &to_send.code, 1);
    check += write(fd, &to_send.length, 1);

    for(int i = 0; i < to_send.length; i++)
        check += write(fd, &to_send.data[i], 1);

    check += write(fd, &to_send.length, 1);
    check += write(fd, &to_send.code, 1);
    check += write(fd, &to_send.stop, 1);

    if(check != 6 + to_send.length)
        return 0;
    else
        return 1;
}

bool USB::read_one_chunk()
{
    int check = 0;

    check += read(fd, &incoming_data.start, 1);
    check += read(fd, &incoming_data.code, 1);
    check += read(fd, &incoming_data.length, 1);

    for(int i = 0; i < incoming_data.length; i++)
        check += read(fd, &incoming_data.data[i], 1);

    check += read(fd, &incoming_data.length, 1);
    check += read(fd, &incoming_data.code, 1);
    check += read(fd, &incoming_data.stop, 1);

    if(check != 6 + incoming_data.length)
        return 0;
    else
        return 1;
}
