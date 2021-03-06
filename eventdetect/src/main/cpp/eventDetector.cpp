// Copyright (C) 2016 Satya Mallick, Big Vision LLC

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
#include <ctime>
#include <chrono>

#define ED_VERSION "v1.0.0"

//#define DEBUG
//#define DEBUG_BACKGROUND

// Max number of frames to be read.
#define MAX_FRAMES 500


// Minimum blob size for detection
#define MIN_BLOB_SIZE 200

// Background subtractor threshold
#define BACKGROUND_SUBTRACTOR_THRESHOLD 50

// If these many frames have motion terminate
#define TERMINATION_THRESHOLD MAX_FRAMES


using namespace std;
using namespace cv;


// Gets bounding boxes from foreground mask.
static void getBoundingBoxes(const Mat& img, Mat& mask, std::vector<cv::Rect> &bboxes)
{
    int niters = 2;

    vector<vector<Point> > contours;
    vector<vector<Point> > selectedContours;
    vector<Vec4i> hierarchy;

    // Morphological closing operation to fill holes in mask
    Mat temp;
    dilate(mask, temp, Mat(), Point(-1,-1), niters);
    erode(temp, temp, Mat(), Point(-1,-1), niters*2);
    dilate(temp, temp, Mat(), Point(-1,-1), niters);

    // Find contours
    findContours( temp, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE );

    if( contours.size() == 0 )
        return;

    // Iterate through all the top-level contours
    // and select contours that form big blogs
    for(int idx = 0; idx >= 0; idx = hierarchy[idx][0] )
    {
        const vector<Point>& c = contours[idx];
        double area = fabs(contourArea(Mat(c)));
        if (area > MIN_BLOB_SIZE)
        {
            selectedContours.push_back(contours[idx]);
        }

    }

    vector<vector<Point> > contoursPoly( selectedContours.size() );
    for( int i = 0; i < selectedContours.size(); i++ )
    {
        // Simplify contour using polygons
        approxPolyDP( Mat(selectedContours[i]), contoursPoly[i], 3, true );

        // Find bounding boxes.
        cv::Rect box = boundingRect( Mat(contoursPoly[i]));
        bool intersects = false;
        for(int j = 0; j < bboxes.size(); j++)
        {
            intersects = intersects | ((bboxes[j] & box).area() >  0);
            if (intersects)
            {
                bboxes[j] = bboxes[j] | box;
                box = bboxes[j];
            }
        }
        if (!intersects) bboxes.push_back(box);
    }

}

