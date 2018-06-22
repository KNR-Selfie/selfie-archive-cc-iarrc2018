#include "usb.hpp"

USB_STM::USB_STM()
{

    std::string name = "/dev/ttyACM0";
    strcpy(&portname[0], name.c_str());
}

int USB_STM::init(int speed)
{
    // Try openning ports from ttyACM0 to ttyACM9

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

    // set to non blocking mode
    fcntl(fd, F_SETFL, FNDELAY);

    // non-canonical mode
    //tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    //tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    //tty.c_oflag &= ~OPOST;

    // fetch bytes asap
    //tty.c_cc[VMIN] = 1;
    //tty.c_cc[VTIME] = 1;

    // Set new parameters of transmission
    if (tcsetattr(fd, TCSANOW, &tty) != 0)
    {
        std::cout << "Error while setting attributes!" << std::endl;
        return -3;
    }
    return 1;
}

void USB_STM::uint32_to_char_tab(uint32_t input, unsigned char *output)
{
    output[0] = (unsigned char) (input >> 24 &0xff);
    output[1] = (unsigned char) (input >> 16 &0xff);
    output[2] = (unsigned char) (input >> 8 &0xff);
    output[3] = (unsigned char) (input &0xff);
}

void USB_STM::char_tab_to_uint32(unsigned char input[], uint32_t *output)
{
    *output = uint32_t(input[0]<<24 | input[1]<<16 | input[2]<<8 |input [3]);
}

void USB_STM::send_buf(data_container &to_send)
{

    // Pack data
    to_send_packed[0] = to_send.start;
    to_send_packed[1] = to_send.code;
    to_send_packed[2] = to_send.length;

    for(int i=0;i<to_send.length;i++)
    to_send_packed[i+3] = to_send.data[i];

    to_send_packed[to_send.length+3] = to_send.length;
    to_send_packed[to_send.length+4] = to_send.code;
    to_send_packed[to_send.length+5] = to_send.stop;

    //send
    write(fd, &to_send_packed,22);
    //unsigned char r = 'r';
    //write(fd,&r,1);

}


void USB_STM::read_buf(int buf_size)

{
    unsigned char buf[buf_size];
    std::cout << "Len: " << read(fd, &buf, buf_size) << std::endl;

    for(int i = 0; i < buf_size; i++)
    {
        std::cout << (int)buf[i]<<"\t";

    }
   // union
   // {
    //    float f;
     //   unsigned char b[4];
   // }u;
    //u.b[0]=buf[0];
     //u.b[1]=buf[1];
      //u.b[2]=buf[2];
      // u.b[3]=buf[3];
      // std::cout<<"nasz float to "<<u.f<<std::endl;
    std::cout << std::endl;
}

void USB_STM::data_pack(uint32_t velo,uint32_t ang,std::vector<uint32_t>flags,data_container *container)
{
    unsigned char char_flags[4]; //convert uint32_flags to unsigned char
    for(int i=0;i<4;i++)
    {
        if(flags[i]>0)
            char_flags[i]=255;
    }

    unsigned char pom[4];
    uint32_to_char_tab(velo,pom);

    for(int i=0;i<4;i++)
    {
        container->data[i] = pom[i];
    }
    uint32_to_char_tab(ang,pom);

    for(int i=0;i<4;i++)
    {
        container->data[i+4] = pom[i];
    }

    container->data[8] = 0 ;//char_flags[0];
    container->data[9] = 0 ;//char_flags[1];
    container->data[10] = 0;//char_flags[2];
    container->data[11]= 0;//char_flags[3];

    uint32_to_char_tab(flags[4],pom);
    for(int i=0;i<4;i++){
        container->data[i+12] = pom[i];
    }

}
