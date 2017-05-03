#include "idsCameraCapture.h"
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>

using namespace cv;
using namespace std;

const int RGB8_BITS_PER_PIXEL = 24;

void IdsCameraCapt::CaptFrames()
{

}

int  IdsCameraCapt::Init(int camID, char * iniFilePath)
{
	std::cout << "IdsCameraCapt::Init - Initializing " << camID << "..." << std::endl;

	if (camID < 0) {
		std::cout << "IdsCameraCapt::Init - Camera, bad camId " << camId << " initialization failed" << std::endl;
		return 0;
	}

	hCam = camID;
	int status = 0;
	if ((status = is_InitCamera(&hCam, NULL)) != IS_SUCCESS) {
		std::cout << "IdsCameraCapt::Init - Camera " << camID << " is_InitCamera failed, status = " << status << std::endl;
		return 0;
	}

	std::cout << "IdsCameraCapt::Init - Configuring " << camID << "..." << std::endl;

	if (!ConfigureCamera(iniFilePath)) {
		std::cout << "IdsCameraCapt::ConfigureCamera - failed to init camera" << std::endl;
		return 0;
	}

	pImg = 0;
	retImg = 0;
	camId = camID;
	imgSize = nSizeW * nSizeH;

	std::cout << "IdsCameraCapt::Init - Camera " << camId << " initialized.  Image size = " << imgSize  << "INI file: " << iniFilePath << std::endl;
	std::cout << "IdsCameraCapt::Init - Camera " << camId << " initialized.  Image width= " << nSizeW  << " Image height=" << nSizeH << std::endl;

	int nRet = 0;
	if (IS_SUCCESS != (nRet = is_CaptureVideo(hCam, IS_WAIT))) {
		return 0;
	} else {
        std::cout << "IdsCameraCapt::Init - Camera Capture Video OK!" << std::endl;
	}

	std::cout << "IdsCameraCapt::Init - Camera is in FreeRun mode - capture ON" << std::endl;

	return 1;
}

IplImage* IdsCameraCapt::GetFrameImage()
{
       int nRet = 0;

       // Throws exception if enabled
       if (pImg != 0) cvReleaseImage(&pImg);

      	cv::Mat* tmpImg = GetFrameImageMat();
	pImg = cvCreateImage(cvSize(tmpImg->cols,tmpImg->rows),8,3);
	IplImage ipltemp=*tmpImg;
	cvCopy(&ipltemp,pImg);
       	return pImg;
}

cv::Mat* IdsCameraCapt::GetFrameImageMat()
{
	int nRet = 0;
	if (retImg == 0) {
          retImg = new cv::Mat(nSizeH, nSizeW, CV_8UC3);
	}

	//retImg -> create(nSizeH, nSizeW, CV_8UC3);

	// Grab frame from camera
	void *pMemVoid; //pointer to where the image is stored
	if (IS_SUCCESS != (nRet = is_GetImageMem(hCam, &pMemVoid))) {
		std::cout << "IdsCameraCapt::GetFrameImageMat - ERROR [" << nRet << "]" << std::endl;
	}

	int imgSizeBytes = nSizeW * nSizeH * (nBitsPerPixel / 8);
	memcpy(retImg -> data, pMemVoid, imgSizeBytes);

	return retImg;
}

