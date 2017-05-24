
#include "videoHelper.h"


#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "json/json.h"
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

int processExtract(string video, string img, int fn)
{
  cout << "Extracting : " << video << endl;
  VideoHelper vh;
  return vh.extractFrame(video, img, fn);
}

string processVideoSize(string video)
{
  VideoHelper vh;
  return vh.sizeVideo(video);
}

string processImageSize(string img)
{
  VideoHelper vh;
  return vh.sizeImage(img);
}

int processDetectROIMask(int w, int h, string roiJson, string maskFilename)
{
  cout << "processDetectROIMask : " << maskFilename << endl;
  VideoHelper vh;
  string jsonResult = vh.detectUnmaskedROIs(w, h, roiJson, maskFilename);
  if (jsonResult.length() > 0) {
   Json::Value root;
    std::istringstream iss (jsonResult);
    iss >> root;
    Json::Value rois = root["rois"];
    return rois.size();
  }
  else return 0;
}

void downSampleTest(int argc, char** argv)
{
   // Parse command line arguments
    CommandLineParser parser(argc, argv, "{help h||}{@input||}");
    if (parser.has("help"))
    {
        help();
        return;
    }
    string input = parser.get<std::string>("@input");

    if (input.empty())
    {
        help();
        return;
    }

    Json::Value root;
    string inJson = input;
    cout << "Input: " << inJson << endl;
    std::istringstream iss (inJson);
    iss >> root;

    Json::Value files = root["files"];
    for ( int index = 0; index < files.size(); ++index ) {
      Json::Value data = files[index];
      string filename = data.get("name","NO_FILE_GIVEN").asString();
      int value = data.get("val",0).asInt();
      std:cout << "Json: filename:" << filename << " value:" << value << endl;
      thread t1(processVideoFile, filename);
      t1.join();
    }
}

void extractTest(int argc, char** argv)
{
    int extract = processExtract("resources/vid.avi", "resources/extract.jpg", 24);
    cout << "Extracted : " << extract << endl;
}

void unmaskedROIDetectionTest(int argc, char** argv)
{
    cout << "unmaskedROIDetectionTest..." << endl;

    string inJson = "resources/rois.json";
    cout << "Input: " << inJson << endl;
    ifstream jsonFile( inJson );
    std::stringstream buffer;
    buffer << jsonFile.rdbuf();

    int detected = processDetectROIMask(800, 600, buffer.str(), "resources/precheck6335675121596400997combined.png");
//    int detected = processDetectROIMask(800, 600, buffer.str(), "resources/mask-complex.png");
    cout << "Rois detected: " << detected << endl;
}

int main(int argc, char** argv)
{
    // downSampleTest(argc, argv);

    unmaskedROIDetectionTest(argc, argv);

    // extractTest(argc,argv);

    //cout << "Video size: " << processVideoSize("resources/vid.avi") << endl;
    //cout << "Video size: " << processVideoSize("resources/imghelper8446270497878802808.mp4") << endl;
    //cout << "Image size: " << processImageSize("resources/mask1.png") << endl;

    exit(0);
}
