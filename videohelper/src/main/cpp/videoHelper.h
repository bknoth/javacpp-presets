#ifndef SHARED_ED_H__
#define SHARED_ED_H__


#include "opencv2/imgproc.hpp"

using namespace std;
using namespace cv;

class VideoHelper {
public:
        VideoHelper() {};
        ~VideoHelper() {};

        int downSample(string src, string dest, int rate);

        string detectUnmaskedROIs(int frameWidth, int frameHeight, string roiJson, string maskFile);
};

#endif
