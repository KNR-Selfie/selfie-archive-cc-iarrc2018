#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>

#include <include/def.hpp>
#include <include/ids.h>
#include <include/kurokesu.hpp>
#include <include/lanedetector.hpp>
#include <include/stoplights.h>
#include <include/sharedmemory.hpp>

// Custom handlers
Kurokesu kurokesu;
LaneDetector lanedetector;
StopLightDetector lightDetector;
SharedMemory shm_lane_points(50002, 5000);
SharedMemory shm_usb_to_send(50003, 64);
SharedMemory shm_watchdog(50004, 64);

// Frame for cv::Size info
cv::Mat frame_ref(2*CAM_RES_Y, CAM_RES_X, CV_8UC1);
cv::Mat frame_inv_ref(CAM_RES_Y, CAM_RES_X, CV_8UC1);

// Functions declarations
void update_bird_eye(int, void*);
void update_cam(int, void*);

int main()
{
#ifndef VID_MODE
#ifndef IDS_MODE
    // Camera init
    if(!kurokesu.init())
        return 0;
#else
    ids.init();
#endif
#else
    // Video read init
    cv::VideoCapture video("/home/mateusz/Videos/sample_selfie/out_1.avi");
    // Check
    if(!video.isOpened())
    {
        std::cout << "Error opening video stream or file!"
                  << "Closing app!" << std::endl;
        return 0;
    }
#endif

    // Init shm
    shm_lane_points.init();
    shm_usb_to_send.init();
    shm_watchdog.init();

    // Declarations of cv::Mat
    cv::Mat frame(CAM_RES_Y, CAM_RES_X, CV_8UC3);
    cv::Mat frame_hls(CAM_RES_Y, CAM_RES_X, CV_8UC3);
    cv::Mat frame_bird(CAM_RES_Y, CAM_RES_X, CV_8UC1);
    std::vector<cv::Mat> frame_split_vec(3);

    cv::Mat frame_h(CAM_RES_Y, CAM_RES_X, CV_8UC1);
    cv::Mat frame_l(CAM_RES_Y, CAM_RES_X, CV_8UC1);
    cv::Mat frame_s(CAM_RES_Y, CAM_RES_X, CV_8UC1);

    cv::Mat frame_binary_h(CAM_RES_Y, CAM_RES_X, CV_8UC1);
    cv::Mat frame_binary_l(CAM_RES_Y, CAM_RES_X, CV_8UC1);
    cv::Mat frame_binary_s(CAM_RES_Y, CAM_RES_X, CV_8UC1);

    cv::Mat frame_morph_h(CAM_RES_Y, CAM_RES_X, CV_8UC1);
    cv::Mat frame_morph_l(CAM_RES_Y, CAM_RES_X, CV_8UC1);
    cv::Mat frame_morph_s(CAM_RES_Y, CAM_RES_X, CV_8UC1);

    cv::Mat frame_data(CAM_RES_Y, CAM_RES_X, CV_8UC3);
    cv::Mat frame_bird_inverse(CAM_RES_Y, CAM_RES_X, CV_8UC1);
    cv::Mat frame_settings(1, 580, CV_8UC1);

#ifdef TEST_SHM
    cv::Mat frame_test_shm(CAM_RES_Y, CAM_RES_X, CV_8UC3);
#endif

// Creations of windows
#ifdef DEBUG_MODE
    cv::namedWindow("Camera", cv::WINDOW_AUTOSIZE);
    cv::moveWindow("Camera", 0, 0);
    cv::namedWindow("Bird Eye", cv::WINDOW_AUTOSIZE);
    cv::moveWindow("Bird Eye", 650, 0);

    cv::namedWindow("Binary H", cv::WINDOW_AUTOSIZE);
    cv::moveWindow("Binary H", 0, 380);
    cv::namedWindow("Binary L", cv::WINDOW_AUTOSIZE);
    cv::moveWindow("Binary L", 0, 380);
    cv::namedWindow("Binary S", cv::WINDOW_AUTOSIZE);
    cv::moveWindow("Binary S", 0, 380);

    cv::namedWindow("Data inverse", cv::WINDOW_AUTOSIZE);
    cv::moveWindow("Data inverse", 0, 0);
    cv::namedWindow("Settings", cv::WINDOW_AUTOSIZE);
    cv::moveWindow("Settings", 600, 380);

#ifndef VID_MODE
#ifndef IDS_MODE
    cv::namedWindow("CAM Settings", cv::WINDOW_AUTOSIZE);
    cv::moveWindow("CAM Settings", 650, 380);
#endif
#endif
#endif

#ifdef VERBOSE_MODE
    cv::namedWindow("H", cv::WINDOW_AUTOSIZE);
    cv::moveWindow("H", 1300, 200);
    cv::namedWindow("L", cv::WINDOW_AUTOSIZE);
    cv::moveWindow("L", 1300, 200);
    cv::namedWindow("S", cv::WINDOW_AUTOSIZE);
    cv::moveWindow("S", 1300, 300);
    cv::namedWindow("Data", cv::WINDOW_AUTOSIZE);
    cv::moveWindow("Data", 1300, 700);
#endif

#ifdef TEST_SHM
    cv::namedWindow("SHM TEST", cv::WINDOW_AUTOSIZE);
    cv::moveWindow("SHM TEST", 1300, 700);
#endif

    // Threshold variables
    int thresh_h_low = 24;
    int thresh_h_high = 28;
    int thresh_l_low = 0;
    int thresh_l_high = 190;
    int thresh_s_low = 50;
    int thresh_s_high = 255;
    int thresh_hist = 100;
    int thresh_stop = 50;

    uint32_t h_l_s = 1; // 0 -H, 1 - L, 2 - S

    // Bird Eye first calculation
    lanedetector.calculate_bird_var(frame_ref, frame_inv_ref);

    // Detection areas, count from the bottom to the top of the cv::Mat
    DetectionArea area_line[DETECTION_NUMBER];

    //Init def values
    for(int i = 0; i < DETECTION_NUMBER; i++)
    {
        area_line[i].init(cv::Point(320, 320 - 40*i));
    }

    // Detected points
    std::vector<cv::Point> left_points;
    std::vector<cv::Point> right_points;
    std::vector<cv::Point> cones_points;

    // Scene variables
    uint32_t red_light_visible= 1;
    uint32_t green_light_visible= 0;
    uint32_t reset_stop = 0;
    uint32_t reset_lane = 0;
    uint32_t light_3_pos = 0;
    uint8_t find_light = false;
    uint32_t ping = 1;

#ifdef STOPLIGHTS_MODE
    //cv::Mats used in stoplight algorythm
    cv::Mat ids_image(CAM_RES_Y,CAM_RES_X,CV_8UC3);
    cv::Mat old_frame(CAM_RES_Y,CAM_RES_X,CV_8UC1);
    cv::Mat display(CAM_RES_Y,CAM_RES_X,CV_8UC4);
    cv::Mat difference(CAM_RES_Y,CAM_RES_X,CV_8UC1);
#endif


#ifdef DEBUG_MODE
    // UI variables
    char keypressed;
#endif

#ifdef DEBUG_MODE
    // Trackbars
    kurokesu.trackbars();
    cv::createTrackbar("f", "Settings", &lanedetector.f_i, 1000, update_bird_eye);
    cv::createTrackbar("dst", "Settings", &lanedetector.dist_i, 1000, update_bird_eye);
    cv::createTrackbar("dst inv", "Settings", &lanedetector.dist_inv_i, 1000, update_bird_eye);
    cv::createTrackbar("cut y", "Settings", &lanedetector.cut_y, 100, update_bird_eye);
    cv::createTrackbar("alpha", "Settings", &lanedetector.alpha_i, 200, update_bird_eye);
    cv::createTrackbar("H low", "Settings", &thresh_h_low, 255, NULL);
    cv::createTrackbar("H high", "Settings", &thresh_h_high, 255, NULL);
    cv::createTrackbar("L low", "Settings", &thresh_l_low, 255, NULL);
    cv::createTrackbar("L high", "Settings", &thresh_l_high, 255, NULL);
    cv::createTrackbar("S low", "Settings", &thresh_s_low, 255, NULL);
    cv::createTrackbar("S high", "Settings", &thresh_s_high, 255, NULL);
    cv::createTrackbar("t hist", "Settings", &thresh_hist, 255, NULL);
    cv::createTrackbar("t stop", "Settings", &thresh_stop, 255, NULL);
#endif

#ifdef FPS_COUNT
    // FPS
    struct timespec start, end;
    unsigned int frame_count = 0;
    float seconds = 0;
    float fps = 0;
#endif

    // Pre-scan
    // Get new frame
#ifndef VID_MODE
#ifndef IDS_MODE
    for(int i = 0; i < 30; i++)
        kurokesu.camera >> frame;
#else
    for(int i = 0; i < 40; i++)
        ids.get_frame_to(frame);
#endif
#else
    video >> frame;
#endif
    // Change perspective
    //cv::GaussianBlur(frame, frame_blur, cv::Size(3,3), 0, 0);
    lanedetector.bird_eye(frame, frame_bird);

    // Obtain HLS color space image
    cv::cvtColor(frame_bird, frame_hls, cv::COLOR_BGR2HLS);

    // Split channels
    cv::split(frame_hls, frame_split_vec);
    frame_h = frame_split_vec[0];
    frame_l = frame_split_vec[1];
    frame_s = frame_split_vec[2];

    switch(h_l_s)
    {
    case 0:
    {
        // Binarize channels
        lanedetector.binarize(frame_h, frame_binary_h, thresh_h_low, thresh_h_high);

        // Opening operation
        lanedetector.opening(frame_binary_h, frame_morph_h);

        // Calculate new ROI's positions
        for(int i = 0; i < DETECTION_NUMBER; i++)
        {
            // Take new ROI based on previous frame
            area_line[i].update(frame_morph_h);

            // Detect position of the line relative to the ROI
            area_line[i].find_center(thresh_hist);

            // Set standard ROI size
            area_line[i].roi_x = ROI_X;

            // Corect position of ROI if outside cv::Mat
            area_line[i].check_boundaries();
        }

        // Pull back lost ROI's
//        lanedetector.correct_ROIs(area_line, frame_morph_h);
        break;
    }
    case 1:
    {
        // Binarize channels
        lanedetector.binarize(frame_l, frame_binary_l, thresh_l_low, thresh_l_high);

        // NOT
        cv::bitwise_not(frame_binary_l, frame_binary_l);

        // Opening operation
        lanedetector.opening(frame_binary_l, frame_morph_l);

        // Calculate new ROI's positions
        for(int i = 0; i < DETECTION_NUMBER; i++)
        {
            // Take new ROI based on previous frame
            area_line[i].update(frame_morph_l);

            // Detect position of the line relative to the ROI
            area_line[i].find_center(thresh_hist);

            // Set standard ROI size
            area_line[i].roi_x = ROI_X;

            // Corect position of ROI if outside cv::Mat
            area_line[i].check_boundaries();
        }

        // Pull back lost ROI's
//        lanedetector.correct_ROIs(area_line, frame_morph_l);
        break;

    }
    case 2:
    {
        // Binarize channels
        lanedetector.binarize(frame_s, frame_binary_s, thresh_s_low, thresh_s_high);

        // Opening operation
        lanedetector.opening(frame_binary_s, frame_morph_s);

        // Calculate new ROI's positions
        for(int i = 0; i < DETECTION_NUMBER; i++)
        {
            // Take new ROI based on previous frame
            area_line[i].update(frame_morph_s);

            // Detect position of the line relative to the ROI
            area_line[i].find_center(thresh_hist);

            // Set standard ROI size
            area_line[i].roi_x = ROI_X;

            // Corect position of ROI if outside cv::Mat
            area_line[i].check_boundaries();
        }

        // Pull back lost ROI's
//        lanedetector.correct_ROIs(area_line, frame_morph_s);
        break;
    }
    }

#ifdef DEBUG_MODE
    // Draw detection
    lanedetector.draw_data(frame_bird, area_line);

    // Inverse data to original camera perspective
    lanedetector.bird_eye_inverse(frame_bird, frame_bird_inverse);
#endif

    // Pack data for SHM
    lanedetector.pack_data(area_line, left_points);

    // Push output data to SHM
    shm_lane_points.push_lane_data(left_points, right_points, cones_points);
    shm_usb_to_send.push_scene_data(red_light_visible, green_light_visible);
    // End of first scan

     // ========================Main loop ==================================
#ifndef VID_MODE
    while(true)
#else
    for(int i = 1; i < video.get(cv::CAP_PROP_FRAME_COUNT); i++)
#endif
    {
#ifdef FPS_COUNT
        // FPS
        if(frame_count == 0)
        {
            clock_gettime(CLOCK_MONOTONIC, &start);
        }
#endif
        // Get new frame
#ifndef VID_MODE
#ifndef IDS_MODE
        kurokesu.camera >> frame;
#else
        ids.get_frame_to(frame);
#endif
#else
        video >> frame;
#endif

#ifdef STOPLIGHTS_MODE
        std::cout<<"red"<<red_light_visible<<" green"<<green_light_visible<<"Light"<<lightDetector.start_light<<std::endl;

        if(light_3_pos == 1)
        {
            static uint8_t light_var = 5;

            if (light_var){
                lightDetector.prepare_first_image(frame,old_frame,lightDetector.roi_number);
                light_var--;
            }
            else if (lightDetector.start_light == false && find_light == true){
#ifdef DEBUG_MODE
                lightDetector.test_roi(frame,display);
#endif //DEBUG_MODE
                lightDetector.find_start(frame,difference,old_frame,lightDetector.roi_number);
                if (lightDetector.start_light == true)
                {
//                  std::cout<<"START"<<std::endl;
                    red_light_visible = false;
                    green_light_visible = true;
                }
                else{
                    red_light_visible = true;
                    green_light_visible = false;
//                  std::cout<<"WAIT"<<std::endl;
                }

            }
            if (find_light ==false){
                lightDetector.prepare_first_image(frame,old_frame,lightDetector.roi_number);
                red_light_visible = true;
                green_light_visible = false;
                find_light = true;
            }
        }

        if(light_3_pos == 0)
        {
            red_light_visible = true;
            green_light_visible = false;
            lightDetector.start_light = false;
            find_light = false;
        }
        if(light_3_pos == 2)
        {
            red_light_visible = false;
            green_light_visible = true;
            lightDetector.start_light = false;
            find_light = false;
        }

#endif //STOPLIGHTS_MODE

        // Change perspective
        //cv::GaussianBlur(frame, frame_blur, cv::Size(3,3), 0, 0);
        lanedetector.bird_eye(frame, frame_bird);

        // Obtain HLS color space image
        cv::cvtColor(frame_bird, frame_hls, cv::COLOR_BGR2HLS);

        // Split channels
        cv::split(frame_hls, frame_split_vec);
        frame_h = frame_split_vec[0];
        frame_l = frame_split_vec[1];
        frame_s = frame_split_vec[2];

        switch(h_l_s)
        {
        case 0:
        {
            // Binarize channels
            lanedetector.binarize(frame_h, frame_binary_h, thresh_h_low, thresh_h_high);

            // Opening operation
            lanedetector.opening(frame_binary_h, frame_morph_h);

            // Calculate new ROI's positions
            for(int i = 0; i < DETECTION_NUMBER; i++)
            {
                // Take new ROI based on previous frame
                area_line[i].update(frame_morph_h);

                // Detect position of the line relative to the ROI
                area_line[i].find_center(thresh_hist);

                // Set standard ROI size
                area_line[i].roi_x = ROI_X;

                // Corect position of ROI if outside cv::Mat
                area_line[i].check_boundaries();
            }

            // Pull back lost ROI's
            lanedetector.correct_ROIs(area_line, frame_morph_h);
            break;
        }
        case 1:
        {
#ifdef AUTO_THRESH
            // Calculate thresh
            lanedetector.calculate_thresh(frame_l, area_line);
#else
            // Binarize channels
            lanedetector.binarize(frame_l, frame_binary_l, thresh_l_low, thresh_l_high);
#endif
            // NOT
            cv::bitwise_not(frame_binary_l, frame_binary_l);

            // Opening operation
            lanedetector.opening(frame_binary_l, frame_morph_l);

            // Calculate new ROI's positions
            for(int i = 0; i < DETECTION_NUMBER; i++)
            {
                // Take new ROI based on previous frame
                area_line[i].update(frame_morph_l);

                // Detect position of the line relative to the ROI
                area_line[i].find_center(thresh_hist);

                // Set standard ROI size
                area_line[i].roi_x = ROI_X;

                // Corect position of ROI if outside cv::Mat
                area_line[i].check_boundaries();
            }

            // Pull back lost ROI's
            lanedetector.correct_ROIs(area_line, frame_morph_l);
            break;

        }
        case 2:
        {
            // Binarize channels
            lanedetector.binarize(frame_s, frame_binary_s, thresh_s_low, thresh_s_high);

            // Opening operation
            lanedetector.opening(frame_binary_s, frame_morph_s);

            // Calculate new ROI's positions
            for(int i = 0; i < DETECTION_NUMBER; i++)
            {
                // Take new ROI based on previous frame
                area_line[i].update(frame_morph_s);

                // Detect position of the line relative to the ROI
                area_line[i].find_center(thresh_hist);

                // Set standard ROI size
                area_line[i].roi_x = ROI_X;

                // Corect position of ROI if outside cv::Mat
                area_line[i].check_boundaries();
            }

            // Pull back lost ROI's
            lanedetector.correct_ROIs(area_line, frame_morph_s);
            break;
        }
        }

#ifdef DEBUG_MODE
        // Draw detection
        lanedetector.draw_data(frame_bird, area_line);

        // Inverse data to original camera perspective
        lanedetector.bird_eye_inverse(frame_bird, frame_bird_inverse);
#endif

        // Pack data for SHM
        lanedetector.pack_data(area_line, left_points);

        // Push output data to SHM
        shm_lane_points.push_lane_data(left_points, right_points, cones_points);
        shm_usb_to_send.push_scene_data(red_light_visible, green_light_visible);

        shm_usb_to_send.pull_scene_data(reset_stop, reset_lane, light_3_pos,  ping);

#ifdef VERBOSE_MODE
        std::cout << "RESET STOP: " << reset_stop << std::endl;
        std::cout << "RESET LANE: " << reset_lane << std::endl;
        std::cout << "3 POS: " << light_3_pos << std::endl;
        std::cout << "PING: " << ping << std::endl;
#endif

        if(reset_lane)
        {
            goto RESET;
        }

#ifdef TEST_SHM
        shm_lane_points.pull_lane_data(frame_test_shm);
#endif

#ifdef DEBUG_MODE
        // Show frames
        cv::imshow("Camera", frame);
        cv::imshow("Bird Eye", frame_bird);

        cv::imshow("Binary H", frame_morph_h);
        cv::imshow("Binary L", frame_morph_l);
        cv::imshow("Binary S", frame_morph_s);

        cv::imshow("Data inverse", frame_bird_inverse);
        cv::imshow("Settings", frame_settings);
        cv::imshow("CAM Settings", frame_settings);

#ifdef VERBOSE_MODE
        cv::imshow("H", frame_h);
        cv::imshow("L", frame_l);
        cv::imshow("S", frame_s);
        cv::imshow("Data", frame_data);
        frame_data = cv::Mat::zeros(CAM_RES_Y, CAM_RES_X, CV_8UC3);
#endif

#ifdef TEST_SHM
        cv::imshow("SHM TEST", frame_test_shm);
        frame_test_shm = cv::Mat::zeros(CAM_RES_Y, CAM_RES_X, CV_8UC3);
#endif

        // Get input from user
        keypressed = (char)cv::waitKey(FRAME_TIME);

        // Process given input
        if( keypressed == 27 )
            break;

        switch(keypressed)
        {
        // Reset ROI positions
        case 'r':
        {
            RESET:
            // Reset
            for(int i = 0; i < DETECTION_NUMBER; i++)
            {
                area_line[i].reset();
            }

            // Pre-scan
            // Get new frame
#ifndef VID_MODE
#ifndef IDS_MODE
            kurokesu.camera >> frame;
#else
            ids.get_frame_to(frame);
#endif
#else
            video >> frame;
#endif
            // Change perspective
            //cv::GaussianBlur(frame, frame_blur, cv::Size(3,3), 0, 0);
            lanedetector.bird_eye(frame, frame_bird);

            // Obtain HLS color space image
            cv::cvtColor(frame_bird, frame_hls, cv::COLOR_BGR2HLS);

            // Split channels
            cv::split(frame_hls, frame_split_vec);
            frame_h = frame_split_vec[0];
            frame_l = frame_split_vec[1];
            frame_s = frame_split_vec[2];

            switch(h_l_s)
            {
            case 0:
            {
                // Binarize channels
                lanedetector.binarize(frame_h, frame_binary_h, thresh_h_low, thresh_h_high);

                // Opening operation
                lanedetector.opening(frame_binary_h, frame_morph_h);

                // Calculate new ROI's positions
                for(int i = 0; i < DETECTION_NUMBER; i++)
                {
                    // Take new ROI based on previous frame
                    area_line[i].update(frame_morph_h);

                    // Detect position of the line relative to the ROI
                    area_line[i].find_center(thresh_hist);

                    // Set standard ROI size
                    area_line[i].roi_x = ROI_X;

                    // Corect position of ROI if outside cv::Mat
                    area_line[i].check_boundaries();
                }

                // Pull back lost ROI's
        //        lanedetector.correct_ROIs(area_line, frame_morph_h);
                break;
            }
            case 1:
            {
                // Binarize channels
                lanedetector.binarize(frame_l, frame_binary_l, thresh_l_low, thresh_l_high);

                // NOT
                cv::bitwise_not(frame_binary_l, frame_binary_l);

                // Opening operation
                lanedetector.opening(frame_binary_l, frame_morph_l);

                // Calculate new ROI's positions
                for(int i = 0; i < DETECTION_NUMBER; i++)
                {
                    // Take new ROI based on previous frame
                    area_line[i].update(frame_morph_l);

                    // Detect position of the line relative to the ROI
                    area_line[i].find_center(thresh_hist);

                    // Set standard ROI size
                    area_line[i].roi_x = ROI_X;

                    // Corect position of ROI if outside cv::Mat
                    area_line[i].check_boundaries();
                }

                // Pull back lost ROI's
        //        lanedetector.correct_ROIs(area_line, frame_morph_l);
                break;
            }
            case 2:
            {
                // Binarize channels
                lanedetector.binarize(frame_s, frame_binary_s, thresh_s_low, thresh_s_high);

                // Opening operation
                lanedetector.opening(frame_binary_s, frame_morph_s);

                // Calculate new ROI's positions
                for(int i = 0; i < DETECTION_NUMBER; i++)
                {
                    // Take new ROI based on previous frame
                    area_line[i].update(frame_morph_s);

                    // Detect position of the line relative to the ROI
                    area_line[i].find_center(thresh_hist);

                    // Set standard ROI size
                    area_line[i].roi_x = ROI_X;

                    // Corect position of ROI if outside cv::Mat
                    area_line[i].check_boundaries();
                }

                // Pull back lost ROI's
        //        lanedetector.correct_ROIs(area_line, frame_morph_s);
                break;
            }
            }

#ifdef DEBUG_MODE
            // Draw detection
            lanedetector.draw_data(frame_bird, area_line);

            // Inverse data to original camera perspective
            lanedetector.bird_eye_inverse(frame_bird, frame_bird_inverse);
#endif

            // Pack data for SHM
            lanedetector.pack_data(area_line, left_points);

            // Push output data to SHM
            shm_lane_points.push_lane_data(left_points, right_points, cones_points);
            shm_usb_to_send.push_scene_data(red_light_visible, green_light_visible);
            // End of first scan

            break;
        }
        // Change side on which line is searched
        case 'c':
        {
            h_l_s++;

            if(h_l_s >=3)
            {
                h_l_s = 0;
            }
            break;
        }
        case 'u':
        {
            kurokesu.update();
            break;
        }
        case 'q':
        {
            red_light_visible = true;
            green_light_visible = false;
            lightDetector.start_light == false;
            break;
        }
        }
#endif

#ifdef FPS_COUNT
        // FPS
        if(frame_count > FPS_AMOUNT)
        {
            frame_count = 0;
            clock_gettime(CLOCK_MONOTONIC, &end);
            seconds = (end.tv_sec - start.tv_sec);
            fps  =  1 / (seconds / FPS_AMOUNT);

            std::cout <<"FPS: " << fps << std::endl;
        }
        else
        {
            frame_count++;
        }
#endif
    }

    // Clean up
#ifndef VID_MODE
    kurokesu.camera.release();
#else
    video.release();
#endif

    shm_lane_points.close();
    shm_usb_to_send.close();
    return 0;
}

void update_bird_eye(int, void*)
{
    lanedetector.calculate_bird_var(frame_ref, frame_inv_ref);
}
