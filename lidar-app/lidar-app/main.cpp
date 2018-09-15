#include <iostream>

#include <include/urg.hpp>
#include <include/process.hpp>
#include <include/sharedmemory.hpp>

URG urg;
Process process;
SharedMemory shm_left(50001, 12000);
SharedMemory shm_right(50002, 12000);
SharedMemory shm_additional_data(50003, 32);

void clean_up();

int main()
{
    // USB communication with Lidar
    if(!urg.init())
    {
        std::cout << "Closing app!" << std::endl;
        clean_up();
        return -1;
    }

    // Shared Memory
    if(!shm_left.init() || !shm_right.init() || !shm_additional_data.init())
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
#ifdef SHM_TEST
    cv::Mat frame_l(HEIGHT, WIDTH, CV_8UC3);
    cv::Mat frame_r(HEIGHT, WIDTH, CV_8UC3);
#endif

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

        // Preprocess data
        urg.polar_to_cartesian();
        urg.filter_distance(urg.raw_data, urg.filtered_data[0]);
        urg.filter_Y(urg.filtered_data[0], urg.filtered_data[1]);

        // Send data to Shared Memory
        shm_left.push_point_data(urg.filtered_data[1].pos);
        shm_right.push_point_data(urg.filtered_data[1].pos);
        shm_additional_data.push_additional_data(cv::Point(WIDTH/2, HEIGHT/2), urg.scale, process.gap_pos_left, process.gap_pos_right);

        // Draw lidar data
        urg.draw_data_raw(frame_raw);
        urg.draw_boundaries(frame_data, urg.filtered_data[1]);
        urg.draw_data_filtered(frame_data, urg.filtered_data[1]);

#ifdef SHM_TEST
        // Read data from Shared Memory
        shm_left.pull_points_data(frame_l, cv::Scalar(0,255,200));
        shm_right.pull_points_data(frame_r, cv::Scalar(200,0,200));
        shm_additional_data.pull_additional_data();
#endif
        // Update windows
        cv::imshow("Data", frame_data);
        cv::imshow("Settings", frame_settings);
#ifdef SHM_TEST
        cv::imshow("SHM_L", frame_l);
        cv::imshow("SHM_R", frame_r);
#endif

#ifdef VERBOSE_MODE
        cv::imshow("Raw", frame_raw);
#endif

        // Get input from user
        keypressed = (char)cv::waitKey(FRAME_TIME);

        // Process given input
        if( keypressed == 27 )
            break;

        // Clear cv::Mat's for the next frame
#ifdef DEBUG_MODE
        frame_data = cv::Mat::zeros(HEIGHT, WIDTH, CV_8UC3);
        frame_data = cv::Mat::zeros(HEIGHT, WIDTH, CV_8UC3);
#ifdef SHM_TEST
        frame_l = cv::Mat::zeros(HEIGHT, WIDTH, CV_8UC3);
        frame_r = cv::Mat::zeros(HEIGHT, WIDTH, CV_8UC3);
#endif
#endif
    }

    clean_up();
    return 0;
}

void clean_up()
{
    urg.close();
    shm_left.close();
    shm_right.close();
    shm_additional_data.close();
}
