#ifndef SHARED_ED_H__
#define SHARED_ED_H__


#include "opencv2/imgproc.hpp"

using namespace std;
using namespace cv;

class ED {
public:
        ED() {};
        ~ED() {};

        string version();
        int detectEvent( std::vector<Mat> &frames, string identifier, bool saveOutput, string edMaskFile);
        int detectFromFile(string filename, string identifier, bool saveOutput, string edMaskFile);
        int detectFromFileWithMask(string filename, string maskfilename, string identifier, bool saveOutput, string edMaskFile);
};

#endif
