#include "tangent.hpp"
#include "spline.hpp"
#include <opencv2/opencv.hpp>
#include "math.h"

tangent::tangent()
{


}

void tangent::calculate(spline_t spl, int num)
{   spline_point=num;

    a = spl.spline.deriv(1,spl.X[spline_point]);
    b =-a*spl.X[spline_point]+spl.spline(spl.X[spline_point]);

}

void tangent::draw(Mat& frame,const Scalar& col)
{

    for (int i=0;i<480;i++){
             Point pom;
             Vec3b  color;
             pom.y = i;
             pom.x = a*i+b;

             if(pom.x>640)
                 pom.x = 640;
             else if(pom.x<0)
                 pom.x=0;

             color = frame.at<Vec3b>(pom);
             color.val[0] = col[0];
             color.val[1] = col[1];
             color.val[2] = col[2];
             frame.at<Vec3b>(pom) = color;
      }
}
void tangent::angle(spline_t spl)
{

    angle_deg = atan2( (a*480+b) - spl.Y[spline_point],480 - spl.X[spline_point]);
    if (angle_deg<0)
       angle_deg =  - angle_deg * 90 / M_PI;
    else
       angle_deg = (-angle_deg * 90 / M_PI);
}

void tangent::calculate_poly2(poly2_interp poly2, int x){

    a = 2*poly2.a*x +poly2.b;
    b =-a*x + poly2.a*x*x+poly2.b*x+poly2.c;

}

void tangent::calculate_exp(exp_interp exp_fit, int x){

    a = exp_fit.A*exp_fit.B*exp(exp_fit.B*x);
    b =-a*x + exp_fit.A*exp(exp_fit.B*x);

}
