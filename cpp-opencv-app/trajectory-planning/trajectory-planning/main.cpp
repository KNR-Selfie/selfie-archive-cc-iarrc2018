#include <iostream>
#include <opencv2/opencv.hpp>

#include "include/spline.hpp"
#include "include/sliders.h"
#include "include/tangent.hpp"

//#define RACE_MODE //without display
#define DEBUG_MODE //data display
#define SLIDER_MODE

using namespace std;
using namespace cv;

 //points number
const int points_num =4;

Point filla[1][2*480];
const Point* ppts[1];
Mat preview = Mat::zeros( 480, 640, CV_8UC3 ); //blank matrix

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



    //add points
    for(int i =0;i<points_num;i++){
        left_spline.add_point();
        right_spline.add_point();
        path_planner.add_point();
    }



while(1){

    /*#if defined(RACE_MODE) || defined (DEBUG_MODE)
     * get points from memory
     *
     *
     *
     *#endif
     */

    #ifdef SLIDER_MODE
        for(int i=0;i<points_num;i++){

            left_spline.set_point_value(i,160*i,left_slider[i]);
            right_spline.set_point_value(i,160*i,right_slider[i]);
        }

    #endif

     left_spline.set_spline();
     right_spline.set_spline();


     //set path planner points and spline
     for(int i=0;i<(points_num-1);i++){

         path_planner.set_point_value(i,160*i,(right_spline.spline(160*i)+left_spline.spline(160*i))/2);
     }
     path_planner.set_point_value(points_num-1,460,320); //middle of view
     path_planner.set_spline();

     //angle calculation
     styczna1.calculate(path_planner,3);
     styczna2.calculate(path_planner,2);

     styczna1.angle(path_planner);
     styczna2.angle(path_planner);

     /*
      *
      * data write
      *
      *
      */



    #if defined(SLIDER_MODE) || defined (DEBUG_MODE)



        styczna1.draw(preview,CV_RGB(0,255,0));
        styczna2.draw(preview,CV_RGB(0,0,255));
        left_spline.draw(preview,CV_RGB(240,0,0));
        path_planner.draw(preview,CV_RGB(240,240,0));
        right_spline.draw(preview,CV_RGB(240,0,0));

        string label = "traj_ang: "+ std::to_string(styczna1.angle_deg);
        putText(preview, label, Point(450, 40), FONT_HERSHEY_SIMPLEX, 0.5, CV_RGB(0,255,0), 1.0);
        label = "traj_ang: "+ std::to_string(styczna2.angle_deg);
        putText(preview, label, Point(450, 60), FONT_HERSHEY_SIMPLEX, 0.5, CV_RGB(0,0,255), 1.0);

        //poly_fill(preview,CV_RGB(250,45,23),left_spline,right_spline,filla,ppts);

        imshow("obraz",preview);
        if(waitKey(30) >= 0) break;
        preview = Mat::zeros( 480, 640, CV_8UC3 );

    #endif


}
    return 0;
}
