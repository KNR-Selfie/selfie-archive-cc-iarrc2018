#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>

#include <include/sharedmemory.hpp>
#include <include/lidar.hpp>
#include <include/lanedetector.hpp>
#include <include/stoplights.h>
#include <include/ids.h>

// Race mode ---> fps boost
// Debug mode --> display data
//#define RACE_MODE
#define DEBUG_MODE
#define NO_USB
//#define STOPLIGHTS_MODE
#define IDS_MODE

//#define MARCZUK_MODE

#define CAMERA_INDEX 0
#define CAM_RES_X 752//640
#define CAM_RES_Y 480
#define FRAME_TIME 10



// Handlers for custom classes
LaneDetector laneDetector;
StopLightDetector lightDetector;
Lidar lidar;
SharedMemory shm_lidar_data(50001);
SharedMemory shm_lane_points(50002);
SharedMemory shm_usb_to_send(50003);
SharedMemory shm_watchdog(50004);
IDS_PARAMETERS ids_camera;
HIDS hCam = 1;

cv::Mat frame_ref(CAM_RES_Y, CAM_RES_X, CV_8UC1);
void update_trackbar(int, void*)
{
    laneDetector.calculate_bird_var(frame_ref);
}

// Variables for communication between threads
bool close_app = false;
std::mutex mu;

cv::Mat ids_test (480, 752, CV_8UC3);
cv :: Mat rect;

#ifdef MARCZUK_MODE
void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
     if  ( event == cv::EVENT_LBUTTONDOWN )
     {
          std::cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << std::endl;
          int h_sum = 0;
          int s_sum = 0;
          int v_sum = 0;
          int pomx = x;
          int pomy = y;
          for(int i=0;i<10;i++)
          {  x=pomx-5+i;
              for(int j=0;j<10;j++)
              {
                  y=pomy-5+j;
                  h_sum += int(ids_test.at<cv::Vec3b>(cv::Point(x, y))[0]);
                  s_sum += int(ids_test.at<cv::Vec3b>(cv::Point(x, y))[1]);
                  v_sum += int(ids_test.at<cv::Vec3b>(cv::Point(x, y))[2]);

              }

          }

          int h_avg = h_sum/100;
          int s_avg = s_sum/100;
          int v_avg = v_sum/100;

          std::cout<<" H "<<h_avg<<std::endl;
          std::cout<<" S "<<s_avg<<std::endl;
          std::cout<<" V "<<v_avg<<std::endl;

          cv::rectangle(ids_test,cv::Point(pomx-5,pomy-5),cv::Point(pomx+5,pomy+5),cv::Scalar(h_avg,s_avg,v_avg));
          imshow("rect",ids_test);

          cv::waitKey(500);

     }

}
#endif

