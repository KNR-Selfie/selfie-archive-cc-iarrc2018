#include "spline.hpp"


spline_t::spline_t()
{


}

void spline_t::add_point()
{

   X.push_back(1);
   Y.push_back(1);
}

void spline_t::delete_point()
{

   X.pop_back();
   Y.pop_back();
}

void spline_t::fit_vector(int points_num)
{   if(!X.empty()){
        if(X.size()< points_num)
        {
            for(int i=0;i<points_num-X.size();i++)
                add_point();

        }

        else if(X.size()>points_num)
        {
            for(int i=0;i<X.size()-points_num;i++)
                delete_point();
        }
    }
    else
    {
        for(int i=0;i<points_num;i++)
            add_point();
    }
}

void spline_t::set_point_value(int num,double x, double y)
{
    X[num]=x;
    Y[num]=y;
}

void spline_t::set_spline()
{
    spline.set_points(X,Y);
}

void spline_t::draw(Mat& frame,const Scalar& col)
{

    for (int i=X[0];i<X[X.size()-1];i++){
        Point pom;
        Vec3b  color;
      pom.y = i;
      pom.x=spline(i);//a*i*i+b*i+c+200;

      if(pom.x<0)
          pom.x=0;
      //else if (pom.x>640)
          //pom.x=640;

      color = frame.at<Vec3b>(pom);
      color.val[0] = col[0];
      color.val[1] = col[1];
      color.val[2] = col[2];
      frame.at<Vec3b>(pom) = color;
     }
}
