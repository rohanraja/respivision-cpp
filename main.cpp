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

 float approx = 0;

 Mat f ;

void collector_fastest(vector<Mat> *buffer, int buffLen, VideoCapture *capture, int *pos){

  int i = 0;

  while(1){
    (*capture) >>  (*(buffer))[i%buffLen] ;
    *pos = i%100;
    i++;
  }
}

void collector(Mat *buffer, int buffLen, VideoCapture *capture, int *pos, Mat *frame){

  int i = 0;

  Mat gray ;
  int newpos ;

  while(1){

    bool bSuccess = capture->read(f); // read a new frame from video

    if (!bSuccess) //if not success, break loop
   {
         cout << "ERROR: Cannot read a frame from video file" << endl;
         break;
    }

    *frame = f ;
    // *capture >> *frame ;
    cvtColor(*frame, gray, COLOR_BGR2GRAY);

    newpos = i%buffLen;
    uchar *arr = gray.data ;

    rep(j, gray.total())
      buffer->at<uchar>(j, newpos) = arr[j];
    
    *pos = newpos;
    // trace(newpos)
    
    i++;
  }
}

float getDominantFreqs(Mat &timeSeries, float low, float high, int Fs, Point *pt, int width){

    cv::Mat fft;

    int n = timeSeries.cols ;

    cv::dft(timeSeries, fft, DFT_ROWS);

    int low_ind = (low * n ) / Fs ;
    int high_ind = ( high * n ) / Fs ;
    
    float amp ,x,y, curMax = 0, freqMax;

    for(int j=20000; j < timeSeries.rows; j++){

     if (low_ind <= 1)
       low_ind = 2 ;
      for(int i=low_ind-1; i< high_ind+1; i++){
        x = fft.at<double>(j , 2*i - 1);
        y = fft.at<double>(j , 2*i);
        amp = x*x + y*y ;

        if(amp > curMax){
          curMax = amp;
          freqMax =  (float)(i* Fs)/(float)(n) ;
          pt->y = j / width ; 
          pt->x = j % width ; 
          // cout << *pt ;
        }

      }
    }

    return freqMax*60 ;

}

float zeroCrossFreq(Mat &y, int Fs, float fftFreq){

    int numCrossings = 0, st = -1, end;

    double s = cv::sum( y )[0];

    s /=  ((double)(y.total())) ;
    y = y - s ;
    for(int i=0; i<y.total()-1; i++){
      
      if(y.at<double>(i) < 0 && y.at<double>(i+1) >=0){
        numCrossings += 1;
        end = i;
        if(st == -1)
          st = i;
      }
    }


    numCrossings -- ;

    trace(numCrossings) ;
    if(numCrossings <= 0)
      return -1 ;

    float approxFreq = (numCrossings * (float)(Fs)) / ((float)(end - st)) ;
    
    float curBest = approxFreq;

    trace(st) ;
    trace(end) ;
    trace(approxFreq*60) ;
    
    int MAXERROR = 10 ; // PARAMETER
     int nc;
    for(int i = -1*MAXERROR; i<= MAXERROR; i++){

      nc = numCrossings + i;
      if(nc <= 0)
        continue;

      approxFreq = (nc * (float)(Fs)) / ((float)(end - st)) ;
      
      if(abs(approxFreq - fftFreq) < abs(curBest - fftFreq))
        curBest = approxFreq ;
    }

    trace(curBest*60) ;

    int MAXDEVIATION = 10 ; // PARAMETER

    if((abs(curBest - fftFreq) * 60.0 ) > MAXDEVIATION)
      return -1 ;

    return curBest ;

}

