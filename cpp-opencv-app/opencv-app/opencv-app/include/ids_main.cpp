#include <iostream>

#include <include/ids.h>
#include <pthread.h>

IDS_PARAMETERS ids_c;

pthread_cond_t algorithm_signal = PTHREAD_COND_INITIALIZER;
pthread_mutex_t algorithm_signal_mutex = PTHREAD_MUTEX_INITIALIZER;
void *camera_thread(void *x){

    while(true){
        ids_c.frame_loop();
    }
    return NULL;
}


int main(void){
    struct timespec start, end;
    unsigned int licznik_czas = 0;
    float seconds = 0;
    float fps = 0;
    cv::Mat frame(480, 752, CV_8UC3);
    pthread_mutex_init(&algorithm_signal_mutex,NULL);
    ids_c.initialize_camera();
    ids_c.setting_auto_params();
    ids_c.change_params();

    ids_c.create_trackbars();
    cvNamedWindow("fram",1);

    pthread_t frame_thread;

    int x = 0;
    if (pthread_create(&frame_thread,NULL,camera_thread,&x)){
        std::cout<<"Error creating thread"<<std::endl;
    }
    else{
         std::cout << "Thread started" << std::endl;
    }

    while(true)
    {
        pthread_cond_wait(&algorithm_signal, &algorithm_signal_mutex);
        if(licznik_czas == 0)
        {
            clock_gettime(CLOCK_MONOTONIC, &start);
        }
        pthread_mutex_lock(&ids_c.frame_mutex);
        ids_c.ids_frame.copyTo(frame);
        pthread_mutex_unlock(&ids_c.frame_mutex);

        char s[20];
        sprintf(s, "FPS: %lf", ids_c.getFPS());
        cv::putText(frame, s, cv::Point(752/2, 480-30), CV_FONT_HERSHEY_DUPLEX, 0.4, CV_RGB(0, 255, 0), 1.0);
        cv::imshow("fram",frame);
        if(licznik_czas > 500)
        {
            licznik_czas = 0;
            clock_gettime(CLOCK_MONOTONIC, &end);
            seconds = (end.tv_sec - start.tv_sec);
            fps  =  1 / (seconds / 500);
            std::cout <<"center_pixel: " << frame.at<int>(752/2, 480/2) << std::endl;
            std::cout <<"FPS: " << fps << std::endl;
        }
        else
        {
            licznik_czas++;
        }
        if(cv::waitKey(10)== 27)
            break;


    }
    ids_c.exit();


    return 0;
}

