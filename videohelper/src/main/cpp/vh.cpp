
#include "videoHelper.h"


#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include <thread>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <string>
#include <algorithm>


static void help()
{
    cout << "Usage : " << endl << "./vh videoFilename" << endl;
}

void processVideoFile(string filename)
{
  cout << "Processing : " << filename << endl;
  VideoHelper vh;
  vh.downSample(filename, "out.mp4", 5);
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

    thread t1(processVideoFile, input);
    t1.join();

    exit(0);
}
