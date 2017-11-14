//APP
#include <thread>
#include <stdio.h>

#include "../include/watchdog_class.cpp"

Watchdog watchdog;

void send(bool &stop_sending)
{
    while(!stop_sending)
    {
        std::cout << "Setting new flag value: 1" << std::endl;
        std::cout << "Press enter to simulate error" << std::endl;

        //Tell watchdog: i'm working
        watchdog.push_flag(1);

        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    std::cout << "\033[2J\033[1;1H";
}

int main()
{
    bool stop_sending = 0;
    char tmp = 'e';

    watchdog.get_access();

    GOTO:

    stop_sending = 0;
    tmp = 'n';

    std::cout << "\033[2J\033[1;1H" << "TUTAJ!" << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::thread thread_send_signal(send, std::ref(stop_sending));

    getchar();

    stop_sending = 1;

    thread_send_signal.join();

    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::cout << "Key in 'e' to exit: ";
    std::cin >> tmp;

    if(tmp == 'e')
    {
        //Tell watchdog to stop
        watchdog.push_flag(2);
    }
    else
    {
        goto GOTO;
    }

    return 0;
}
