#include "main.h"
#include <time.h>

using namespace std;
using namespace cv;

//camera params
const int Height = 360;
const int Width = 752;

//rectangle algorithm params
int number_of_rec_cols = 47;
int number_of_rec_raws = 10; //liczba pasów detekcji //slidery ustawiaja

//main MAT
Mat w_mat = Mat::zeros( Height, Width, CV_8UC3 ); //blank matri
Mat y_mat = Mat::zeros( Height, Width, CV_8UC3 );

Mat wy_test_mat = Mat::zeros(Height, Width, CV_8UC3 );
Mat wy_mat = Mat::zeros(Height, Width, CV_8UC3 );

Mat lidar_mat = Mat::zeros(600, 1000, CV_8UC3 );


float car_velocity;
uint16_t tf_mini_distance;
float servo_weigth;
float ang_div;
float far_tg_fac;

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

    //sharedmemory
    SharedMemory shm_lane_points(50002);
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

    SharedMemory shm_dataready(50005,4);
    uint32_t ready_state = 0;
    shm_dataready.init();

    //usb communication
    USB_STM USB_COM;
    data_container to_send;
    uint32_t velocity;
    uint32_t angle;
    uint32_t angle_to_send;


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
////////////////////////////////////////////WHILE///////////////////////////////////////////////////
while(1)
{
    #if defined(TEST_MODE) || defined(DEBUG_MODE)
        number_of_rec_cols = rect_slider[0];
        number_of_rec_raws = rect_slider[1];
    #endif

    #if defined(DEBUG_MODE) || defined(RACE_MODE)
    //clear buffer which could have another size
    w_point_vector.clear();
    y_point_vector.clear();
    c_point_vector.clear();
    //l_point_vector.clear();
    //get new set of points


    if(shm_dataready.pull_signal())
    {
        //shm_lidar_points.pull_lidar_data(l_point_vector);
        shm_lane_points.pull_line_data(y_point_vector,w_point_vector,c_point_vector);
        shm_usb_to_stm.pull_usb_data(usb_from_vision);

        shm_dataready.push_signal(0u);
    }
    else
    {
        shm_usb_to_stm.pull_usb_data(usb_from_vision);

        USB_COM.data_pack(velocity,angle_to_send,usb_from_vision,&to_send);
        USB_COM.send_buf(to_send);

        //cout<<"red: "<<usb_from_vision[1]<<"green: "<<usb_from_vision[2]<<endl;

        USB_COM.read_buf(14,car_velocity,tf_mini_distance,taranis_3_pos,taranis_reset_gear,stm_reset,lights);
        shm_usb_to_stm.push_data(lights,taranis_reset_gear,stm_reset);

        //cout<<"wait "<<"3 pos: "<<(int)taranis_3_pos<<" reset g: "<<(int)taranis_reset_gear<<" stm reset: "<<(int)stm_reset<<" lights: "<<(int)lights<<endl;

        waitKey(20);
        continue;
    }

    //preview of received points
    points_preview(w_point_vector,wy_test_mat,CV_RGB(255,255,255));
    points_preview(y_point_vector,wy_test_mat,CV_RGB(255,255,0));
    points_preview(c_point_vector,wy_test_mat,CV_RGB(255,0,0));

    //preview of lidar
    //points_preview(l_point_vector,lidar_mat,CV_RGB(255,0,255));
    //rectangle(lidar_mat,Point(490,490),Point(510,510),CV_RGB(255,0,0));

    y_line_detect = 0;
    w_line_detect = 0;

    //begin y condition
    if(y_point_vector.size()>2)
    {
        y_line_detect = 1;
        new_optimization(y_point_vector,y_spline,y_mat);
    }
    if(w_point_vector.size()>2)
    {
        w_line_detect = 1;
        new_optimization(w_point_vector,w_spline,w_mat);
    }
    else if(y_point_vector.size() ==2)
    {
        y_line_detect = 1;
        int avg_x = (y_point_vector[0].x+y_point_vector[1].x/2);
        int avg_y = (y_point_vector[0].y+y_point_vector[1].y/2);
        y_point_vector.push_back(Point(avg_x,avg_y));

        new_optimization(y_point_vector,y_spline,y_mat);
    }
    else if(w_point_vector.size() == 2)
    {
        w_line_detect = 1;
        int avg_x = (w_point_vector[0].x+w_point_vector[1].x/2);
        int avg_y = (w_point_vector[0].y+w_point_vector[1].y/2);
        w_point_vector.push_back(Point(avg_x,avg_y));

        new_optimization(w_point_vector,w_spline,w_mat);
    }


    //set path according to lines
    if(y_line_detect == 1 && w_line_detect == 1)
    {
       two_line_planner(y_spline,w_spline,0,trajectory_path);
       trajectory_tangent.calculate(trajectory_path,rect_slider[3]);
       trajectory_tangent.angle();

       middle_tangent.calculate(trajectory_path,200);
       middle_tangent.angle();
    }
    else if(y_line_detect)
    {
        one_line_planner(y_spline,90,trajectory_path);

        trajectory_tangent.calculate(trajectory_path,rect_slider[3]);
        trajectory_tangent.angle();

        middle_tangent.calculate(trajectory_path,200);
        middle_tangent.angle();
    }
    else if(w_line_detect)
    {
        one_line_planner(w_spline,-90,trajectory_path);
        trajectory_tangent.calculate(trajectory_path,rect_slider[3]);
        trajectory_tangent.angle();

        middle_tangent.calculate(trajectory_path,200);
        middle_tangent.angle();
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
    //nominal values
        velocity = 4000;
        servo_weigth = 0.9;
        ang_div = abs(middle_tangent.angle_deg - trajectory_tangent.angle_deg);
        far_tg_fac = 0.8;

        //first condition
        if(ang_div>10)
        {
            velocity = 10/ang_div*velocity;
            servo_weigth = 1;
            far_tg_fac = 0.9;
        }

        angle = 300 + servo_weigth*(far_tg_fac*middle_tangent.angle_deg + (1.0-far_tg_fac)*trajectory_tangent.angle_deg)*10;

        angle_to_send = angle;

        //cout<<"red: "<<usb_from_vision[1]<<"green: "<<usb_from_vision[2]<<endl;
        USB_COM.data_pack(velocity,angle_to_send,usb_from_vision,&to_send);
        USB_COM.send_buf(to_send);

        USB_COM.read_buf(14,car_velocity,tf_mini_distance,taranis_3_pos,taranis_reset_gear,stm_reset,lights);

        //cout<<"loop "<<"3 pos: "<<(int)taranis_3_pos<<" reset g: "<<(int)taranis_reset_gear<<" stm reset: "<<(int)stm_reset<<" lights: "<<(int)lights<<endl;

        //app reset
        //if(taranis_reset_gear)
            //system("gnome-terminal --geometry 20x35+0+0 -x sh -c '~/Desktop/Selfie-autonomous-car/CIRCUIT/CIRCUIT.sh; bash'");

        shm_usb_to_stm.push_data(lights,taranis_reset_gear,stm_reset);


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


        label  = "traj_ang: "+ std::to_string(0.3*middle_tangent.angle_deg + 0.7*trajectory_tangent.angle_deg);
        putText(wy_mat, label, Point(450, 40), FONT_HERSHEY_SIMPLEX, 0.5, CV_RGB(0,255,0), 1.0);

        //show white line mat
        imshow("tryb DEBUG",wy_mat);
        imshow("Podglad", wy_test_mat);
        //imshow("Lidar",lidar_mat);

        //clean matrixes
        wy_mat = Mat::zeros(Height,Width,CV_8UC3);
        w_mat = Mat::zeros(Height,Width,CV_8UC3);
        y_mat = Mat::zeros(Height,Width,CV_8UC3);
        wy_test_mat = Mat::zeros( Height, Width, CV_8UC3 );//zero matrix
        //lidar_mat = Mat::zeros(600,1000, CV_8UC3 );

        if(waitKey(2)=='q')
            break;
    #endif
}//end of while(1)

   // shm_lane_points.close();
    return 0;

}