template <typename T>
cv::Mat plotGraph(std::vector<T>& vals, int YRange[2])
{

    auto it = minmax_element(vals.begin(), vals.end());
    float scale = 1./ceil(*it.second - *it.first); 
    float bias = *it.first;
    int rows = YRange[1] - YRange[0] + 1;
    cv::Mat image = Mat::zeros( rows, vals.size(), CV_8UC3 );
    image.setTo(0);
    for (int i = 0; i < (int)vals.size()-1; i++)
    {
        cv::line(image, cv::Point(i, rows - 1 - (vals[i] - bias)*scale*YRange[1]), cv::Point(i+1, rows - 1 - (vals[i+1] - bias)*scale*YRange[1]), Scalar(0, 0, 255), 1);
    }

    return image;
}

void processor(Mat *buffer, int buffLen, int *pos, float *ans, Point *pt, int width){


  int j,k,l=0 ;
     Mat subSamp ; 

     int st, end, subLen = 200 ;
     int Fs = 24 ;

  while(1){

    end = *pos ;
    st = end - subLen ;
    if (st < 0)
      st = 0;
    if (st == end)
      end ++;

    if(end < 10)
      continue ;

    subSamp = (*buffer)(Range::all(), Range(st, end)) ;

    subSamp.convertTo(subSamp, CV_64F) ;
    *ans = getDominantFreqs(subSamp, 0.1, 0.9, Fs, pt, width) ;

    trace(*ans)
    Mat ys = subSamp.row( pt->y*width + pt->x ).clone() ;

    double *y = ys.ptr<double>(0) ;
    vector<double> numbers(y, y + subLen);

    int range[2] = {0, subLen};
    Mat plotted = plotGraph( numbers , range);

    // imshow("plot" , plotted);
    // waitKey(1);
    approx = zeroCrossFreq(ys , Fs, (*ans) / 60.0) * 60.0;

    trace(*ans) ;
  
    // ans = processCV(buffer[pos-100:pos], FPS);

  }

}

void MyFilledCircle( Mat &img, Point center )
{
 int thickness = -1;
 int lineType = 8;

 circle( img,
         center,
         7,
         Scalar( 23, 23, 255 ),
         thickness,
         lineType );
}
int main(int arlen, char** argv)
{
      // VideoCapture cap(0); // open the default camera
    
    VideoCapture cap(argv[1]); // open the default camera

    if(!cap.isOpened())  // check if we succeeded
        return -1;
    Mat edges;
    // namedWindow("edges",1);

    int buffLen = 1000;

    int width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
    int height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);

    Mat buffer(width*height, buffLen , CV_8U);
    cout << sizeof(buffer) << "\n";
    // vector<Mat> buffer;

    int pos  = 0 ;
    float ans = 30 ;
    Point osciPoint ;

    Mat frame;
    thread t(collector, &buffer, buffLen, &cap, &pos, &frame);
    thread t2(processor, &buffer, buffLen, &pos, &ans, &osciPoint, width);

      waitKey(1000);

    for(;;)
    {

        if(waitKey(1) >= 0) break;
        // trace((ans));
        // TIMECH("getframe")
        // cvtColor(frame, edges, COLOR_BGR2GRAY);
        // GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5);
        // Canny(edges, edges, 0, 30, 3);
        // TIMECH("imshow")
        //
        // frame = &(buffer[pos]) ;
        // // cerr << pos << "\n";
        //
        string text1 = to_string(ans);
        string text2 = to_string(approx);
        string text = text1 + " , " + text2;
        // string text = "tst"; 
        int fontFace = FONT_HERSHEY_SCRIPT_SIMPLEX;
        double fontScale = 0.8;
        int thickness = 1;
        int baseline=0;
        Size textSize = getTextSize(text, fontFace,
                            fontScale, thickness, &baseline);
        Point textOrg((frame.cols - textSize.width)/2 - 100,
              (frame.rows + textSize.height)/2 - 100);
        putText(frame, text, textOrg, fontFace, fontScale, Scalar::all(255), thickness, 1);
        MyFilledCircle(frame, osciPoint);
        try{
          imshow("edges", frame);
        }catch(exception &e){
        }

        // TIMECH("waitkey")
    }
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}
