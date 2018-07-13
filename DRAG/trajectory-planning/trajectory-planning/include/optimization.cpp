
#include "optimization.h"

using namespace std;
using namespace cv;


int param = 2;
bool rectangle_optimize(Mat& point_mat,spline_t& spl)
{
    vector<Point> spline_points;
    int rec_height = Height/number_of_rec_raws;
    int rec_width = Width/number_of_rec_cols;//szerokość prostokąta wykrywania
    int previous_max_pixel_rec_index = 0;
    int find_flag = 0;

    for(int j=0;j<number_of_rec_raws;j++)//przejscie do kolejnego pasa //jeden do gory
    {
        vector<int> number_of_pixels_in_rect;//liczba pixeli w prostokacie

        if(find_flag == 0)//szukaj po rzedach poki nie znajdzie punktow
        {
            for(int i=0;i<number_of_rec_cols;i++)//przejscie po jednym pasie //tylko za pierwszym razem!
            {
                number_of_pixels_in_rect.push_back(0);

                for(int k=Height-(j+1)*rec_height;k<Height-j*rec_height;k++)
                {
                    for(int m =i*rec_width;m<i*rec_width+rec_width;m++)
                    {
                        //check if pixel is white
                        Vec3b colour = point_mat.at<Vec3b>(Point(m, k));

                        if(colour.val[0]==255 && colour.val[1]==255 && colour.val[2]==255)
                        {
                            number_of_pixels_in_rect.back() = number_of_pixels_in_rect.back()+1;
                        }
                    }
                }
             }


            int max_pixel_count =0;
            int max_pixel_rec_index=0;

            //find max pixel count
            for(int i=0;i<number_of_pixels_in_rect.size();i++)
            {
                if(number_of_pixels_in_rect[i]>max_pixel_count && number_of_pixels_in_rect[i] <rect_slider[2])
                {
                    max_pixel_count = number_of_pixels_in_rect[i];
                    max_pixel_rec_index = i;
                }

            }
            previous_max_pixel_rec_index = max_pixel_rec_index;

            if(previous_max_pixel_rec_index!=0)
            {
                find_flag = 1;
            }


            //draw rectangles
            if(max_pixel_count!=0)
            {
                //cout<<max_pixel_rec_index<<endl;
                Rect search_rectangle(max_pixel_rec_index*rec_width,Height-(j+1)*rec_height,rec_width,rec_height);
                rectangle(point_mat,search_rectangle,CV_RGB(255,0,255));
                spline_points.push_back((search_rectangle.br() + search_rectangle.tl())*0.5);//middle of rectangle
            }

        }

        else
        {       //petla do wykrycia rozkładu pixeli
                for(int i = previous_max_pixel_rec_index-param;i<previous_max_pixel_rec_index+param+1;i++)
                {//przeszukiwanie pasa tylko w zakresie [-param +param]
                    number_of_pixels_in_rect.push_back(0);//dodaj kwadrat

                    for(int k=Height-(j+1)*rec_height;k<Height-j*rec_height;k++) //wysokość pasa
                    {
                        for(int m = i*rec_width;m<i*rec_width+rec_width;m++)//szerokosc prostokata
                        {
                            //check if pixel is white
                            Vec3b colour = point_mat.at<Vec3b>(Point(m, k));

                            if(colour.val[0]==255 && colour.val[1]==255 && colour.val[2]==255)
                            {
                                number_of_pixels_in_rect.back() = number_of_pixels_in_rect.back()+1;//przypisz kazdemu kwadratowi liczbe punktow
                            }
                        }
                     }
                  }


                int max_pixel_count =0;
                int max_pixel_rec_index=0;

                //find max pixel count
                for(int i=0;i<number_of_pixels_in_rect.size();i++)
                {
                    if(number_of_pixels_in_rect[i]>max_pixel_count &&number_of_pixels_in_rect[i] <rect_slider[2])
                    {
                        max_pixel_count = number_of_pixels_in_rect[i];
                        max_pixel_rec_index = i+previous_max_pixel_rec_index-param;
                    }

                }

                if(max_pixel_rec_index>0)
                    previous_max_pixel_rec_index = max_pixel_rec_index;


                //draw rectangles
                if(max_pixel_count!=0)
                {
                    //cout<<max_pixel_rec_index<<endl;
                    Rect search_rectangle(max_pixel_rec_index*rec_width,Height-(j+1)*rec_height,rec_width,rec_height);
                    rectangle(point_mat,search_rectangle,CV_RGB(255,0,255));
                    spline_points.push_back((search_rectangle.br() + search_rectangle.tl())*0.5);//middle of rectangle
                }
        }

    }
    //warunek co najmniej 3 punktow
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
            spline_set.push_back(spline_points[0]);//pierwszy punkt
            for(int i =2;i<spline_points.size()/2;i++)
                spline_set.push_back(spline_points[i]);
            spline_set.push_back(spline_points.back());//ostatni punkt
        }


        spl.set_spline(spline_set);
        return true;
    }

    else//nie wykryto
    {
        return false;
    }


}


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
    Point middle(Width/2,Height-1);

    vector<Point>pom;
    pom.push_back(middle);

    pom.push_back(Point(int(r_line.spline(Height/2))+offset,Height/2));
    pom.push_back(Point(int(r_line.spline(Height/3))+offset,Height/3));

    pom.push_back(Point(r_line.spline(1)+offset,1));


    path_line.set_spline(pom);

}