int processFrames( std::vector<Mat> &frames, int initialFrameRate, string identifier, bool saveOutput, string edMaskFile)
{
  auto start = std::chrono::steady_clock::now();
  int countFramesWithBboxes = 0;
  bool updateBackgroundModel = true;
  int blobs = 0;

  // Find frames per second.
  double fps = 15;
  Mat frame, bgMask, outFrame, edMask;

  // Make sure
  if(frames.empty())
  {
      cout << "Empty vector of frames" << endl;
      return -1;
  }

  // Background subtractor
  Ptr<BackgroundSubtractorMOG2> bgsubtractor=createBackgroundSubtractorMOG2();
  bgsubtractor->setVarThreshold(BACKGROUND_SUBTRACTOR_THRESHOLD);

  int numFrames = frames.size();
  for (int curFrame = 0; curFrame < numFrames; curFrame += max(1,numFrames/10))
  {
    frame = frames[curFrame];
    bgsubtractor->apply(frame, bgMask, updateBackgroundModel ? -1 : 0);
  }
  updateBackgroundModel = false;

  #ifdef DEBUG
    // Get character code for codec
    namedWindow("video", 1);
  #endif

  #ifdef DEBUG_BACKGROUND
    // Show background
    Mat bkgnd;
    bgsubtractor->getBackgroundImage(bkgnd);
    imshow("Background", bkgnd);
    waitKey(1);
  #endif /* DEBUG */

  if (saveOutput) {
    Mat bkgnd;
    bgsubtractor->getBackgroundImage(bkgnd);
    imwrite("./debug/ed-" + identifier + "-bg.jpg", bkgnd);
  }

  VideoWriter vidOut;
  if (saveOutput) {
     vidOut = VideoWriter("./debug/ed-" + identifier + ".mp4", VideoWriter::fourcc('X','2','6','4'), initialFrameRate, frame.size(), true);
     if (!vidOut.isOpened()) {
       cout << "ERROR: Could not open output video file" << endl;
       return 0;
     }
  }

  // Create a black mask canvas
  edMask = Mat(frame.size(), CV_8UC4);
  edMask = cv::Scalar(0,0,0,255);

  // Vector of motion bounding boxes
  std::vector<cv::Rect> bboxes;

  for (int curFrame = 0; curFrame < frames.size(); curFrame++)
  {
      // Capture frames until end of file
      frame = frames[curFrame];
      bboxes.clear();

      // Apply background subtraction.
      bgsubtractor->apply(frame, bgMask, updateBackgroundModel ? -1 : 0);

      // Remove small blobs and get bounding boxes for large ones
      getBoundingBoxes(frame, bgMask, bboxes);

      if (bboxes.size() > 0){
          countFramesWithBboxes += 1;
          blobs += bboxes.size();
#ifdef DEBUG
          cout << "Frame: " << curFrame << " No. of objects detected: " << bboxes.size() << endl;
          for(int j = 0; j < bboxes.size(); j++)
          {
              cout << bboxes[j] << endl;
          }
#endif /* DEBUG */
      }

      // Apply every "blob"/roi to the edMask as transparent
      for (int j = 0; j < bboxes.size(); j++)
      {
        rectangle( edMask, bboxes[j].tl(), bboxes[j].br(), Scalar(255,255,255,0), CV_FILLED );
      }

      if (saveOutput) {
          outFrame = frame;

          for (int j = 0; j < bboxes.size(); j++)
          {
              rectangle( outFrame, bboxes[j].tl(), bboxes[j].br(), Scalar(255,255,255), 2, 8, 0 );
          }
          vidOut << outFrame;
      }

      if ( TERMINATION_THRESHOLD > 0 && countFramesWithBboxes >= TERMINATION_THRESHOLD)
      {
  	if (!edMaskFile.empty()) imwrite(edMaskFile, edMask);
        cout << "Total frames: " << frames.size() << endl << "TERMINATION_THRESHOLD reached : " << countFramesWithBboxes << endl;
        auto end = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        cout << "It took " << elapsed.count() << " milliseconds." << endl;
        cout << "Total blobs found: " << blobs << endl;
        return TERMINATION_THRESHOLD;
      }

#ifdef DEBUG
      outFrame = frame;

      for (int j = 0; j < bboxes.size(); j++)
      {
          rectangle( outFrame, bboxes[j].tl(), bboxes[j].br(), Scalar(255,255,255), 2, 8, 0 );
      }

      imshow("video", outFrame);
      int keycode = waitKey(1);
      if( keycode == 27 ) break;
      // Manually toggle updateBackgroundModel using spacebar
      if( keycode == ' ' )
      {
          updateBackgroundModel = !updateBackgroundModel;
          printf("Learn background is in state = %d\n",updateBackgroundModel);
      }

      waitKey(5000);

#endif /* DEBUG */

  }
  cout << "Total frames: " << frames.size() << endl << "Total frames having motion: " << countFramesWithBboxes << endl;

  if (saveOutput) {
    vidOut.release();
  }

  if (!edMaskFile.empty()) imwrite(edMaskFile, edMask);

  auto end = std::chrono::steady_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  cout << "It took " << elapsed.count() << " milliseconds." << endl;
  cout << "Total blobs found: " << blobs << endl;

  return countFramesWithBboxes;

}

string ED::version() {
    return ED_VERSION;
}


int ED::detectEvent( std::vector<Mat> &frames, string identifier, bool saveOutput, string edMaskFile){
  return processFrames(frames, 5, identifier, saveOutput, edMaskFile);
}

int ED::detectFromFile(string filename, string identifier, bool saveOutput, string edMaskFile){
  cout << "Detecting events from file: [" << filename << "]" << endl;
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
  return processFrames(frames,(int) cap.get(CV_CAP_PROP_FPS), identifier, saveOutput, edMaskFile);
}

int ED::detectFromFileWithMask(string filename, string maskfilename, string identifier, bool saveOutput, string edMaskFile) {
  cout << "Detecting events from file: [" << filename << "] with mask [" << maskfilename << "]" << endl;

  Mat mask = imread(maskfilename, 0);
  if (!mask.data) {
    cout << "ERROR: Bad mask [" << maskfilename << "]" << endl;
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

  VideoCapture cap(filename);
  std::vector<Mat> frames;
  int numFrames = 0;
  while(1)
  {
    Mat frame;
    cap >> frame;
    if (frame.empty()) break;

    // Here we make sure the mask is the same size as the frame
    Mat properMask;
    resize(mask, properMask, frame.size());

    // Now we mask the frame
    Mat maskedFrame;
    frame.copyTo(maskedFrame,properMask);

    frames.push_back(maskedFrame);
    numFrames++;
    if (numFrames == MAX_FRAMES) break;
  }

  return processFrames(frames,(int) cap.get(CV_CAP_PROP_FPS), identifier, saveOutput, edMaskFile);
}

