#include "interpolation.h"

using namespace std;

poly2_interp::poly2_interp(){

}

poly3_interp::poly3_interp(){

}

exp_interp::exp_interp(){

}


void poly2_interp::calculate_coef(vector<Point> pts){

   double arr_x[3][3];
   double arr_y[3][1];

   float sum_x = 0;
   double sum_x2 = 0;
   double sum_x3 = 0;
   double sum_x4 = 0;

   double sum_y = 0;
   double sum_xy = 0;
   double sum_x2y= 0;


    for(int i=0;i<int(pts.size());i++){

       sum_x += pts[i].x;
       sum_x2 += pts[i].x*pts[i].x;
       sum_x3 += pts[i].x*pts[i].x*pts[i].x;
       sum_x4 += pts[i].x*pts[i].x*pts[i].x*pts[i].x;

       sum_y += pts[i].y;
       sum_xy +=pts[i].x*pts[i].y;
       sum_x2y +=pts[i].x*pts[i].x*pts[i].y;
    }


            //fill an arr_x
    for(int j=0;j<3;j++){
        for(int i=0;i<3;i++){
            if(i==0 && j==0)
                arr_x[j][i] = double(pts.size());

            else if((i==0 && j==1) || (i==1 && j==0))
                arr_x[j][i] = sum_x;
            else if((i==2 && j==0) || (i==1 && j==1) || (i==0 && j==2))
                arr_x[j][i] = sum_x2;
            else if((i==2 && j ==1)|| (i==1 && j==2))
                arr_x[j][i] =sum_x3;
            else if(i==2 && j==2)
                arr_x[j][i] = sum_x4;
        }
    }

    arr_y[0][0] = sum_y;
    arr_y[1][0] = sum_xy;
    arr_y[2][0] = sum_x2y;

    double inv_arr_x[3][3];
    double det;
/*
    for(int i=0;i<3;i++){
        cout<<arr_x<<" ";
    }
    cout<<endl;
*/
    INVERT_3X3(inv_arr_x,det,arr_x);

    double coef_arr[3][1];

    for(int j =0;j<3;j++ ){
        double tmp = 0;
        for(int i=0;i<3;i++){
            tmp += inv_arr_x[j][i]*arr_y[i][0];
        }
        coef_arr[j][0]=tmp;
    }
    a = coef_arr[2][0];
    b = coef_arr[1][0];
    c = coef_arr[0][0];

}

void poly2_interp::draw(Mat& frame,const Scalar& col)
{
    cout<<a<<endl<<b<<endl<<c<<endl;
    for (int i=0;i<640;i++){
        Point pom;
        Vec3b  color;
      pom.y = a*i*i+b*i+c;
      if(pom.y>480){}
      pom.x= i;


      color = frame.at<Vec3b>(pom);
      color.val[0] = col[0];
      color.val[1] = col[1];
      color.val[2] = col[2];
      frame.at<Vec3b>(pom) = color;
     }
}



void poly3_interp::calculate_3coef(vector<Point> pts){


   double arr_x[4][4];
   double arr_y[4][1];

   double sum_x = 0;
   double sum_x2 = 0;
   double sum_x3 = 0;
   double sum_x4 = 0;
   double sum_x5 =0;
   double sum_x6 = 0;

   double sum_y = 0;
   double sum_xy = 0;
   double sum_x2y= 0;
   double sum_x3y=0;


    for(int i=0;i<pts.size();i++){


       sum_x += pts[i].x;
       sum_x2 += pts[i].x*pts[i].x;
       sum_x3 += pts[i].x*pts[i].x*pts[i].x;
       sum_x4 += pts[i].x*pts[i].x*pts[i].x*pts[i].x;
       sum_x5 += pts[i].x*pts[i].x*pts[i].x*pts[i].x*pts[i].x;
       sum_x6 += pts[i].x*pts[i].x*pts[i].x*pts[i].x*pts[i].x*pts[i].x;

       sum_y += pts[i].y;
       sum_xy +=pts[i].x*pts[i].y;
       sum_x2y +=pts[i].x*pts[i].x*pts[i].y;
       sum_x3y +=pts[i].x*pts[i].x*pts[i].x*pts[i].y;


    }


            //fill an arr_x
    for(int j=0;j<4;j++){
        for(int i=0;i<4;i++){
            if(i==0 && j==0)
                arr_x[j][i] = int(pts.size());
            else if((i==0 && j==1) || (i==1 && j==0))
                arr_x[j][i] = sum_x;
            else if((i==2 && j==0) || (i==1 && j==1) || (i==0 && j==2))
                arr_x[j][i] = sum_x2;
            else if((i==2 && j ==1)|| (i==1 && j==2) || (i==3 && j==0) || (i==0 && j ==3))
                arr_x[j][i] =sum_x3;
            else if((i==3 && j==1 ) || (i==2 && j==2)  || (i==1 && j==3))
                arr_x[j][i] = sum_x4;
            else if((i==3 && j==2)||(i==2 && j==3))
                arr_x[j][i]=sum_x5;
            else if(i==3 && j==3)
                arr_x[j][i]=sum_x6;
        }
    }

    arr_y[0][0] = sum_y;
    arr_y[1][0] = sum_xy;
    arr_y[2][0] = sum_x2y;
    arr_y[3][0] = sum_x3y;

    double inv_arr_x[4][4];
    double det;

    INVERT_4X4(inv_arr_x,det,arr_x);

    float coef_arr[4][1];

    for(int j =0;j<4;j++ ){
        float tmp = 0;
        for(int i=0;i<4;i++){
            tmp += inv_arr_x[j][i]*arr_y[i][0];
        }
        coef_arr[j][0]=tmp;
    }
    a = coef_arr[3][0];
    b = coef_arr[2][0];
    c = coef_arr[1][0];
    d = coef_arr[0][0];

}

void poly3_interp::draw(Mat& frame,const Scalar& col)
{

    for (int i=0;i<480;i++){
        Point pom;
        Vec3b  color;
      pom.y = i;
      pom.x=a*i*i*i + b*i*i + c*i +d;


      color = frame.at<Vec3b>(pom);
      color.val[0] = col[0];
      color.val[1] = col[1];
      color.val[2] = col[2];
      frame.at<Vec3b>(pom) = color;
     }
}


void exp_interp::calculate_exp(vector<Point> pts , uint32_t len){
{
        double sum_lny = 0;
        double sum_x2 = 0;
        double sum_x = 0;
        double sum_xlny = 0;


        for(int i=0;i<len;i++){
            sum_lny +=log(pts[i].y);
            sum_xlny +=pts[i].x*log(pts[i].y);
            sum_x += pts[i].x;
            sum_x2 += pts[i].x *pts[i].x;
        }

        double a = (sum_lny*sum_x2 - sum_x*sum_xlny)/(pts.size()*sum_x2-sum_x*sum_x);
        double b = (pts.size()*sum_xlny - sum_x*sum_lny)/(pts.size()*sum_x2 -sum_x*sum_x);

        A= exp(a);
        B =b;
    }


}

void exp_interp::draw(Mat& frame,const Scalar& col)
{

    for (int i=0;i<480;i++){
        Point pom;
        Vec3b  color;
      pom.y = i;
      pom.x=A*exp(B*i);

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
