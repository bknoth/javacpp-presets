// Copyright (C) 2016 Satya Mallick, Big Vision LLC


// Max number of frames to be read.
#define MAX_FRAMES 500


#include "eventDetector.h"


#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/video/tracking.hpp"
#include "opencv2/video/background_segm.hpp"
#include <thread>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <string>
#include <algorithm>


static void help()
{
    cout << "Usage : " << endl << "./tracker videoFilename" << endl;
}


void processVideoFile(string filename)
{
  cout << "Processing : " << filename << endl;
  VideoCapture cap(filename);
  std::vector<Mat> frames;
  int numFrames = 0;
  while(1)
  {
    //cout << "Frame : " << numFrames << endl;
    Mat frame;
    cap >> frame;
    if (frame.empty()) break;
    frames.push_back(frame);
    numFrames++;
    if (numFrames == MAX_FRAMES) break;
  }
  ED ed;
  ed.detectEvent(frames);
}

int main(int argc, char** argv)
{

    // Parse command line arguments
    CommandLineParser parser(argc, argv, "{help h||}{@input||}");
    if (parser.has("help"))
    {
        help();
        return 0;
    }
    string input = parser.get<std::string>("@input");



    if (input.empty())
    {
        help();
        return 0;
    }

    int numT = 100;
    while (numT > 0) {
       thread t1(processVideoFile, input);
       thread t2(processVideoFile, input);
       thread t3(processVideoFile, input);
       thread t4(processVideoFile, input);
       thread t5(processVideoFile, input);
       thread t6(processVideoFile, input);
       thread t7(processVideoFile, input);
       thread t8(processVideoFile, input);
       thread t9(processVideoFile, input);
       thread t10(processVideoFile, input);

       t1.join();
       t2.join();
       t3.join();
       t4.join();
       t5.join();
       t6.join();
       t7.join();
       t8.join();
       t9.join();
       t10.join();

       numT--;
    }

    exit(0);
}
