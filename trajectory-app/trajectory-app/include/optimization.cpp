
#include "optimization.h"

using namespace std;
using namespace cv;

int param = 5;

void points_to_mat(Mat& point_mat,vector<Point> points)
{
    for (int i=0;i<(int)points.size();i++)
    {
             Point pom;
             Vec3b  color;
             pom.y = points[i].y;
             pom.x = points[i].x;

             color = point_mat.at<Vec3b>(pom);
             color.val[0] = 255;
             color.val[1] = 255;
             color.val[2] = 255;
             point_mat.at<Vec3b>(pom) = color;
      }


}

void one_line_planner(spline_t r_line,int offset, spline_t& path_line)
{
    //middle of camera view
    Point middle(LIDAR_MAT_WIDTH/2,LIDAR_MAT_HEIGHT-1);

    vector<Point>pom;
    pom.push_back(middle);

    pom.push_back(Point(int(r_line.spline(LIDAR_MAT_HEIGHT/2))+offset,LIDAR_MAT_HEIGHT/2));
    pom.push_back(Point(int(r_line.spline(LIDAR_MAT_HEIGHT/3))+offset,LIDAR_MAT_HEIGHT/3));

    pom.push_back(Point(r_line.spline(1)+offset,1));


    path_line.set_spline(pom,false);

}

void two_line_planner(spline_t y_line, spline_t w_line, int offset, spline_t& path_line)
{
    Point middle(LIDAR_MAT_WIDTH/2,LIDAR_MAT_HEIGHT-1);
    bool direction = false;

    //check if direction is correct
    if(y_line.spline(LIDAR_MAT_HEIGHT)<w_line.spline(LIDAR_MAT_HEIGHT) || y_line.spline(LIDAR_MAT_HEIGHT/2)<w_line.spline(LIDAR_MAT_HEIGHT/2))
    {
        direction = true;
    }

    if(direction)
    {
    vector<Point>pom;
    pom.push_back(middle);
    pom.push_back(Point((w_line.spline(LIDAR_MAT_HEIGHT/2) + y_line.spline(LIDAR_MAT_HEIGHT/2))/2 + offset, LIDAR_MAT_HEIGHT/2));
    pom.push_back(Point((w_line.spline(LIDAR_MAT_HEIGHT/3) + y_line.spline(LIDAR_MAT_HEIGHT/3))/2 + offset, LIDAR_MAT_HEIGHT/3));
    pom.push_back(Point((w_line.spline(1) + y_line.spline(1))/2 + offset, 1));
    path_line.set_spline(pom,true);



    }
}

void new_optimization(vector<Point> pts_vector,spline_t& spl,Mat &preview)
{
    int rec_height = LIDAR_MAT_HEIGHT/number_of_rec_raws;
    int rec_width = LIDAR_MAT_WIDTH/number_of_rec_cols;//szerokość prostokąta wykrywania
    int grid[number_of_rec_cols][number_of_rec_raws];

    vector<Point> spline_points;

    for(int c =0;c<number_of_rec_cols;c++)
    {
        for(int r =0;r<number_of_rec_raws;r++)
        {
            grid[c][r] = 0;
        }
    }

    for(int i = 0;i<pts_vector.size();i++)
    {
        int x = pts_vector[i].x/rec_width;
        int y = pts_vector[i].y/rec_height;

        grid[x][y] = grid[x][y]+1;
    }

    int max_col_index = 0;

    int find_flag = 0;

    for(int r=number_of_rec_raws-1;r>0;r--)
    {
        int max =0;

        if(find_flag ==0)
        {
            for(int c=0;c<number_of_rec_cols;c++)
            {
                if(grid[c][r]>max)
                {
                    max = grid[c][r];
                    max_col_index = c;
                }
            }

            if(max>0)
            {
                find_flag = 1;
                spline_points.push_back(Point(max_col_index*rec_width+0.5*rec_width,r*rec_height+0.5*rec_height));
                rectangle(preview,Point(max_col_index*rec_width,r*rec_height),Point(max_col_index*rec_width+rec_width,r*rec_height+rec_height),CV_RGB(255,0,255));

            }
        }
        else
        {
            for(int c = max_col_index-param;c<max_col_index+param;c++)
            {
                if(c>number_of_rec_cols-1 || c<0)
                    continue;

                if(grid[c][r]>max)
                {
                    max = grid[c][r];
                    max_col_index = c;
                }
                if(max == 0)
                {
                    find_flag = 0;
                    break;
                }
            }

            spline_points.push_back(Point(max_col_index*rec_width+0.5*rec_width,r*rec_height+0.5*rec_height));

            rectangle(preview,Point(max_col_index*rec_width,r*rec_height),Point(max_col_index*rec_width+rec_width,r*rec_height+rec_height),CV_RGB(255,0,255));
        }
    }

    if(spline_points.size()>2)
    {
        vector<Point> spline_set;

        //minimalna liczba punktow
        if(spline_points.size()<6)
        {

        spline_set.push_back(spline_points[0]);//pierwszy punkt
        spline_set.push_back(spline_points[1]);//drugi punkt
        spline_set.push_back(spline_points.back());//ostatni punkt

        }

        //jezeli wiecej punktow to wez wiecej do spline
        else
        {
            //spline_set.push_back(spline_points[0]);//pierwszy punkt
            for(int i =0;i<spline_points.size();i++)
                spline_set.push_back(spline_points[i]);
            //spline_set.push_back(spline_points.back());//ostatni punkt
        }

        for(int i=0;i<spline_set.size()-1;i++)
        {
            if(spline_set[i+1].x==spline_set[i].x)
                spline_set[i].x=spline_set[i].x+1;
        }
        spl.set_spline(spline_set,false);
    }

    else//nie wykryto
    {
    }


}
void two_wall_planner(spline_t left_spline,spline_t right_spline, spline_t &path_line)
{


    vector<Point>pom;
    Point middle(LIDAR_MAT_WIDTH/2,LIDAR_MAT_HEIGHT-1); //midle point of view
    pom.push_back(middle);

    pom.push_back(Point((left_spline.Y[left_spline.Y.size()/2] + right_spline.Y[right_spline.Y.size()/2])/2, (left_spline.X[left_spline.X.size()/2]+right_spline.X[right_spline.X.size()/2])/2));

    pom.push_back(Point((left_spline.Y[0] + right_spline.Y[0])/2, (left_spline.X[0]+right_spline.X[0])/2)); //midle of space between top detected points

    path_line.set_spline(pom,true);

}
