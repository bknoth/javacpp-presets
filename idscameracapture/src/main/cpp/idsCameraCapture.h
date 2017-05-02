#ifndef SHARED_H__
#define SHARED_H__

#include <opencv/cv.h>
#include <uEye.h>

using namespace std;
using namespace cv;

class IdsCameraCapt {
public:
	IdsCameraCapt() :grabFrames(false), hCam(0) {};

	// Closes camera
	~IdsCameraCapt() {CloseCamera();};

	// Init - Open specified camera
	// IN:
	// camId  - Cam.ID displayd in IDS Camera Manager
	// iniFilePath - path to ini file used set camera parameters
	// RETURN:
	// > 0 - success, indicates image buffer size
	int Init(int camID, char * iniFilePath);

	// GetFrame - Get most recent frame from camera
	cv::Mat* GetFrameImageMat();
	IplImage* GetFrameImage();
	void SaveToDisk(const cv::Mat& frm, std::string fileName);


private:
	void CaptFrames();
	bool ConfigureCamera(std::string iniFileName);
	int GetBitsPerPixel();

	void CloseCamera();
	IplImage *pImg;
	cv::Mat *retImg;
	HIDS hCam;
	int camId;
	int imgSize;
	bool grabFrames;
	int nSizeW;
	int nSizeH;
	char nColorMode;
	int nBitsPerPixel;
	int	 lMemoryId;	// grabber memory - buffer ID
	char* pcImageMemory;// grabber memory - pointer to buffer

};

#endif
