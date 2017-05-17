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
    cout << "Usage : " << endl << "./ed videoFilename [maskFileName]" << endl;
}

void processVideoFileNoSaveOutput(string filename, string edMaskFile)
{
  ED ed;
  ed.detectFromFile(filename, "", false, edMaskFile);
}

void processVideoFile(string filename, string identifier, bool saveOutput, string edMaskFile)
{
  ED ed;
  ed.detectFromFile(filename, identifier, saveOutput, edMaskFile);
}

void processVideoFileWithMask(string filename, string maskfilename, string identifier, bool saveOutput, string edMaskFile)
{
  ED ed;
  ed.detectFromFileWithMask(filename, maskfilename, identifier, saveOutput, edMaskFile);
}

void processVersion()
{
  ED ed;
  cout << "EventDetector Version: " << ed.version() << endl;
}


int main(int argc, char** argv)
{
    processVersion();

    // Parse command line arguments
    CommandLineParser parser(argc, argv, "{help h||}{@arg1||}{@arg2||}");
    if (parser.has("help"))
    {
        help();
        return 0;
    }
    string input = parser.get<std::string>(0);
    string mask = parser.get<std::string>(1);
    string edMaskFile = "debug/ed-mask-gen.png";

    if (input.empty())
    {
        help();
        return 0;
    }

    bool testMultiThread = false;
    if (testMultiThread) {
      int numT = 100;
      while (numT > 0) {
       thread t1(processVideoFileNoSaveOutput, input, edMaskFile);
       thread t2(processVideoFileNoSaveOutput, input, edMaskFile);
       thread t3(processVideoFileNoSaveOutput, input, edMaskFile);

       t1.join();
       t2.join();
       t3.join();

       numT--;
      }
    }
    else {
      if (mask.empty()) {
         processVideoFile(input, "direct", true, edMaskFile);
      }
      else {
         processVideoFileWithMask(input,mask,"direct", true, edMaskFile);
      }
    }

    exit(0);
}
