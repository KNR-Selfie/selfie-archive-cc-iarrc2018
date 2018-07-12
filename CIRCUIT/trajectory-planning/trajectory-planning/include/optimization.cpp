
#include "optimization.h"

using namespace std;
using namespace cv;

#define OPTTIMING

#ifdef OPTTIMING
#include <chrono>
#define INIT_OPTTIMER static auto start_opt = std::chrono::high_resolution_clock::now();
#define START_OPTTIMER  start_opt = std::chrono::high_resolution_clock::now();
#define STOP_OPTTIMER(name)  std::cout << "RUNTIME of " << name << ": " << \
    std::chrono::duration_cast<std::chrono::microseconds>( \
            std::chrono::high_resolution_clock::now()-start_opt \
    ).count() << " us " << std::endl;
#else
#define INIT_OPTTIMER
#define START_OPTTIMER
#define STOP_OPTTIMER(name)
#endif

int param = 5;
bool rectangle_optimize(Mat& point_mat,spline_t& spl)
{
    INIT_OPTTIMER
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

        START_OPTTIMER
        spl.set_spline(spline_set);
        STOP_OPTTIMER("spline set")
        START_OPTTIMER
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



void optimization(vector<Point2i> input_vector,spline_t &spl)
{
    int rec_height = Height/number_of_rec_raws;
    int rec_width = Width/number_of_rec_cols;//szerokość prostokąta wykrywania

    vector<Point> spl_points;

    int points_number[number_of_rec_raws][number_of_rec_cols];
    for(int i=0;i<number_of_rec_raws;i++)
    {
        for(int j=0;j<number_of_rec_cols;j++)
        {
            points_number[i][j]=0;
        }
    }
    for(int i=0;i<input_vector.size();i++)
    {   int x = input_vector[i].x/rec_width;
        int y = input_vector[i].y/rec_height;
        points_number[y][x]++;
    }



    int max_index = 0;
    int find_flag = 0;
    //pierwszy rzad

    for(int r=number_of_rec_raws-1;r>0;r--)
    {
        int max = 0;

        //szukaj rzad po rzedzie
        if(find_flag == 0)
        {
            for(int c=0;c<number_of_rec_cols;c++)
            {
                if(points_number[r][c]>max)
                {
                    max = points_number[r][c];
                    max_index = c;
                }
            }
            if(max>0)
            {
                find_flag=1; //znaleziony 1 kwadrat
                Point2i pkt = Point2i(max_index*rec_width+0.5*rec_width,r*rec_height+0.5*rec_height);
                spl_points.push_back(pkt); //pushuj środek kwadratu
            }
        }
        else //szukaj tylko w okolicy
        {
            for(int c=max_index - param;c<max_index + param;c++)
            {
                if(points_number[r][c]>max)
                {
                    max = points_number[r][c];
                    max_index =c;
                }
            }
            spl_points.push_back(Point2i(max_index*rec_width+0.5*rec_width,r*rec_height+0.5*rec_height)); //pushuj środek kwadratu

        }

    }
    int ssz = spl_points.size();
    //warunek co najmniej 3 punktow
    if(ssz>2)
    {
        vector<Point2i> spline_set;

        //minimalna liczba punktow
        if(ssz<6)
        {

        spline_set.push_back(spl_points[0]);//pierwszy punkt
        spline_set.push_back(spl_points[1]);//drugi punkt
        spline_set.push_back(spl_points[ssz]);//ostatni punkt

        }

        //jezeli wiecej punktow to wez wiecej do spline
        else
        {
            spline_set.push_back(spl_points[0]);//pierwszy punkt
            for(int i =2;i<ssz/2;i++)
                spline_set.push_back(spl_points[i]);
            spline_set.push_back(spl_points[ssz]);//ostatni punkt
        }

        spl.set_spline(spline_set);
    }

    else//nie wykryto
    {

    }

}
void new_optimization(vector<Point> pts_vector,spline_t& spl,Mat &preview)
{
    int rec_height = Height/number_of_rec_raws;
    int rec_width = Width/number_of_rec_cols;//szerokość prostokąta wykrywania
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
            spline_set.push_back(spline_points[0]);//pierwszy punkt
            for(int i =2;i<spline_points.size()/2;i++)
                spline_set.push_back(spline_points[i]);
            spline_set.push_back(spline_points.back());//ostatni punkt
        }

        for(int i=0;i<spline_set.size()-1;i++)
        {
            if(spline_set[i+1].x==spline_set[i].x)
                spline_set[i].x=spline_set[i].x+1;
        }
        spl.set_spline(spline_set);
    }

    else//nie wykryto
    {
    }


}
