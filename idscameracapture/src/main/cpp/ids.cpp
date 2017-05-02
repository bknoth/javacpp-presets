#include "idsCameraCapture.h"
#include <opencv2/highgui/highgui.hpp>

#define USE_MAT 1

#include <iostream>

int main() {

	int camId = 1;
	char * camIniFilePath = "camConfig.ini";

	IdsCameraCapt cam;
	int imgSize = 0;
	if ((imgSize = cam.Init(camId, camIniFilePath)) <= 0) {
		return 0;
	}
	std::cout << "Camera " << camId << " initialized.  Image size = " << imgSize << std::endl;

	cvNamedWindow("Test CamCapture", 1);
	cv::Mat* img;
	IplImage* iplImg;
	for (int j = 0; j < 5000; j++) {
		// now you can use your img just like a normal OpenCV image
#ifdef USE_MAT
		img = cam.GetFrameImageMat();
		if (img != 0) {
			//cam.SaveToDisk(img, "testImg.jpg");
			//cvShowImage("A", img);
			cv::imshow("Test CamCapture", *img);
#else
		iplImg = cam.GetFrameImage();
		if (iplImg != 0) {
	//		cam.SaveToDisk(img, "testImg.jpg");
			cvShowImage("Test CamCapture", iplImg);
#endif
		}
		else
			return 0;

		cv::waitKey(1);
	}

	return 1;
}