int main_not()
{
    ids_test.copyTo(rect);
//#ifdef DEBUG_MODE
    //FPS
    struct timespec start, end;
    unsigned int licznik_czas = 0;
    float seconds = 0;
    float fps = 0;
//#endif

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

#ifdef IDS_MODE
    cvNamedWindow("frame_ids",1);
    ids_camera.initialize_camera();
    ids_camera.setting_auto_params();
    ids_camera.change_params();
#ifdef DEBUG_MODE
    ids_camera.create_trackbars();
#endif
#endif

#ifndef IDS_MODE
    // Camera init
    cv::VideoCapture camera;
    camera.open(CAMERA_INDEX, cv::CAP_V4L2);

    // Check if camera is opened propely
    if (!camera.isOpened())
    {
        std::cout << "Error could not open camera on port: " << CAMERA_INDEX << std::endl
                  << "Closing app!" << std::endl;

        camera.release();
        return 0;
    }
    else
    {
        // Set resolution
        camera.set(cv::CAP_PROP_FRAME_WIDTH, CAM_RES_X);
        camera.set(cv::CAP_PROP_FRAME_HEIGHT, CAM_RES_Y);

        //Set and check decoding of frames
        camera.set(cv::CAP_PROP_CONVERT_RGB, true);
        std::cout << "RGB: " << camera.get(cv::CAP_PROP_CONVERT_RGB) << std::endl;
    }
#endif

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

    //Trackbars
    //


    //Read from file
//    cv::Mat frame_gray(CAM_RES_Y, CAM_RES_X, CV_8UC1);
//    frame_gray = cv::imread("../../data/SELFIE-example-img-prosto.png", CV_LOAD_IMAGE_GRAYSCALE);

//    if(!frame_gray.data)
//    {
//        std::cout << "No data!" << std::endl
//                  << "Closing app!" << std::endl;

//        return 0;
//    }


#ifdef STOPLIGHTS_MODE
cv::namedWindow("Frame_ids", 1);
#ifndef IDS_MODE
        camera >> frame;
#endif

#ifdef IDS_MODE
        cv::Mat ids_image (480, 752, CV_8UC3);
        ids_camera.get_frame(&hCam,752,480,ids_image);
        ids_camera.update_params(&hCam);
        ids_image.copyTo(frame_ids);
        lightDetector.prepare_first_image(frame_ids,old_frame,lightDetector.roi_number);
#endif

    while(true)
    {
// Get new frame from camera
#ifndef IDS_MODE
        camera >> frame;
        lightDetector.find_start(frame,diffrence,old_frame,lightDetector.roi_number);
#endif

#ifdef IDS_MODE
        cv::Mat ids_image (480, 752, CV_8UC3);
        ids_camera.get_frame(&hCam,752,480,ids_image);
        ids_camera.update_params(&hCam);
        //other way doesn't work
        ids_image.copyTo(frame_ids);
        cv::imshow("Frame_ids", frame_ids);
        cv::waitKey(20);
        // lightDetector.test_roi(ids_image,display);
        lightDetector.find_start(ids_image,diffrence,old_frame,lightDetector.roi_number);
#endif

#ifdef DEBUG_MODE
        if (lightDetector.start_finding ==true){
                cv::imshow("Frame",ids_image);
                cv::imshow("Light detection",diffrence);
                //  cv::imshow("Light detection", display);
        }
        if (lightDetector.start_light == true){
                std::cout<<"START"<<std::endl;
        }
        else
                std::cout<<"WAIT"<<std::endl;


#endif

    }

#endif
#ifdef IDS_MODE
ids_camera.setting_auto_params();
#endif
// Main loop

#ifdef MARCZUK_MODE

    cv::namedWindow("kolor pick",cv::WINDOW_AUTOSIZE);
    cv::setMouseCallback("kolor pick", CallBackFunc, NULL);

    while(true)
    {


        ids_camera.get_frame(&hCam,752,480,ids_test);
        ids_camera.update_params(&hCam);

        laneDetector.Undist(ids_test, undist_frame, cameraMatrix, distCoeffs);

        laneDetector.Hsv(undist_frame, frame_out_yellow, frame_out_white, frame_out_edge_yellow, frame_out_edge_white);
        //cv::cvtColor(ids_test,ids_test,cv::COLOR_BGR2RGB);
        cv::cvtColor(ids_test,ids_test, cv::COLOR_BGR2HSV);

        imshow("kolor pick",frame_out_yellow);
        char keypressed = (char)cv::waitKey(FRAME_TIME);

        if(keypressed == 'k')
            break;

    }
#endif

    cv::namedWindow("3.1 Yellow Bird Eye", 1);
    cv::namedWindow("3.2 White Bird Eye", 1);

    cv::createTrackbar("f", "3.1 Yellow Bird Eye", &laneDetector.f_i, 1000, update_trackbar);
    cv::createTrackbar("dst", "3.1 Yellow Bird Eye", &laneDetector.dist_i, 1000, update_trackbar);
    cv::createTrackbar("alpha", "3.1 Yellow Bird Eye", &laneDetector.alpha_i, 100, update_trackbar);

    // Bird Eye first calculation
    laneDetector.calculate_bird_var(frame_ref);

    while(true)
    {
//#ifdef DEBUG_MODE
        //FPS
        if(licznik_czas == 0)
        {
            clock_gettime(CLOCK_MONOTONIC, &start);
        }
        //FPS
//#endif

        // Get new frame from camera

#ifndef IDS_MODE
        camera >> frame;
#endif

#ifdef IDS_MODE
//        ids_camera.get_frame(752,480,ids_image);
        ids_camera.update_params();

        //writes how many fps we have
        //std::cout << "FPS: "<<ids_camera.NEWFPS<< std::endl;

        //other way doesn't work
       //ids_image.copyTo(frame_ids);
#endif


        // Process frame
#ifndef IDS_MODE
        laneDetector.Undist(frame, undist_frame, cameraMatrix, distCoeffs);
#endif
#ifdef IDS_MODE
        laneDetector.Undist(ids_image, undist_frame, cameraMatrix, distCoeffs);
#endif
        //laneDetector.Hsv(undist_frame, frame_out_yellow, frame_out_white, frame_out_edge_yellow, frame_out_edge_white);
        laneDetector.Hsv_both(undist_frame, frame_out_yellow, frame_out_white, frame_out_edge_yellow, frame_out_edge_white);
        //laneDetector.BirdEye(frame_out_edge_yellow, yellow_bird_eye_frame);
        //laneDetector.BirdEye(frame_out_edge_white, white_bird_eye_frame);
        laneDetector.bird_eye(frame_out_edge_white, white_bird_eye_frame);
        laneDetector.bird_eye(frame_out_edge_yellow, yellow_bird_eye_frame);
        //laneDetector.colorTransform(yellow_bird_eye_frame, yellow_bird_eye_frame_tr);
        //laneDetector.colorTransform(white_bird_eye_frame, white_bird_eye_frame_tr);

        // Detect cones
        laneDetector.ConeDetection(frame_ids, cone_frame_out, laneDetector.cones_vector);

        // Detect lines
        laneDetector.detectLine_both(white_bird_eye_frame, white_vector,yellow_bird_eye_frame, yellow_vector);
        //laneDetector.detectLine(yellow_bird_eye_frame, yellow_vector);
        //laneDetector.detectLine(white_bird_eye_frame, white_vector);
       laneDetector.drawPoints_both(white_vector, white_vector_frame,yellow_vector, yellow_vector_frame);
        //laneDetector.drawPoints(yellow_vector, yellow_vector_frame);
       // laneDetector.drawPoints(white_vector, white_vector_frame);

        // Push data
        shm_lane_points.push_lane_data(yellow_vector, white_vector, laneDetector.cones_vector);
        // Test pull
        //shm_lane_points.pull_lane_data(test-lane);

        // Push data
        shm_usb_to_send.push_scene_data(reset_stm, red_light_visible, green_light_visible, stop_line_detected, stop_line_distance);
        // Test pull
        // shm_usb_to_send.pull_scene_data();


#ifdef DEBUG_MODE
        // Display info on screen
        //cv::imshow("Camera", frame);
        //cv::imshow("UndsCamera", undist_frame);
#ifdef IDS_MODE
        cv::imshow("0 Frame", ids_image);
#endif
#ifndef IDS_MODE
        cv::imshow("0 Frame",frame);
#endif
        cv::imshow("1.1 Yellow Line", frame_out_yellow);
        cv::imshow("1.2 White Line", frame_out_white);
        cv::imshow("2.1 Yellow Edges", frame_out_edge_yellow);
        cv::imshow("2.2 White Edges", frame_out_edge_white);
        //cv::imshow("BirdEyeTtransform", bird_eye_frame_tr);
        cv::imshow("3.1 Yellow Bird Eye", yellow_bird_eye_frame);
        cv::imshow("3.2 White Bird Eye", white_bird_eye_frame);
   //     cv::imshow("4.1 Yellow Vector", yellow_vector_frame);
   //     cv::imshow("4.2 White Vector", white_vector_frame);
   //     cv::imshow("5 Cone Detect", cone_frame_out);

        //cv::imshow("TEST", test);
        //test = cv::Mat::zeros(CAM_RES_Y, CAM_RES_X, CV_8UC3);

        yellow_vector_frame = cv::Mat::zeros(CAM_RES_Y, CAM_RES_X, CV_8UC3);
        white_vector_frame = cv::Mat::zeros(CAM_RES_Y, CAM_RES_X, CV_8UC3);
        test_lane = cv::Mat::zeros(CAM_RES_Y, CAM_RES_X, CV_8UC3);

        // Get input from user
        char keypressed = (char)cv::waitKey(FRAME_TIME);

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
#endif
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

    close_app = true;
#ifndef IDS_MODE
    camera.release();
#endif

#ifdef IDS_MODE
    is_FreeImageMem(hCam, ids_camera.pMem, ids_camera.memID);
    is_ExitCamera(hCam);
#endif

    shm_lidar_data.close();
    shm_lane_points.close();
    shm_usb_to_send.close();
    shm_watchdog.close();

    return 0;
}
