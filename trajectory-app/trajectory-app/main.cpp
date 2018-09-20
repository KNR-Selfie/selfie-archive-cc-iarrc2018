#include "main.h"

#define AVG_ANGLE_COUNT 1

using namespace std;
using namespace cv;


//rectangle algorithm params
int number_of_rec_cols = 40;
int number_of_rec_raws = 30; //liczba pasów detekcji //s

//mats
Mat test_mat = Mat::zeros(LIDAR_MAT_HEIGHT, LIDAR_MAT_WIDTH, CV_8UC3 );
Mat output_mat = Mat::zeros(LIDAR_MAT_HEIGHT, LIDAR_MAT_WIDTH, CV_8UC3 );

Mat  lidar_left_mat = Mat::zeros(LIDAR_MAT_HEIGHT,LIDAR_MAT_WIDTH,CV_8UC3);
Mat  lidar_right_mat = Mat::zeros(LIDAR_MAT_HEIGHT,LIDAR_MAT_WIDTH,CV_8UC3);


int main(int argc, char** argv)
{
    #if defined(TEST_MODE) || defined (DEBUG_MODE)
    //init_trackbars();
    namedWindow("rect_trackbars");
    init_rect_trackbars("rect_trackbars");//trackbars for rect opt
    namedWindow("tryb DEBUG",WINDOW_NORMAL);
    resizeWindow("tryb DEBUG",1000,600);
    #endif

    //splines
    spline_t left_lidar_spline;
    spline_t right_lidar_spline;
    spline_t trajectory_path_spline;

    //set default spline = go straigth
    vector<Point> straight_vector;
    straight_vector.push_back(Point(LIDAR_MAT_WIDTH/2,LIDAR_MAT_HEIGHT));
    straight_vector.push_back(Point(LIDAR_MAT_WIDTH/2,LIDAR_MAT_HEIGHT/2));
    straight_vector.push_back(Point(LIDAR_MAT_WIDTH/2,0));

    left_lidar_spline.set_spline(straight_vector,false);
    right_lidar_spline.set_spline(straight_vector,false);
    trajectory_path_spline.set_spline(straight_vector,true);

    //tangents
    tangent middle_tangent;
    tangent trajectory_tangent;

    //sharedmemory
    SharedMemory left_lidar_points_shm(50001, 12000);
    vector<Point>left_lidar_vec;
    left_lidar_points_shm.init(); //init

    SharedMemory right_lidar_points_shm(50002, 12000);
    vector<Point>right_lidar_vec;
    right_lidar_points_shm.init(); //init

    SharedMemory additional_data_shm(50003,32);
    vector<uint32_t> add_data_container(7);
    additional_data_shm.init();

    //usb communication
    USB_STM USB_COM;
    data_container to_send;
    uint16_t velocity_to_send;
    uint16_t angle;

    USB_COM.init();//init

    init_trackbars();

    bool left_line_detect = 0;
    bool right_line_detect = 0;



    uint16_t angle_to_send;
    uint16_t velocity;

    //offets and wagis
    float servo_weigth;
    float ang_div;
    float far_tg_fac;

    left_lidar_vec.push_back(Point(300,800));
    left_lidar_vec.push_back(Point(350,700));
    left_lidar_vec.push_back(Point(200,600));

    right_lidar_vec.push_back(Point(500,800));
    right_lidar_vec.push_back(Point(450,700));
    right_lidar_vec.push_back(Point(480,600));

////////////////////////////////////////////WHILE///////////////////////////////////////////////////
while(1)
{
    #if defined(TEST_MODE) || defined(DEBUG_MODE)
        number_of_rec_cols = rect_slider[0];
        number_of_rec_raws = rect_slider[1];
    #endif


    //clear buffer which could have another size
    left_lidar_vec.clear();
    right_lidar_vec.clear();

    #ifdef DEBUG_MODE

    test_mat = Mat::zeros(LIDAR_MAT_HEIGHT, LIDAR_MAT_WIDTH, CV_8UC3 );
    output_mat = Mat::zeros(LIDAR_MAT_HEIGHT, LIDAR_MAT_WIDTH, CV_8UC3 );

    lidar_left_mat = Mat::zeros(LIDAR_MAT_HEIGHT,LIDAR_MAT_WIDTH,CV_8UC3);
    lidar_right_mat = Mat::zeros(LIDAR_MAT_HEIGHT,LIDAR_MAT_WIDTH,CV_8UC3);


    //get new set of points
    left_lidar_points_shm.pull_lidar_data(left_lidar_vec);
    right_lidar_points_shm.pull_lidar_data(right_lidar_vec);


    //get additional data
    additional_data_shm.pull_add_data(add_data_container);

    //preview of received points
    points_preview(right_lidar_vec,lidar_right_mat,CV_RGB(255,255,255));
    points_preview(left_lidar_vec,lidar_left_mat,CV_RGB(255,255,0));

    add(lidar_right_mat,lidar_left_mat,lidar_left_mat);
    #endif

    //detection flags
    left_line_detect = 0;
    right_line_detect = 0;

    //begin y condition
    if(right_lidar_vec.size()>2)
    {
        new_optimization(right_lidar_vec,right_lidar_spline,lidar_right_mat);
        right_line_detect = 1;
    }
    if(left_lidar_vec.size()>2)
    {
        new_optimization(left_lidar_vec,left_lidar_spline,lidar_left_mat);
        left_line_detect = 1;
    }


    //set path according to lines
    if(left_line_detect == 1 && right_line_detect == 1)
    {
       two_wall_planner(left_lidar_spline,right_lidar_spline,trajectory_path_spline);
       trajectory_tangent.calculate(trajectory_path_spline,rect_slider[3]);
       trajectory_tangent.angle();

       middle_tangent.calculate(trajectory_path_spline,200);
       middle_tangent.angle();
    }
    else if(right_line_detect)
    {
       one_line_planner(right_lidar_spline,0,trajectory_path_spline);
       trajectory_tangent.calculate(trajectory_path_spline,rect_slider[3]);
       trajectory_tangent.angle();

       middle_tangent.calculate(trajectory_path_spline,200);
       middle_tangent.angle();
    }
    else if(left_line_detect)
    {
       one_line_planner(left_lidar_spline,0,trajectory_path_spline);
       trajectory_tangent.calculate(trajectory_path_spline,rect_slider[3]);
       trajectory_tangent.angle();

       middle_tangent.calculate(trajectory_path_spline,200);
       middle_tangent.angle();
    }

///////////////////////////////////////////////////////////////////////////////////////////
     #if defined(RACE_MODE) || defined(DEBUG_MODE)

    static uint32_t previous_velocity = 2500;
    servo_weigth = 0.9;
    ang_div = abs(middle_tangent.angle_deg - trajectory_tangent.angle_deg);
    far_tg_fac = 0.8;

    //first condition
    if(ang_div>10)
    {
        velocity = 10/ang_div*2500; //*velocity
        servo_weigth = 1;
        far_tg_fac = 0.9;
    }
    else
    {
        uint32_t acceleration = 20;
        velocity = previous_velocity + acceleration;

        if(velocity>5000)
            velocity = 5000;
    }
    previous_velocity = velocity;
    angle = 300 + servo_weigth*(far_tg_fac*middle_tangent.angle_deg + (1.0-far_tg_fac)*trajectory_tangent.angle_deg)*10;

    angle_to_send = angle;
        //pack data
    USB_COM.data_pack(velocity_to_send,angle_to_send,&to_send);

    //send data to stm
    USB_COM.send_buf(to_send);
    #endif

////////////////////////////////////////////////////////////////////////////////////////////////////////
     //display section
    string label;
    #ifdef DEBUG_MODE

        add(lidar_right_mat,lidar_left_mat,output_mat); // to see rectangles

        //draw detected spline
        if(right_line_detect)
            right_lidar_spline.draw(output_mat,CV_RGB(255,255,0));
        if(left_line_detect)
            left_lidar_spline.draw(output_mat,CV_RGB(255,255,255));

        trajectory_path_spline.draw(output_mat,CV_RGB(0,255,0));
        trajectory_tangent.draw(output_mat,CV_RGB(100,100,100));
        middle_tangent.draw(output_mat,CV_RGB(255,0,255));

        //display send angle
        label  = "send_angle: "+ std::to_string(far_tg_fac*middle_tangent.angle_deg + (1.0-far_tg_fac)*trajectory_tangent.angle_deg);
        putText(output_mat, label, Point(450, 40), FONT_HERSHEY_SIMPLEX, 0.5, CV_RGB(0,255,0), 1.0);

        //display send speed
        label = "send speed:" + std::to_string(velocity);
        putText(output_mat, label, Point(450, 80), FONT_HERSHEY_SIMPLEX, 0.5, CV_RGB(0,255,0), 1.0);

        //show white line mat
        imshow("tryb DEBUG",output_mat);

        //clean matrixes

        if(waitKey(30)=='q')
            break;
    #endif
}//end of while(1)

   //left_lidar_points_shm.close();
   //right_lidar_points_shm.close();
   //additional_data_shm.close();

    return 0;

}
