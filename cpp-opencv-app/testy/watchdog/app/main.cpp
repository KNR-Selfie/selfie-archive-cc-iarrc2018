//APP
#include <thread>
#include <stdio.h>

#include "../include/watchdog_class.cpp"

Watchdog watchdog;

void send(bool &stop_sending)
{
    const bool flag = 1;

    while(!stop_sending)
    {
        std::cout << "Setting new flag value: " << flag << std::endl;
        std::cout << "Press enter to simulate error" << std::endl;

        watchdog.push_flag(flag);

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
    tmp = 'e';

    std::cout << "\033[2J\033[1;1H";

    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::thread thread_send_signal(send, std::ref(stop_sending));

    getchar();

    stop_sending = 1;

    thread_send_signal.join();

    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::cout << "Key in 'r' to start sending, or anything else to exit: ";
    std::cin >> tmp;

    if(tmp == 'r')
        goto GOTO;

    return 0;
}
