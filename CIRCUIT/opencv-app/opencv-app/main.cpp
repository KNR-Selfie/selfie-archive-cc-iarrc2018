#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>

#include <include/sharedmemory.hpp>
#include <include/lidar.hpp>
#include <include/lanedetector.hpp>
#include <include/stoplights.h>
#include <include/ids.h>

//#define TIMING

#ifdef TIMING
#define INIT_TIMER auto start1 = std::chrono::high_resolution_clock::now();
#define START_TIMER  start1 = std::chrono::high_resolution_clock::now();
#define STOP_TIMER(name)  std::cout << "RUNTIME of " << name << ": " << \
    std::chrono::duration_cast<std::chrono::microseconds>( \
            std::chrono::high_resolution_clock::now()-start1 \
    ).count() << " us " << std::endl;
#else
#define INIT_TIMER
#define START_TIMER
#define STOP_TIMER(name)
#endif

#define INIT_TIMER2 auto start2 = std::chrono::high_resolution_clock::now();
#define START_TIMER2  start2 = std::chrono::high_resolution_clock::now();
#define TIMER2_DIFF ((double)(std::chrono::duration_cast<std::chrono::microseconds>( \
    std::chrono::high_resolution_clock::now()-start2).count()))

// Race mode ---> fps boost
// Debug mode --> display data
//#define RACE_MODE
#define IMSHOW_RATE 1
//#define DEBUG_MODE
#define PREVIEW_MODE
#define NO_USB
#define STOPLIGHTS_MODE
#define IDS_MODE


#define CAMERA_INDEX 0

#define CAM_RES_X IDS_WIDTH
#define CAM_RES_Y IDS_HEIGHT

#define FRAME_TIME 10

// Handlers for custom classes
LaneDetector laneDetector;
StopLightDetector lightDetector;
Lidar lidar;
SharedMemory shm_lidar_data(50001);
SharedMemory shm_lane_points(50002);
SharedMemory shm_usb_to_send(50003);
SharedMemory shm_watchdog(50004);
SharedMemory shm_dataready(50005,4);

cv::Mat frame_ref(CAM_RES_Y, CAM_RES_X, CV_8UC1);
void update_trackbar(int, void*)
{
    laneDetector.calculate_bird_var(frame_ref);
}
// Variables for communication between threads
bool close_app = false;
std::mutex mu;

cv :: Mat rect;