void two_line_planner(spline_t y_line, spline_t w_line, int offset, spline_t& path_line)
{
    Point middle(Width/2,Height-1);
    bool direction = false;

    //check if direction is correct
    if(y_line.spline(Height)<w_line.spline(Height) || y_line.spline(Height/2)<w_line.spline(Height/2))
    {
        direction = true;
    }

    if(direction)
    {
    vector<Point>pom;
    pom.push_back(middle);
    pom.push_back(Point((w_line.spline(Height/2) + y_line.spline(Height/2))/2 + offset, Height/2));
    pom.push_back(Point((w_line.spline(Height/3) + y_line.spline(Height/3))/2 + offset, Height/3));
    pom.push_back(Point((w_line.spline(1) + y_line.spline(1))/2 + offset, 1));
    path_line.set_spline(pom);



    }
}

void line_search(int previous_angle, spline_t &path_line)
{
    vector<Point> pom;
    //last detected angle of path
    if(previous_angle>0)
    {

        pom.push_back(Point(Width/2,Height));
        pom.push_back(Point (Width,Height/2));
        pom.push_back(Point(Width,0));

        path_line.set_spline(pom);
    }
    else
    {
        pom.push_back(Point(Width/2,Height));
        pom.push_back(Point (0,Height/2));
        pom.push_back(Point(0,0));

        path_line.set_spline(pom);
    }

}

bool line_side(vector<Point> point_vector)
{
    int right_counter = 0;
    int left_counter = 0;
    for(int i=0;i<point_vector.size();i++)
    {
        if(point_vector[i].x>Width/2)
            right_counter++;
        else
            left_counter++;

    }

    if(right_counter>left_counter)
        return true;
    else
        return false;
}

void drag_optimization(vector<Point> y_point_vector,spline_t &trajectory_path,int drag_offset)
{
    vector<Point> pom;

    pom.push_back(Point(Width/2,Height-1));//srodek

    //pom.push_back(y_point_vector[y_point_vector.size()-2]);
    pom.push_back(y_point_vector[1]+Point(drag_offset,0));
    pom.push_back(y_point_vector.back()+Point(drag_offset,0));

    trajectory_path.set_spline(pom);//poprowadz spline
}


int auto_offset(vector<Point> pts)
{   int offset=0;
    int sum=0;;
    for(int i = 0;i<pts.size();i++)
    {
        sum += pts[i].x;
    }
    offset = sum/pts.size() - Width/2;
   return offset;
}


