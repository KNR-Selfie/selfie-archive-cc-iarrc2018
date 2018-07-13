#include "main.h"

#define AVG_ANGLE_COUNT 1

using namespace std;
using namespace cv;

//camera params
const int Height = 360;
const int Width = 752;

//rectangle algorithm params
int number_of_rec_cols = 40;
int number_of_rec_raws = 10; //liczba pasów detekcji //s
//main MAT
Mat w_mat = Mat::zeros( Height, Width, CV_8UC3 ); //blank matri
Mat y_mat = Mat::zeros( Height, Width, CV_8UC3 );

Mat wy_test_mat = Mat::zeros(Height, Width, CV_8UC3 );
Mat wy_mat = Mat::zeros(Height, Width, CV_8UC3 );

Mat lidar_mat = Mat::zeros(600, 1000, CV_8UC3 );


uint32_t angle_to_send;
uint32_t velocity;

int main(int argc, char** argv)
{

    #if defined(TEST_MODE) || defined (DEBUG_MODE)
    //init_trackbars();
    namedWindow("rect_trackbars");
    init_rect_trackbars("rect_trackbars");//trackbars for rect opt
    #endif


    //splines
    spline_t y_spline;
    spline_t trajectory_path;
    spline_t w_spline;

    //set default spline = go straigth
    vector<Point> straight_vector;
    straight_vector.push_back(Point(Width/2,Height));
    straight_vector.push_back(Point(Width/2,Height/2));
    straight_vector.push_back(Point(Width/2,0));

    y_spline.set_spline(straight_vector);
    w_spline.set_spline(straight_vector);
    trajectory_path.set_spline(straight_vector);

    //tangents
    tangent y_tangent;
    tangent w_tangent;
    tangent trajectory_tangent;
    tangent middle_tangent;


    tangent tangents[5];


    //sharedmemory
    SharedMemory shm_lane_points(50002, 5000);
    vector<Point> w_point_vector; //vector for white line
    vector<Point> y_point_vector; //vector for yellow line
    vector<Point> c_point_vector; //vector for cons
    shm_lane_points.init(); //init

    SharedMemory shm_lidar_points(50001);
    vector<Point> l_point_vector; //vector for lidar points
    shm_lidar_points.init();


    SharedMemory shm_usb_to_stm(50003,32);
    vector<uint32_t> usb_from_vision(6);
    shm_usb_to_stm.init();



    //usb communication
    USB_STM USB_COM;
    data_container to_send;
    uint32_t velocity_to_send;
    uint32_t angle;


    //usb receive data
    float car_velocity;
    uint16_t tf_mini_distance=0;
    uint8_t taranis_3_pos=0;
    uint8_t taranis_reset_gear=0;
    uint8_t stm_reset = 0;
    uint8_t lights = 0;
    USB_COM.init();//init

    init_trackbars();

    bool y_line_detect = 0;
    bool w_line_detect = 0;

    //average angle
    uint32_t average_angle_counter = 0;
    uint32_t angle_sum = 0;

    //offsets and wagis
    uint8_t side_dect_flag = 1;
    float ang_div = 0;
    int drag_offset = 300; //default
    float servo_weight = 0.5;
    float far_tg_fac = 0.9;

////////////////////////////////////////////WHILE///////////////////////////////////////////////////
while(1)
{
    #if defined(TEST_MODE) || defined(DEBUG_MODE)
        number_of_rec_cols = rect_slider[0];
        number_of_rec_raws = rect_slider[1];
    #endif

    #ifdef DEBUG_MODE
    //clear buffer which could have another size
    w_point_vector.clear();
    y_point_vector.clear();
    c_point_vector.clear();
    //l_point_vector.clear();

    //get new set of points
    shm_lane_points.pull_line_data(y_point_vector,w_point_vector,c_point_vector);
    shm_usb_to_stm.pull_usb_data(usb_from_vision);
    shm_lidar_points.pull_lidar_data(l_point_vector);

    //preview of received points
    points_preview(w_point_vector,wy_test_mat,CV_RGB(255,255,255));
    points_preview(y_point_vector,wy_test_mat,CV_RGB(255,255,0));
    points_preview(c_point_vector,wy_test_mat,CV_RGB(255,0,0));

    //preview of lidar
    //points_preview(l_point_vector,lidar_mat,CV_RGB(255,0,255));
    //rectangle(lidar_mat,Point(490,490),Point(510,510),CV_RGB(255,0,0));

    imshow("Podglad", wy_test_mat);

    y_line_detect = 0;
    w_line_detect = 0;

    //begin y condition
    if(y_point_vector.size()>2)
    {
        y_line_detect = 1;

        //lane side - only first time
        if(side_dect_flag)
        {
            drag_offset = auto_offset(y_point_vector);
            side_dect_flag = 0;
            cout<<"drag offset: "<<drag_offset;
        }

    }
    if(w_point_vector.size()>1)
    {
        w_line_detect = 1;
    }


    //set path according to lines
    if(y_line_detect == 1 && w_line_detect == 1)
    {
       two_line_planner(y_spline,w_spline,0,trajectory_path);
       trajectory_tangent.calculate(trajectory_path,rect_slider[3]);
       trajectory_tangent.angle();
    }
    else if(y_line_detect)
    {   drag_offset =-50;
/////////////////////Versja bez rect////////////
        drag_optimization(y_point_vector,trajectory_path,drag_offset);
/////////////////////////////////////////////////////////////////

        //5 tangents

        for(int i =0;i<5;i++)
        {
            tangents[i].calculate(trajectory_path,Height/5*(i+1));
            tangents[i].angle();
        }
        //5 tangents end
        trajectory_tangent.calculate(trajectory_path,rect_slider[3]);
        middle_tangent.calculate(trajectory_path,50);
        trajectory_tangent.angle();
        middle_tangent.angle();
    }
    else if(w_line_detect)
    {
        one_line_planner(w_spline,0,trajectory_path);
        trajectory_tangent.calculate(trajectory_path,rect_slider[3]);
        trajectory_tangent.angle();
    }
    else
    {

    }



    #endif

    #ifdef TEST_MODE
        points_to_mat(test_mat,"/home/mateusz/GIT/Selfie-autonomous-car/cpp-opencv-app/trajectory-planning/trajectory-planning/selfie_testy/prosta_syf.txt");
        rectangle_optimize(test_mat,test_spline);
        one_line_planner(test_spline,rect_slider[4],test_path);
        test_tangent.calculate(test_path,rect_slider[3]);
    #endif


///////////////////////////////////////////////////////////////////////////////////////////
     #if defined(RACE_MODE) || defined(DEBUG_MODE)

        velocity = 2000;
        servo_weight = 1.3;
        ang_div = abs(middle_tangent.angle_deg - trajectory_tangent.angle_deg);
        far_tg_fac = 0.7;

        if(abs(trajectory_tangent.angle_deg)<20)
            velocity_to_send =8000;
        else
            velocity_to_send = 5xxxx000;

        //angle = 300 + servo_weight*((far_tg_fac*middle_tangent.angle_deg + (1.0-far_tg_fac)*trajectory_tangent.angle_deg))*10;
        //5 tangents
        angle = 300 + servo_weight*(0.3*trajectory_tangent.angle_deg + 0.2*tangents[0].angle_deg +  0.2*tangents[1].angle_deg + 0.2*tangents[2].angle_deg + 0.2*tangents[3].angle_deg + 0.2*tangents[4].angle_deg);


        angle_to_send = angle;
        //send data to STM
        USB_COM.data_pack(velocity_to_send,angle_to_send,usb_from_vision,&to_send);

        USB_COM.send_buf(to_send);

        //read 12 byte data from stm 0-2 frame, 3-6 velocity, 7-8 tf_mini, 9-10 futaba gears

        USB_COM.read_buf(14,car_velocity,tf_mini_distance,taranis_3_pos,taranis_reset_gear,stm_reset,lights);

        //app reset
        //if(taranis_reset_gear)
            //system("gnome-terminal --geometry 20x35+0+0 -x sh -c '~/Desktop/Selfie-autonomous-car/DRAG/DRAG.sh; bash'");

        //taranis to shm
        shm_usb_to_stm.push_data(lights,taranis_reset_gear,stm_reset);

       //cout<<"3 pos: "<<int(taranis_3_pos)<<" reset "<<int(taranis_reset_gear)<<" stm_reset "<<(int)stm_reset<<" ligths "<<(int)lights<<endl;


        //read data from STM
        angle_sum = 0;
        average_angle_counter = 0;

    #endif

////////////////////////////////////////////////////////////////////////////////////////////////////////
     //display section
    string label;

    #ifdef TEST_MODE

        //test display
        test_spline.draw(test_mat,CV_RGB(20,20,250));//draw white right line
        test_path.draw(test_mat,CV_RGB(225,20,100));
        test_tangent.draw(test_mat,CV_RGB(255,255,0));

        label  = "traj_ang: "+ std::to_string(test_tangent.angle_deg);
        putText(test_mat, label, Point(450, 40), FONT_HERSHEY_SIMPLEX, 0.5, CV_RGB(0,255,0), 1.0);

        //show white line mat
        imshow("tryb TEST",test_mat);
        test_mat = Mat::zeros( 480, 640, CV_8UC3 );//zero matrix
    #endif

    #ifdef DEBUG_MODE

        add(y_mat,w_mat,wy_mat); // to see rectangles

        //draw detected spline
        if(y_line_detect)
        y_spline.draw(wy_mat,CV_RGB(255,255,0));
        if(w_line_detect)
        w_spline.draw(wy_mat,CV_RGB(255,255,255));

        trajectory_path.draw(wy_mat,CV_RGB(0,255,0));
        trajectory_tangent.draw(wy_mat,CV_RGB(100,100,100));
        middle_tangent.draw(wy_mat,CV_RGB(255,0,255));


        label  = "traj_ang: "+ std::to_string(trajectory_tangent.angle_deg);

        putText(wy_mat, label, Point(450, 40), FONT_HERSHEY_SIMPLEX, 0.5, CV_RGB(0,255,0), 1.0);

        label = std::to_string(middle_tangent.angle_deg);
        putText(wy_mat, label, Point(450, 80), FONT_HERSHEY_SIMPLEX, 0.5, CV_RGB(0,255,0), 1.0);
        //show white line mat
        imshow("tryb DEBUG",wy_mat);

        //clean matrixes
        wy_mat = Mat::zeros(Height,Width,CV_8UC3);
        w_mat = Mat::zeros(Height,Width,CV_8UC3);
        y_mat = Mat::zeros(Height,Width,CV_8UC3);
        wy_test_mat = Mat::zeros( Height, Width, CV_8UC3 );//zero matrix
        lidar_mat = Mat::zeros(600,1000, CV_8UC3 );

        if(waitKey(30)=='q')
            break;
    #endif
}//end of while(1)

//   shm_lane_points.close();
//   shm_usb_to_stm.close();
//   shm_lidar_points.close();

    return 0;

}
