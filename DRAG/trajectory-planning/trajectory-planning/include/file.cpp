#include "include/file.h"

void points_to_mat(Mat &point_mat, string path)
{
    vector<Point> test_set;
    string line1;
    string line2;

    ifstream myfile (path);
    if (myfile.is_open())
     {
       while ( getline (myfile,line1) )
       {
           getline (myfile,line2);
        test_set.push_back(Point(stoi(line1),stoi(line2)));
       }
       myfile.close();
     }

     else
        cout << "Unable to open file";


    for (int i=0;i<(int)test_set.size();i++){
             Point pom;
             Vec3b  color;
             pom.y = test_set[i].y;
             pom.x = test_set[i].x;

             color = point_mat.at<Vec3b>(pom);
             color.val[0] = 255;
             color.val[1] = 255;
             color.val[2] = 255;
             point_mat.at<Vec3b>(pom) = color;
      }
}
void save_point_to_file(vector<Point> point_vector,String path)
{
    ofstream plik ("/home/selfie/Desktop/cpp-opencv-app/trajectory-planning/trajectory-planning/krzywa_prosta1.txt");

    if(plik.good()==true)
    {
            for(int i=0;i<point_vector.size();i++)
            {
                plik<<point_vector[i].x;
                plik<<endl;
                plik<<point_vector[i].y;
                plik<<endl;
            }
       plik.close();
    }

}

