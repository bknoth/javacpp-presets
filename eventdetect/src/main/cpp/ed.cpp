// Copyright (C) 2016 Satya Mallick, Big Vision LLC


// Max number of frames to be read.
#define MAX_FRAMES 500


#include "eventDetector.h"


#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/video/tracking.hpp"
#include "opencv2/video/background_segm.hpp"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>


static void help()
{
    cout << "Usage : " << endl << "./tracker videoFilename" << endl;
}


int processVideoFile(std::string &filename)
{
  VideoCapture cap(filename);
  std::vector<Mat> frames;
  int numFrames = 0;
  while(1)
  {
    Mat frame;
    cap >> frame;
    if (frame.empty()) break;
    frames.push_back(frame);
    numFrames++;
    if (numFrames == MAX_FRAMES) break;
  }
  ED ed;
  return ed.detectEvent(frames);

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

    processVideoFile(input);


    return 0;
}
