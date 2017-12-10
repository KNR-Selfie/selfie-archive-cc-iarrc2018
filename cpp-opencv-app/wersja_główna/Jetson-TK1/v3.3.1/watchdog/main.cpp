#include <thread>
#include <stdio.h>

#include "include/watchdog_class.cpp"

Watchdog watchdog;

void wait_for_connection(bool &end_of_app)
{
    unsigned short flag_od = 0;

    while(flag_od == 0)
    {
        flag_od = watchdog.pull_flag();

        std::cout << "\033[2J\033[1;1H";
        std::cout << "Waiting..." << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    if(flag_od == 1)
    {
        std::cout << "Connection regained!" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    else
    {
        end_of_app = true;
    }
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

                system("pkill -9 -f opencv-app");

                system("gnome-terminal --geometry 20x10+0+0 -x sh -c \"sudo /home/mateusz/Desktop/opencv-app/opencv-app/build/opencv-app; bash\"");
                wait_for_connection(end_of_app);
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

            std::cout << "Main app closed without error, closing app" << std::endl;
            end_of_app = 1;
            break;

        default:
            std::cout << "Error: press enter" << std::endl;
            end_of_app = 1;
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

    watchdog.push_flag(0);

    //Start new terminal with opencv-app
    system("gnome-terminal -x sh -c \"/home/mateusz/Desktop/opencv-app/opencv-app/build/opencv-app; bash\"");

    std::this_thread::sleep_for(std::chrono::seconds(1));

    wait_for_connection(end_of_app);

    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::cout << "\033[2J\033[1;1H";

    std::thread thread_ping_main_app(watch, std::ref(end_of_app));

    while(!end_of_app)

    thread_ping_main_app.join();

    watchdog.close();

    return 0;
}
