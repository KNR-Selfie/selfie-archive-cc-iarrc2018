#include <iostream>

#include <include/ids.h>
#include <pthread.h>
void* music_thread_f(void*){
    system("canberra-gtk-play -f selfie.wav");

    system("canberra-gtk-play -f start.wav");

    system("canberra-gtk-play -f crash.wav");

    return NULL;
}
int main_test(void){
    sounds_init();
    struct timespec start, end;
    unsigned int licznik_czas = 0;
    float seconds = 0;
    float fps = 0;
    cv::Mat frame(IDS_HEIGHT, IDS_WIDTH, CV_8UC3);
    cvNamedWindow("fram",1);
    ids.init();

    while(true)
    {

        pthread_cond_wait(&algorithm_signal, &algorithm_signal_mutex);
        static int denom = 0;
        if(++denom > 1){
            denom = 0;
            if(licznik_czas == 0)
            {
                clock_gettime(CLOCK_MONOTONIC, &start);
            }

            pthread_mutex_lock(&ids.frame_mutex);
            ids.ids_frame.copyTo(frame);
            pthread_mutex_unlock(&ids.frame_mutex);

            char s[20];
            sprintf(s, "FPS: %lf", ids.getFPS());
            cv::putText(frame, s, cv::Point(IDS_WIDTH/2, IDS_HEIGHT-30), CV_FONT_HERSHEY_DUPLEX, 0.4, CV_RGB(0, 255, 0), 1.0);
            cv::imshow("fram",frame);
            if(licznik_czas > 50)
            {
                licznik_czas = 0;
                clock_gettime(CLOCK_MONOTONIC, &end);
                seconds = (end.tv_sec - start.tv_sec);
//                fps  =  10 / (seconds / 50);
                fps = ids.getFPS();
                std::cout <<"FPS: " << fps << std::endl;
            }
            else
            {
                licznik_czas++;
            }

            cv::waitKey(1);
        }



    }
    ids.exit();


    return 0;
}
void sounds_init(){
    pthread_t music_thread;
    pthread_create(&music_thread,NULL,music_thread_f,(void*)0);
}
