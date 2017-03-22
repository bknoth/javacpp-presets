
#include "videoHelper.h"


#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include "json/json.h"

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

int VideoHelper::detectUnmaskedROIs(int frameWidth, int frameHeight, string roiJson, string maskFile)

{
    Mat mask = imread(maskFile, 0);
    if (!mask.data) {
        cout << "ERROR: Bad mask [" << maskFile << "]" << endl;
        return -1;
    }
    else {
        // Need to invert the mask
        for( int y = 0; y < mask.rows; y++ ) {
          for( int x = 0; x < mask.cols; x++ ) {
            if (mask.at<uchar>(y,x) == 0) mask.at<uchar>(y,x) = 1; else mask.at<uchar>(y,x) = 0;
          }
        }
    }

    // Create a black roi canvas
    Mat roiCanvas = Mat(frameHeight, frameWidth, CV_8U);
    roiCanvas = cv::Scalar(0);

    // Here we make sure the mask is the same size as the roi canvas
    Mat properMask;
    resize(mask, properMask, roiCanvas.size());

    // Draw all the rois on the black canvas as filled, white regions
    Json::Value root;
    std::istringstream iss (roiJson);
    iss >> root;

    for ( int index = 0; index < root.size(); ++index ) {
      Json::Value roi = root[index];
      double x = roi.get("x",0.0).asDouble();
      double y = roi.get("y",0.0).asDouble();
      double w = roi.get("w",0.0).asDouble();
      double h = roi.get("h",0.0).asDouble();
#ifdef DEBUG
      std:cout << "ROI: " << roi << endl;
#endif // DEBUG

       rectangle( roiCanvas,
           Point( x * frameWidth, y * frameHeight ),
           Point( (x + w) * frameWidth, (y + h) * frameHeight),
           Scalar( 255, 255, 255 ),
           CV_FILLED );
    }

    Mat maskedCanvas;
    roiCanvas.copyTo(maskedCanvas,properMask);

#ifdef DEBUG
      imshow("roiCanvas", roiCanvas);
      imshow("maskedCanvas", maskedCanvas);
      int keycode = waitKey(0);
#endif // DEBUG

    // Now, we finally run a scan over the maskedCanvas to find out if we "see" any white from the rois.
    // If so, then it means part or all of the roi is within the unmasked region(s). Since we don't care
    // right now about which or how many rois are visible in the unmasked regions, we will stop as soon
    // as we find any pixel that is white
    for( int y = 0; y < maskedCanvas.rows; y++ ) {
      for( int x = 0; x < maskedCanvas.cols; x++ ) {
        if (maskedCanvas.at<uchar>(y,x) != 0) return 1;
      }
    }

    return 0;
}

