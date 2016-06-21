#include <opencv2/core/core.hpp>
#include "opencv2/opencv.hpp"
#include <opencv2/highgui/highgui.hpp>
#include<iostream>
#include<thread>
#include<stdio.h>
#include"helpers.h"
#include <unistd.h>

using namespace cv;
using namespace std;

void collector(vector<Mat> *buffer, int buffLen, VideoCapture *capture, int *pos){

  int i = 0;

  while(1){
    (*capture) >>  (*(buffer))[i%buffLen] ;
    *pos = i%100;
    i++;
  }
}

void processor(vector<Mat> *buffer, int buffLen, int *pos, float *ans){


  int j,k,l=0 ;
  while(1){

    for(j=0; j<1000000; j++)
      for(k=0; k<1000; k++)
        l = j*k;

    *ans = (float) (*pos) ;
  
    // ans = processCV(buffer[pos-100:pos], FPS);

  }

}

int main(int arlen, char** argv)
{
      VideoCapture cap(0); // open the default camera
    
    // VideoCapture cap(argv[1]); // open the default camera

    if(!cap.isOpened())  // check if we succeeded
        return -1;
    Mat edges;
    namedWindow("edges",1);

    int buffLen = 100;

    int width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
    int height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);

    vector<Mat> buffer(buffLen ,Mat(width,height,CV_32F));
    cout << sizeof(buffer) << "\n";
    // vector<Mat> buffer;

    int pos  = 0 ;
    float ans = 30 ;

    thread t(collector, &buffer, buffLen, &cap, &pos);
    thread t2(processor, &buffer, buffLen, &pos, &ans);
    Mat *frame;
    for(;;)
    {
        if(waitKey(1) >= 0) break;
        // TIMECH("getframe")
        // cvtColor(frame, edges, COLOR_BGR2GRAY);
        // GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5);
        // Canny(edges, edges, 0, 30, 3);
        // TIMECH("imshow")
        //
        frame = &(buffer[pos]) ;
        // cerr << pos << "\n";

        string text = to_string(ans);
        // string text = "tst"; 
        int fontFace = FONT_HERSHEY_SCRIPT_SIMPLEX;
        double fontScale = 2;
        int thickness = 3;
        int baseline=0;
        Size textSize = getTextSize(text, fontFace,
                            fontScale, thickness, &baseline);
        Point textOrg((frame->cols - textSize.width)/2,
              (frame->rows + textSize.height)/2);
        putText(*frame, text, textOrg, fontFace, fontScale, Scalar::all(255), thickness, 8);
        imshow("edges", *frame);

        // TIMECH("waitkey")
    }
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}
