#include <iostream>

#include <include/urg.hpp>

//USB usb;
URG urg;

void clean_up();

int main()
{
    // USB communication with Lidar
    if(urg.init() != 1)
    {
        std::cout << "Closing app!" << std::endl;
        clean_up();
        return -1;
    }

#ifdef DEBUG_MODE
    // OpenCV Mat's
    cv::Mat frame_raw(HEIGHT, WIDTH, CV_8UC3);
    cv::Mat frame_data(HEIGHT, WIDTH, CV_8UC3);
    cv::Mat frame_settings(1, 580, CV_8UC1);

    // OpenCV UI
    char keypressed;

    // OpenCV windows
    cv::namedWindow("Data", cv::WINDOW_AUTOSIZE);
    cv::moveWindow("Data", 0, 0);
    cv::namedWindow("Settings", cv::WINDOW_AUTOSIZE);
    cv::moveWindow("Settings", 1500, 0);
#ifdef VERBOSE_MODE
    cv::namedWindow("Raw", cv::WINDOW_AUTOSIZE);
    cv::moveWindow("Raw", 100, 0);
#endif

    // OpenCV trackbars
    cv::createTrackbar("Scale", "Settings", &urg.scale, 10, NULL);
    cv::createTrackbar("C left", "Settings", &urg.cut_left, 270, NULL);
    cv::createTrackbar("C right", "Settings", &urg.cut_right, 450, NULL);
    cv::createTrackbar("Closest", "Settings", &urg.closest, 1000, NULL);
    cv::createTrackbar("Farthest", "Settings", &urg.farthest, 5000, NULL);
    cv::createTrackbar("X left", "Settings", &urg.x_left, WIDTH/2, NULL);
    cv::createTrackbar("X right", "Settings", &urg.x_right, WIDTH, NULL);
    cv::createTrackbar("Y up", "Settings", &urg.y_up, HEIGHT/2, NULL);
    cv::createTrackbar("Y down", "Settings", &urg.y_down, HEIGHT, NULL);
    cv::createTrackbar("Thresh", "Settings", &urg.thresh, 100, NULL);
#endif

    // Main loop
    while(true)
    {
        // Read new data from Lidar
        if(urg.read() < 0)
        {
            std::cout << "Closing app!" << std::endl;
            clean_up();
            return -3;
        }

        urg.polar_to_cartesian();
        urg.filter_distance(urg.raw_data, urg.filtered_data[0]);
        urg.filter_Y(urg.filtered_data[0], urg.filtered_data[1]);

#ifdef DEBUG_MODE
        frame_data = cv::Mat::zeros(HEIGHT, WIDTH, CV_8UC3);
        frame_data = cv::Mat::zeros(HEIGHT, WIDTH, CV_8UC3);
        urg.draw_data_raw(frame_raw);
        urg.draw_boundaries(frame_data, urg.filtered_data[1]);
        urg.draw_data_filtered(frame_data, urg.filtered_data[1]);

        cv::imshow("Data", frame_data);
        cv::imshow("Settings", frame_settings);
#ifdef VERBOSE_MODE
        cv::imshow("Raw", frame_raw);
#endif

        // Get input from user
        keypressed = (char)cv::waitKey(FRAME_TIME);

        // Process given input
        if( keypressed == 27 )
            break;
#endif
    }

    clean_up();
    return 0;
}

void clean_up()
{
    urg.close();
}
