
#include "videoHelper.h"


#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <set>
#include "json/json.h"

//#define DEBUG

using namespace std;
using namespace cv;

int VideoHelper::extractFrame(string srcVid, string destImg, int frameNum) {
  VideoCapture cap(srcVid);
  Mat frame;

  int srcRate = (int) cap.get(CV_CAP_PROP_FPS);

  if (srcRate <= 0 && frameNum > 0) {
    cout << "ERROR: Can't process source as video" << endl;
    return 0;
  }

  cout << "Extracting frame num: " << frameNum << endl;

  int frameCount = 0;
  bool found = false;
  while(!found)
  {
    frameCount++;
#ifdef DEBUG
    cout << "frame num: " << frameCount << endl;
#endif // DEBUG
    cap >> frame;
    if (frame.empty()) break;
    if (frameCount == frameNum) found = true;
  }

  if (found) {
    imwrite(destImg, frame);
    return 1;
  }
  else return 0;
}


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

string VideoHelper::sizeVideo(string vidFile)
{
   VideoCapture cap(vidFile);
   return "{\"width\":" + to_string((int) cap.get(CV_CAP_PROP_FRAME_WIDTH)) + ", \"height\":" + to_string((int) cap.get(CV_CAP_PROP_FRAME_HEIGHT)) + "}";
}

string VideoHelper::sizeImage(string imgFile)
{
   Mat mask = imread(imgFile, 0);
   return "{\"width\":" + to_string((int) mask.size().width) + ", \"height\":" + to_string((int)mask.size().height) + "}";
}

string VideoHelper::detectUnmaskedROIs(int frameWidth, int frameHeight, string roiJson, string maskFile)

{
    // This can be increased for debugging/visualization. Increasing this value has an inverse affect
    // on the number of ROI's that ca be uniquely identified on the canvas
    int intensityFactor = 255;

    Mat mask = imread(maskFile, -1);
    if (!mask.data) {
        cout << "ERROR: Bad mask [" << maskFile << "]" << endl;
        return "";
    }
    else {
        // Need to invert the mask
        for( int y = 0; y < mask.rows; y++ ) {
          for( int x = 0; x < mask.cols; x++ ) {
              if (mask.at<uint>(y,x) == 0) mask.at<uint>(y,x) = 1; else mask.at<uint>(y,x) = 0;
          }
        }
    }

#ifdef DEBUG
      // Create a white roi canvas
      Mat whiteCanvas = Mat(frameHeight, frameWidth, CV_8UC4);
      whiteCanvas = cv::Scalar(255);

      // Here we make sure the mask is the same size as the roi canvas
      Mat sampleMask;
      resize(mask, sampleMask, whiteCanvas.size());

      // Now apply the mask to the canvas
      Mat sampleMaskedCanvas;
      whiteCanvas.copyTo(sampleMaskedCanvas,sampleMask);
      imshow("Default Mask", sampleMaskedCanvas);

      cout << "detectUnmaskedROIs: Size [" << frameWidth << " x " << frameHeight << "]" << endl;
#endif // DEBUG

    // This will be the final detected set of rois (indexes)
    set<int> roiSet;

    Json::Value root;
    std::istringstream iss (roiJson);
    iss >> root;

    // Draw each roi on a black canvas and then mask over it
    for ( int index = 0; index < root.size(); ++index ) {

#ifdef DEBUG
      cout << "detectUnmaskedROIs: Evaluating ROI [" << index << "]" << endl;
#endif

      // Create a black roi canvas
      Mat roiCanvas = Mat(frameHeight, frameWidth, CV_8UC4);
      roiCanvas = cv::Scalar(0);

      // Here we make sure the mask is the same size as the roi canvas
      Mat properMask;
      resize(mask, properMask, roiCanvas.size());

      Json::Value roi = root[index];
      double x = roi.get("x",0.0).asDouble();
      double y = roi.get("y",0.0).asDouble();
      double w = roi.get("w",0.0).asDouble();
      double h = roi.get("h",0.0).asDouble();

      int roiX = x * frameWidth;
      int roiY = y * frameHeight;
      int roiW = (x + w) * frameWidth;
      int roiH = (y + h) * frameHeight;

#ifdef DEBUG
      cout << "ROI: " << roi << endl;
      cout << "ROI (denormalized): [" << roiX << "," << roiY << "," << roiW << "," << roiH << "]" << endl;
#endif // DEBUG

      rectangle( roiCanvas, Point( roiX, roiY ), Point( roiW, roiH), Scalar( intensityFactor), CV_FILLED );

      Mat maskedCanvas;
      roiCanvas.copyTo(maskedCanvas,properMask);

#ifdef DEBUG
      imshow("roiCanvas", roiCanvas);
      imshow("maskedCanvas", maskedCanvas);
      int keycode = waitKey(0);
#endif // DEBUG

      // Now, we finally run a scan over the roi on maskedCanvas to find out if we "see" any white from the roi.
      bool found = false;
      for( int innery = roiY; !found && (innery < (roiY + (roiH-roiY))); innery++ ) {
        for( int innerx = roiX; !found && (innerx < (roiX + (roiW-roiX))); innerx++ ) {
          uint pixelValue = (uint)(maskedCanvas.at<uint>(innery,innerx));
          if (pixelValue > 0) {
            found = true;
            roiSet.insert(index);
          }
        }
      }

#ifdef DEBUG
      if (found) cout << "detectUnmaskedROIs: *YES* ROI [" << index << "]" << endl;
      else cout << "detectUnmaskedROIs: *NO* ROI [" << index << "]" << endl;
#endif
    }

    string arrayStr = "";
    for (set<int>::iterator it=roiSet.begin(); it!=roiSet.end(); ++it) {
            if (arrayStr.length() > 0) arrayStr += ",";
            arrayStr += to_string(*it);
    }

#ifdef DEBUG
    cout << "Final ROI Set: [" << arrayStr << "]" << endl;
#endif // DEBUG

    // Return a JSON array string
    return "{\"rois\":[" + arrayStr + "]}";
}

