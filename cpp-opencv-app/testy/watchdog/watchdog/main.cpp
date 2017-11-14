//APP
#include <thread>
#include <stdio.h>

#include "../include/watchdog_class.cpp"

Watchdog watchdog;

void wait_for_connection()
{
    unsigned short flag_od = 0;

    while(!flag_od)
    {
        flag_od = watchdog.pull_flag();

        std::cout << "\033[2J\033[1;1H";
        std::cout << "Waiting..." << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    std::cout << "Connection regained!" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
}


void watch(bool &end_of_app)
{
    const unsigned short flag = 0;
    unsigned short flag_od = 0;
    unsigned int counter = 0;

    while(!end_of_app)
    {
        std::cout << "Setting new flag value: " << flag << std::endl;

        watchdog.push_flag(flag);

        std::cout << "New flag value written!" << std::endl;
        std::cout << "Press enter to exit" << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(2));

        std::cout << "\033[2J\033[1;1H";

        flag_od = watchdog.pull_flag();

        std::cout << "Counter value: " << counter << std::endl;

        std::cout << "Flag value: " << flag_od << std::endl;

        switch (flag_od)
        {
        case 0:

            counter++;
            if(counter >= 2)
            {
                std::cout << "Warning: second packet lost!" << std::endl;
                std::cout << "Warning: app reset!" << std::endl;

                system("pkill -9 -f app");
                system("gnome-terminal -x sh -c \"/home/mateusz/Desktop/SELFIE_watchdog/app/build/app; bash\"");
                wait_for_connection();
            }
            else
            {
                std::cout << "Warning: first packet lost!" << std::endl;
            }
            break;

        case 1:

            std::cout << "OK!" << std::endl;
            counter = 0;
            break;

        case 2:

            std::cout << "Main app closed without error, press enter to close this app" << std::endl;
            end_of_app = 1;
            break;

        default:
            std::cout << "press enter" << std::endl;
            end_of_app = 0;
            break;
        }
    }

    std::cout << "\033[2J\033[1;1H";
}

int main()
{
    bool end_of_app = 0;

    watchdog.init();

    std::cout << "\033[2J\033[1;1H";

    std::this_thread::sleep_for(std::chrono::seconds(1));

    wait_for_connection();

    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::cout << "\033[2J\033[1;1H";

    std::thread thread_ping_main_app(watch, std::ref(end_of_app));

    getchar();

    end_of_app = 1;

    thread_ping_main_app.join();

    watchdog.close();

    return 0;
}