int main()
{
//    sounds_init();
    laneDetector.CreateTrackbars();
    INIT_TIMER
    INIT_TIMER2

    struct timespec start, end;
    unsigned int licznik_czas = 0;
    float seconds = 0;
    float fps = 0;

    // Declaration of cv::MAT variables
    cv::Mat frame(CAM_RES_Y, CAM_RES_X, CV_8UC4);
    cv::Mat frame_ids(CAM_RES_Y,CAM_RES_X,CV_8UC3);
    cv::Mat yellow_bird_eye_frame(CAM_RES_Y, CAM_RES_X, CV_8UC3);
    cv::Mat white_bird_eye_frame(CAM_RES_Y, CAM_RES_X, CV_8UC3);
    cv::Mat yellow_bird_eye_frame_tr(CAM_RES_Y, CAM_RES_X, CV_8UC3);
    cv::Mat white_bird_eye_frame_tr(CAM_RES_Y, CAM_RES_X, CV_8UC3);
    cv::Mat yellow_vector_frame(CAM_RES_Y, CAM_RES_X, CV_8UC3);
    cv::Mat white_vector_frame(CAM_RES_Y, CAM_RES_X, CV_8UC3);
    cv::Mat undist_frame, cameraMatrix, distCoeffs;
    cv::Mat hsv_frame;
    cv::Mat HSV_frame;
    cv::Mat cone_frame_out;
    cv::Mat frame_out_yellow, frame_out_white, frame_out_edge_yellow, frame_out_edge_white;
    cv::Mat test_lane(CAM_RES_Y, CAM_RES_X, CV_8UC3);
    cv::Mat test_scene(CAM_RES_Y, CAM_RES_X, CV_8UC3);
    cv::Mat ids_image(CAM_RES_Y,CAM_RES_X,CV_8UC3);

    // Scene variables
    bool reset_stm = false;
    bool red_light_visible = false;
    bool green_light_visible = false;
    bool stop_line_detected = false;
    uint32_t stop_line_distance = 0;

#ifdef STOPLIGHTS_MODE
    //cv::Mats used in stoplight algorythm
    cv::Mat old_frame(CAM_RES_Y,CAM_RES_X,CV_8UC1);
    cv::Mat display;
    cv::Mat diffrence(CAM_RES_Y,CAM_RES_X,CV_8UC1);

#endif

    std::vector<std::vector<cv::Point>> yellow_vector;
    std::vector<std::vector<cv::Point>> white_vector;


    ids.init();

    //Read XML file
    laneDetector.UndistXML(cameraMatrix, distCoeffs);
#ifdef DEBUG_MODE
    laneDetector.CreateTrackbars();

#endif

#ifndef NO_USB
    // Lidar communication init
    lidar.init();
    lidar.allocate_memory();
    lidar.calculate_offset();
#endif

    // Shared Memory init
    shm_lidar_data.init();
    shm_lane_points.init();
    shm_usb_to_send.init();
    shm_watchdog.init();
    shm_dataready.init();

    static int denom = 0;

#ifdef STOPLIGHTS_MODE
    cv::namedWindow("Frame",1);
    cv::namedWindow("Light detection",1);
    cv::namedWindow("ROI",1);

    for(int i = 0; i < 15; i++){
        pthread_cond_wait(&algorithm_signal, &algorithm_signal_mutex);
        pthread_mutex_lock(&ids.frame_mutex);
        ids.ids_frame.copyTo(frame);
        pthread_mutex_unlock(&ids.frame_mutex);
        lightDetector.prepare_first_image(frame,old_frame,lightDetector.roi_number);
    }

    while(true){
        if(licznik_czas == 0)
        {
            clock_gettime(CLOCK_MONOTONIC, &start);
        }
        ids.get_frame_to(frame);

#ifdef DEBUG_MODE
        lightDetector.test_roi(frame,display);
#endif //DEBUG_MODE
        lightDetector.find_start(frame,diffrence,old_frame,lightDetector.roi_number);

#ifdef DEBUG_MODE
        if (++denom>5)
        {
            denom = 0;
            cv::imshow("Frame", frame);
            cv::imshow("Light detection", diffrence);
            cv::imshow("ROI", display);

        }
        cv::waitKey(1);
#endif //DEBUG_MODE

        if (lightDetector.start_light == true)
        {
//                std::cout<<"START"<<std::endl;
                red_light_visible = false;
                green_light_visible = true;
                break;
        }
        else{
//                std::cout<<"WAIT"<<std::endl;
    }
        if(licznik_czas > 100)
        {
            licznik_czas = 0;
            clock_gettime(CLOCK_MONOTONIC, &end);
            seconds = (end.tv_sec - start.tv_sec);
            fps  =  1 / (seconds / 100);
            std::cout <<"FPS: " << fps << std::endl;
        }
        else
        {
            licznik_czas++;
        }
    }

#endif //STOPLIGHTS_MODE

// Main loop

    cv::namedWindow("3.1 Yellow Bird Eye", 1);
    cv::namedWindow("3.2 White Bird Eye", 1);

    cv::createTrackbar("f", "3.1 Yellow Bird Eye", &laneDetector.f_i, 1000, update_trackbar);
    cv::createTrackbar("dst", "3.1 Yellow Bird Eye", &laneDetector.dist_i, 1000, update_trackbar);
    cv::createTrackbar("alpha", "3.1 Yellow Bird Eye", &laneDetector.alpha_i, 100, update_trackbar);

    // Bird Eye first calculation
    laneDetector.calculate_bird_var(frame_ref);

    while(true)
    {
        static auto frame_delay_then = std::chrono::high_resolution_clock::now();
        static double frame_delay_max = 0.0;
        START_TIMER
        ids.get_frame_to(ids_image);
        auto frame_delay = (double)(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - frame_delay_then).count());
        frame_delay_then = std::chrono::high_resolution_clock::now();
        if(frame_delay > frame_delay_max)
            frame_delay_max = frame_delay;
        STOP_TIMER("Copying from Camera")
        START_TIMER
        //FPS
        if(licznik_czas == 0)
        {
            START_TIMER2
            clock_gettime(CLOCK_MONOTONIC, &start);
        }
        //FPS

        // Get new frame from camera

        // Process frame
//        laneDetector.Undist(ids_image, undist_frame, cameraMatrix, distCoeffs);
//        STOP_TIMER("Distort")
//        START_TIMER

        laneDetector.bird_eye(ids_image, undist_frame);
        STOP_TIMER("BIRD EYE")
        START_TIMER
        cv::cvtColor(undist_frame, HSV_frame, cv::COLOR_BGR2HSV);
        laneDetector.Hsv_both(HSV_frame, frame_out_yellow, frame_out_white, frame_out_edge_yellow, frame_out_edge_white);
        STOP_TIMER("HSV")
        START_TIMER

        // Detect cones
//        laneDetector.ConeDetection(frame_ids, cone_frame_out, laneDetector.cones_vector);

        // Detect lines
        laneDetector.detectLine_both(frame_out_edge_white, white_vector,frame_out_edge_yellow, yellow_vector);
        STOP_TIMER("Detect lines")
        START_TIMER

       laneDetector.drawPoints_both(white_vector, white_vector_frame,yellow_vector, yellow_vector_frame);
        STOP_TIMER("Draw points")
        START_TIMER

        // Stop line
        laneDetector.StopLine_v2(HSV_frame, hsv_frame, stop_line_detected);
        STOP_TIMER("STOP LINE")
        START_TIMER
        // Push data
        shm_lane_points.push_lane_data(yellow_vector, white_vector, laneDetector.cones_vector);

        // Push data
        shm_usb_to_send.push_scene_data(reset_stm, red_light_visible, green_light_visible, stop_line_detected, stop_line_distance);

        // Data ready
        shm_dataready.push_signal(1u);

        STOP_TIMER("SHMEM")
        START_TIMER


#ifdef DEBUG_MODE
        if(++denom >= IMSHOW_RATE){
            denom = 0;
            // Display info on screen
            //cv::imshow("Camera", frame);
            //cv::imshow("UndsCamera", undist_frame);
            cv::imshow("0 Frame", ids_image);
            //cv::imshow("Hist frame", hist_frame);
//            cv::imshow("Hsv frame", hsv_frame);
            cv::imshow("1.1 Yellow Line", frame_out_yellow);
//            cv::imshow("1.2 White Line", frame_out_white);
//            cv::imshow("2.1 Yellow Edges", frame_out_edge_yellow);
//            cv::imshow("2.2 White Edges", frame_out_edge_white);
            //cv::imshow("BirdEyeTtransform", bird_eye_frame_tr);
            cv::imshow("3.1 Yellow Bird Eye", yellow_bird_eye_frame);
//            cv::imshow("3.2 White Bird Eye", white_bird_eye_frame);
                 cv::imshow("4.1 Yellow Vector", yellow_vector_frame);
            //     cv::imshow("4.2 White Vector", white_vector_frame);
            //     cv::imshow("5 Cone Detect", cone_frame_out);
            //cv::imshow("Hist frame", hist_frame);
            //cv::imshow("Hsv frame", hsv_frame);

            //cv::imshow("TEST", test);
            //test = cv::Mat::zeros(CAM_RES_Y, CAM_RES_X, CV_8UC3);

            yellow_vector_frame = cv::Mat::zeros(CAM_RES_Y, CAM_RES_X, CV_8UC3);
            white_vector_frame = cv::Mat::zeros(CAM_RES_Y, CAM_RES_X, CV_8UC3);
            test_lane = cv::Mat::zeros(CAM_RES_Y, CAM_RES_X, CV_8UC3);

            // Get input from user
            char keypressed = (char)cv::waitKey(1);

            // Process given input
            if( keypressed == 27 )
                break;

            switch(keypressed)
            {
            case '1':
            {
                cv::Mat LIDAR_data = cv::Mat(FRAME_X, FRAME_Y, CV_8UC3);

                while(true)
                {
                    lidar.read_new_data();
                    lidar.polar_to_cartesian();
                    lidar.draw_data(LIDAR_data);

                    // Show data in window
                    cv::imshow("LIDAR", LIDAR_data);
                    // Clear window for next frame data
                    LIDAR_data = cv::Mat::zeros(FRAME_X, FRAME_Y, CV_8UC3);

                    // Get input from user
                    char keypressed_1 = (char)cv::waitKey(100);

                    // Process given input
                    if( keypressed_1 == 27 )
                        break;
                }


                break;
            }
            case '2':

                break;

            default:

                break;
            }
        }
#endif
#ifdef PREVIEW_MODE
        if(++denom >= IMSHOW_RATE){
            denom = 0;
//            cv::imshow("0 Frame", ids_image);
            cv::imshow("1.1 Yellow Line", frame_out_yellow);
            //cv::imshow("4.1 Yellow Vector", yellow_vector_frame);
            cv::waitKey(1);
        }
#endif //PREVIEW_MODE

        if(licznik_czas > 200)
        {

            licznik_czas = 0;
            clock_gettime(CLOCK_MONOTONIC, &end);
            seconds = (end.tv_sec - start.tv_sec);
            fps  =  200*1e6 / TIMER2_DIFF;
            std::cout <<"FPS: " << fps << " dt MAX: "<< frame_delay_max << "ms" << std::endl;
            frame_delay_max = 0.0;
        }
        else
        {
            licznik_czas++;
        }


STOP_TIMER("imshows")

    }

    close_app = true;

    ids.exit();

    shm_lidar_data.close();
    shm_lane_points.close();
    shm_usb_to_send.close();
    shm_watchdog.close();
    shm_dataready.close();

    return 0;
}
