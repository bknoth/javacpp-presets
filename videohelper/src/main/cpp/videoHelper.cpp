
#include "videoHelper.h"


#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>


//#define DEBUG

using namespace std;
using namespace cv;

int VideoHelper::downSample(string src, string dest, int rate) {
  VideoCapture cap(src);
  std::vector<Mat> frames;

  int srcRate = (int) cap.get(CV_CAP_PROP_FPS);
  int destRate = rate;
  if (srcRate < destRate) destRate = srcRate;

  if (destRate <= 0) {
    cout << "ERROR: Destination FPS is 0" << endl;
    return 0;
  }
  
  int frameSkip = (int)(srcRate / destRate);

  cout << "Src FPS : " << srcRate << endl;
  cout << "Dest FPS : " << destRate << endl;
  cout << "Frame Skip : " << frameSkip << endl;

  int frameCount = 0;
  while(1)
  {
    if ((frameCount % frameSkip) == 0) {
      Mat frame;
      cap >> frame;
      if (frame.empty()) break;
      frames.push_back(frame);
    }
    frameCount++;
  }

  if (frames.empty()) {
    return 0;
  }

  Size size = Size((int) cap.get(CV_CAP_PROP_FRAME_WIDTH), (int) cap.get(CV_CAP_PROP_FRAME_HEIGHT));
  
  VideoWriter vidOut(dest, VideoWriter::fourcc('X','2','6','4'), rate, size, true);
  if (!vidOut.isOpened()) {
    return 0;
  }
  else {
    int numFrames = frames.size();
    for (int curFrame = 0; curFrame < numFrames; curFrame += max(1,numFrames/10))
    {
      Mat frame = frames[curFrame];
      vidOut << frame;
    }
    vidOut.release();
    return numFrames;
  }
}

