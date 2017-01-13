#ifndef SHARED_ED_H__
#define SHARED_ED_H__


#include "opencv2/imgproc.hpp"

using namespace std;
using namespace cv;

class ED {
public:
        ED() {};
        ~ED() {};

	int detectEvent( std::vector<Mat> &frames);
        int detectFromFile(string filename);
};

#endif