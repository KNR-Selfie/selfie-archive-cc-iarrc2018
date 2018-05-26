#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

#include "include/spline.hpp"
#include "include/sliders.h"
#include "include/tangent.hpp"
#include "include/sharedmemory.hpp"
#include "include/interpolation.h"
#include "include/usb.hpp"

//#define RACE_MODE //without display
#define DEBUG_MODE //data display
#define SLIDER_MODE

using namespace std;
using namespace cv;

 //points number
int left_points_num = 5;
int right_points_num = 4;
int middle_points_num =7;

const int Height = 480;
const int Length = 640;

//poly fill variables
Mat preview = Mat::zeros( Height, Length, CV_8UC3 ); //blank matrix


int main(int argc, char** argv)
{
    #ifdef SLIDER_MODE
        init_trackbars();
    #endif

    #if defined(SLIDER_MODE) || defined (DEBUG_MODE)
        namedWindow("obraz",1);
    #endif

    //spline we need
    spline_t left_spline;
    spline_t right_spline;
    spline_t path_planner;

    //tangent we need
    tangent styczna1;
    tangent styczna2;
    tangent styczna3;

    //distance middle of view <---> middle of trajectory
    double dist;

    //sharedmemory
    SharedMemory shm_lane_points(50002);
    shm_lane_points.init(); //init
    uint32_t lenght = shm_lane_points.get_lenght();//
    std::vector<Point> point_vector;



    //usb communication
    USB_STM USB_COM;
    data_container to_send;
    uint32_t velocity;
    uint32_t angle;
    uint32_t lane_dist;
    unsigned char flags[4] ={255,0,255,0};

    USB_COM.init();




    exp_interp exp_fit;

    poly2_interp pol_fit;

    poly3_interp pol3_fit;



while(1){

    /*#if defined(RACE_MODE) || defined (DEBUG_MODE)


    */


    point_vector.clear();
    lenght = shm_lane_points.get_lenght();

    std::cout << "LENGHT: " << lenght << std::endl;

    shm_lane_points.pull_data(point_vector);


    for(int i=0;i<3;i++)
        cout<< "Punkt nr "<<i+1<<" x :"<<point_vector[i].x<<" y :"<<point_vector[i].y<<endl;;

   cout<< "Punkt nr "<<lenght<<" x :"<<point_vector[lenght-1].x<<" y :"<<point_vector[lenght-1].y<<endl;

   pol3_fit.calculate_3coef(point_vector,lenght);
   pol3_fit.draw(preview,CV_RGB(0,255,0));


    vector<Point> test(5);

    //check vector dimension
    left_spline.fit_vector(left_points_num);
    right_spline.fit_vector(right_points_num);
    path_planner.fit_vector(middle_points_num);


    #ifdef SLIDER_MODE
        for(int i=0;i<left_points_num;i++){
            test[i]=Point(Height/(left_points_num-1)*i,left_slider[i]);
            left_spline.set_point_value(i,Height/(left_points_num-1)*i,left_slider[i]);
            right_spline.set_point_value(i,160*i,right_slider[i]);
        }
        /*
        for(int j=5;j<10;j++){
            test[j] = Point(Height/(left_points_num-1)*(j-5),right_slider[j-5]);
        }*/



    #endif

     left_spline.set_spline();
     right_spline.set_spline();



    styczna3.calculate_exp(exp_fit,220);

    styczna3.draw(preview,CV_RGB(255,0,0));


     //set path planner points and spline

     for(int i=0;i<(middle_points_num-1);i++){

         path_planner.set_point_value(i,480/(middle_points_num-1)*i,(right_spline.spline(480/(middle_points_num-1)*i)+left_spline.spline(480/(middle_points_num-1)*i))/2);
     }
     path_planner.set_point_value(middle_points_num-1,460,320); //middle of view
     path_planner.set_spline();

     //angle calculation
     styczna1.calculate(path_planner,middle_points_num-3);
     styczna2.calculate(path_planner,middle_points_num-4);

     styczna1.angle(path_planner);
     styczna2.angle(path_planner);

     //calculate dist
     dist = left_spline.spline(480) + (right_spline.spline(480) - left_spline.spline(480))/2 - 320;


///////////////////////////////////////////////////////////////////////////////////////////
     angle = (styczna1.angle_deg+90)*1000;
     velocity = 255;

     //send data to STM
     //USB_COM.data_pack(velocity,angle,flags,lane_dist,&to_send);
     to_send.data[0]=74;

     USB_COM.send_buf(to_send);
    // USB_COM.read_buf();

////////////////////////////////////////////////////////////////////////////////////////////////////////
     //display section

    #if defined(SLIDER_MODE) || defined (DEBUG_MODE)

        //poly_fill(preview,CV_RGB(255,255,255),left_spline,right_spline,preview);
         points_preview(test,preview,CV_RGB(20,100,140));

        styczna1.draw(preview,CV_RGB(0,255,0));
        //styczna2.draw(preview,CV_RGB(0,0,255));
        left_spline.draw(preview,CV_RGB(240,0,0));
        //path_planner.draw(preview,CV_RGB(240,240,0));
        right_spline.draw(preview,CV_RGB(240,0,0));

        string label = "traj_ang: "+ std::to_string(styczna1.angle_deg);
        putText(preview, label, Point(450, 40), FONT_HERSHEY_SIMPLEX, 0.5, CV_RGB(0,255,0), 1.0);
        label = "traj_ang: "+ std::to_string(styczna2.angle_deg);
        putText(preview, label, Point(450, 60), FONT_HERSHEY_SIMPLEX, 0.5, CV_RGB(0,0,255), 1.0);
        label = "distance: " + std::to_string(dist);
        putText(preview, label, Point(450, 80), FONT_HERSHEY_SIMPLEX, 0.5, CV_RGB(250,0,255), 1.0);

        imshow("obraz",preview);
        if(waitKey(30) >= 0) break;
        preview = Mat::zeros( 480, 640, CV_8UC3 );

    #endif


}
    return 0;

}