bool IdsCameraCapt::ConfigureCamera(string iniFileName)
{
	int nRet = 0;

	// Load camera parameters from ini file which is created by IDS app
	// Convert string to Unicode
	std::wstring iniFileNameW = L"/home/mosaiq/mosaiqjr-ids.ini";
	//std::wstring stemp = std::wstring(iniFileName.begin(), iniFileName.end());
	//LPCWSTR iniFileNameW = stemp.c_str();

	if (IS_SUCCESS != (nRet = is_ParameterSet(hCam, IS_PARAMETERSET_CMD_LOAD_FILE, (void*)iniFileNameW.c_str(), NULL))) {
		////LOG_STATUS(format("ERROR: is_ParameterSet IS_PARAMETERSET_CMD_LOAD_FILE failed, nRet = %d") % nRet);
		return false;
	}

	////LOG_STATUS(format("Loaded camera INI file, %s") % iniFileName.c_str());

	is_SetColorMode(hCam, IS_CM_RGB8_PACKED);


	// Verify color mode
	int colorMode = is_SetColorMode(hCam, IS_GET_COLOR_MODE);
	////LOG_STATUS(format("IS_GET_COLOR_MODE, colorMode = %d") % colorMode);
	 nBitsPerPixel = GetBitsPerPixel();


	// Get sensor info
	SENSORINFO snrInfo;
	if (IS_SUCCESS != (nRet = is_GetSensorInfo(hCam, &snrInfo))) {
		////LOG_STATUS(format("ERROR: is_GetSensorInfo failed, nRet = %d") % nRet);
		return false;
	}
	nColorMode = snrInfo.nColorMode;

	// Get image size
	IS_SIZE_2D imgSize;
	is_AOI(hCam, IS_AOI_IMAGE_GET_SIZE, &imgSize, sizeof(imgSize));
	nSizeW = imgSize.s32Width;
	nSizeH = imgSize.s32Height;

	// Set frame rate
	double actFrameRate;
	int fps = is_SetFrameRate(hCam, IS_GET_FRAMERATE, &actFrameRate);

	////LOG_STATUS(format("Camera Width %d , height %d, colormode %d, bitsperpixel %d, actFrameRate %f") % m_nSizeX % m_nSizeY % m_nColorMode % bpp % actFrameRate);

	// memory initialization
	if (IS_SUCCESS != (nRet = is_AllocImageMem(hCam,
		nSizeW,
		nSizeH,
		nBitsPerPixel,
		&pcImageMemory,
		&lMemoryId))) {
		////LOG_STATUS(format("ERROR: is_AllocImageMem failed, nRet = %d") % nRet);
		return false;
	}

	if (IS_SUCCESS != (nRet = is_SetImageMem(hCam, pcImageMemory, lMemoryId))) {
		////LOG_STATUS(format("ERROR: is_SetImageMem failed, nRet = %d") % nRet);
		return false;
	}

	// Captures an image in system memory (RAM).
	nRet = is_SetDisplayMode(hCam, IS_SET_DM_DIB);

	// disable the dialog based error report
	nRet = is_SetErrorReport(hCam, IS_DISABLE_ERR_REP); // IS_DISABLE_ERR_REP or IS_ENABLE_ERR_REP
	if (nRet != IS_SUCCESS)
	{
		////LOG_STATUS(format("ERROR: Can not enable the automatic uEye error report!"));
		return false;
	}

	return true;
}

void IdsCameraCapt::CloseCamera()
{
	// Stop frame capture
	if (grabFrames) {
		grabFrames = false;
//		procFrames.join();
	}

	if (!hCam) {
		is_FreeImageMem(hCam, pcImageMemory, lMemoryId);
		is_ExitCamera(hCam);
	}


	std::cout << "IdsCameraCapt::CloseCamera - Closed [" << camId << "]" << std::endl;

}

int IdsCameraCapt::GetBitsPerPixel()
{
	int bpp = 0;
	switch (is_SetColorMode(hCam, IS_GET_COLOR_MODE))
	{
	case IS_CM_RGBA12_UNPACKED:
	case IS_CM_BGRA12_UNPACKED:
		bpp = 64;
		break;

	case IS_CM_RGB12_UNPACKED:
	case IS_CM_BGR12_UNPACKED:
	case IS_CM_RGB10_UNPACKED:
	case IS_CM_BGR10_UNPACKED:
		bpp = 48;
		break;

	case IS_CM_RGBA8_PACKED:
	case IS_CM_BGRA8_PACKED:
	case IS_CM_RGB10_PACKED:
	case IS_CM_BGR10_PACKED:
	case IS_CM_RGBY8_PACKED:
	case IS_CM_BGRY8_PACKED:
		bpp = 32;
		break;

	case IS_CM_RGB8_PACKED:
	case IS_CM_BGR8_PACKED:
		bpp = 24;
		break;

	case IS_CM_BGR565_PACKED:
	case IS_CM_UYVY_PACKED:
	case IS_CM_CBYCRY_PACKED:
		bpp = 16;
		break;

	case IS_CM_BGR5_PACKED:
		bpp = 15;
		break;

	case IS_CM_MONO16:
	case IS_CM_SENSOR_RAW16:
	case IS_CM_MONO12:
	case IS_CM_SENSOR_RAW12:
	case IS_CM_MONO10:
	case IS_CM_SENSOR_RAW10:
		bpp = 16;
		break;

	case IS_CM_RGB8_PLANAR:
		bpp = 24;
		break;

	case IS_CM_MONO8:
	case IS_CM_SENSOR_RAW8:
	default:
		bpp = 8;
		break;
	}
	return bpp;
}

void IdsCameraCapt::SaveToDisk(const cv::Mat& frm, string fileName)
{
	// imwrite expects BGR
	cv::Mat bgr;
	cv::cvtColor(frm, bgr, CV_RGB2BGR);

	cv::imwrite(fileName, bgr); // A JPG FILE IS BEING SAVED
}
