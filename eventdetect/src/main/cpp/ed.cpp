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

void processVideoFileNoSaveOutput(string filename)
{
  ED ed;
  ed.detectFromFile(filename, "", false);
}

void processVideoFile(string filename, string identifier, bool saveOutput)
{
  ED ed;
  ed.detectFromFile(filename, identifier, saveOutput);
}

void processVideoFileWithMask(string filename, string maskfilename, string identifier, bool saveOutput)
{
  ED ed;
  ed.detectFromFileWithMask(filename, maskfilename, identifier, saveOutput);
}

int main(int argc, char** argv)
{

    // Parse command line arguments
    CommandLineParser parser(argc, argv, "{help h||}{@arg1||}{@arg2||}");
    if (parser.has("help"))
    {
        help();
        return 0;
    }
    string input = parser.get<std::string>(0);
    string mask = parser.get<std::string>(1);

    if (input.empty())
    {
        help();
        return 0;
    }

    bool testMultiThread = false;
    if (testMultiThread) {
      int numT = 100;
      while (numT > 0) {
       thread t1(processVideoFileNoSaveOutput, input);
       thread t2(processVideoFileNoSaveOutput, input);
       thread t3(processVideoFileNoSaveOutput, input);
       thread t4(processVideoFileNoSaveOutput, input);
       thread t5(processVideoFileNoSaveOutput, input);
       thread t6(processVideoFileNoSaveOutput, input);
       thread t7(processVideoFileNoSaveOutput, input);
       thread t8(processVideoFileNoSaveOutput, input);
       thread t9(processVideoFileNoSaveOutput, input);
       thread t10(processVideoFileNoSaveOutput, input);

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
    }
    else {
      if (mask.empty()) {
         processVideoFile(input, "direct", true);
      }
      else {
         processVideoFileWithMask(input,mask,"direct", true);
      }
    }

    exit(0);
}
