#pragma once
#include "stdafx.h"
#include "MiniAIFaceEngine.h"
#include <string>
#include <mutex>
#include <stdio.h>

using namespace std;

#define NSCALE 32
#define FACENUM 5

MiniAIFaceEngine::MiniAIFaceEngine()
{

}

MiniAIFaceEngine::~MiniAIFaceEngine()
{

}



MRESULT MiniAIFaceEngine::CheckActiveSDK()
{
	char hwid[256] = { 0 };
	MAIGetCurrentHWID(hwid, sizeof(hwid));

	char license[1024] = { 0 };
	FILE* fp;
	if (fopen_s(&fp, "license.txt", "rb") == 0) {
	//if (fopen_s(&fp, "license.txt", "rb") == 0) {
		int fileLen = 0;
		fseek(fp, 0, SEEK_END);
		fileLen = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		fread(license, __min(sizeof(license), fileLen), 1, fp);
		fclose(fp);
	}
	
	int ret = MAISetSerialNumber(license);
	return ret;
}

MRESULT MiniAIFaceEngine::SetActiveInfo(CString license, int licenseLen)
{
	CFile file;
	if (file.Open("license.txt", CFile::modeCreate | CFile::modeWrite))
	{
		file.Write(license, licenseLen);
		file.Close();
	}

	return MOK;
}

MRESULT MiniAIFaceEngine::InitEngine(MAI_DetectMode detectMode)
{
	m_hEngine = NULL;
	MInt32 mask = 0;

	if (MAI_DETECT_MODE_IMAGE == detectMode)
	{
		mask = MAI_FACE_DETECT | MAI_FACERECOGNITION | MAI_AGE | MAI_GENDER | MAI_FACE3DANGLE | MAI_LIVENESS | MAI_IR_LIVENESS;
	}
	else
	{
		mask = MAI_FACE_DETECT | MAI_FACERECOGNITION | MAI_LIVENESS | MAI_IR_LIVENESS;
	}

	MRESULT res = MAIInitEngine(detectMode, MAI_OP_0_ONLY, NSCALE, FACENUM, mask, &m_hEngine);
	return res;
}


MRESULT MiniAIFaceEngine::FacePairMatching(MFloat &confidenceLevel, MAI_FaceFeature feature1, 
	MAI_FaceFeature feature2, MAI_CompareModel compareModel)
{
	int res = MAIFaceFeatureCompare(m_hEngine, &feature1, &feature2, &confidenceLevel, compareModel);
	return res;
}

MRESULT MiniAIFaceEngine::SetLivenessThreshold(MFloat	rgbLiveThreshold, MFloat irLiveThreshold)
{
	MAI_LivenessThreshold threshold = { 0 };

	threshold.thresholdmodel_BGR = rgbLiveThreshold;
	threshold.thresholdmodel_IR = irLiveThreshold;

	int res = MAISetLivenessParam(m_hEngine, &threshold);
	return res;
}

MRESULT MiniAIFaceEngine::FaceMAIProcess(MAI_MultiFaceInfo detectedFaces, IplImage *img, MAI_AgeInfo &ageInfo,
	MAI_GenderInfo &genderInfo, MAI_Face3DAngle &angleInfo, MAI_LivenessInfo& liveNessInfo)
{
	if (!img)
	{
		return -1;
	}

	MInt32 lastMask = MAI_AGE | MAI_GENDER | MAI_FACE3DANGLE | MAI_LIVENESS;
	IplImage* cutImg = cvCreateImage(cvSize(img->width - (img->width % 4), img->height), IPL_DEPTH_8U, img->nChannels);
	PicCutOut(img, cutImg, 0, 0);

	ASVLOFFSCREEN offscreen = { 0 };
	ColorSpaceConversion(cutImg, ASVL_PAF_RGB24_B8G8R8, offscreen);
	if (!cutImg)
	{
		cvReleaseImage(&cutImg);
		return -1;
	}

	int res = MAIProcessEx(m_hEngine, &offscreen, &detectedFaces, lastMask);
	res = MAIGetAge(m_hEngine, &ageInfo);
	res = MAIGetGender(m_hEngine, &genderInfo);
	res = MAIGetFace3DAngle(m_hEngine, &angleInfo);
	res = MAIGetLivenessScore(m_hEngine, &liveNessInfo);
	cvReleaseImage(&cutImg);
	return res;
}

MRESULT MiniAIFaceEngine::FaceMAIProcess_IR(MAI_MultiFaceInfo detectedFaces, IplImage *img, MAI_LivenessInfo& irLiveNessInfo)
{
	if (!img)
	{
		return -1;
	}

	cv::Mat grayMat;
	cv::Mat matImg(img, false);						//IplImage to Mat is set to ture as deep copy
	cv::cvtColor(matImg, grayMat, CV_BGR2GRAY);
	IplImage* iplGrayMat = &IplImage(grayMat);		//mat to IplImage shallow copy

	MInt32 lastMask = MAI_IR_LIVENESS;
	IplImage* cutGrayImg = cvCreateImage(cvSize(iplGrayMat->width - (iplGrayMat->width % 4), iplGrayMat->height), 
		IPL_DEPTH_8U, iplGrayMat->nChannels);
	PicCutOut(iplGrayMat, cutGrayImg, 0, 0);

	ASVLOFFSCREEN offscreen = { 0 };
	ColorSpaceConversion(cutGrayImg, ASVL_PAF_GRAY, offscreen);
	if (!cutGrayImg)
	{
		cvReleaseImage(&cutGrayImg);
		return -1;
	}

	int res = MAIProcessEx_IR(m_hEngine, &offscreen, &detectedFaces, lastMask);
	res = MAIGetLivenessScore_IR(m_hEngine, &irLiveNessInfo);
	cvReleaseImage(&cutGrayImg);
	return res;
}

const MAI_VERSION MiniAIFaceEngine::GetVersion()
{
	const MAI_VERSION pVersionInfo = MAIGetVersion();
	return pVersionInfo;
}

MRESULT MiniAIFaceEngine::PreDetectFace(IplImage* image, MAI_SingleFaceInfo& faceRect, bool isRGB)
{
	if (!image)
	{
		return -1;
	}

	IplImage* cutImg = NULL;
	MRESULT res = MOK;
	MAI_MultiFaceInfo detectedFaces = { 0 };//Face Detection
	
	if (isRGB)
	{
		cutImg = cvCreateImage(cvSize(image->width - (image->width % 4), image->height),
			IPL_DEPTH_8U, image->nChannels);

		PicCutOut(image, cutImg, 0, 0);

		ASVLOFFSCREEN offscreen = { 0 };
		ColorSpaceConversion(cutImg, ASVL_PAF_RGB24_B8G8R8, offscreen);

		res = MAIDetectFacesEx(m_hEngine, &offscreen, &detectedFaces);
	}
	else  //IR Image
	{
		cv::Mat grayMat;
		cv::Mat matImg(image, false);						//IplImage to Mat is set to ture as deep copy
		cv::cvtColor(matImg, grayMat, CV_BGR2GRAY);
		IplImage* iplGrayMat = &IplImage(grayMat);		//mat to IplImage shallow copy

		cutImg = cvCreateImage(cvSize(iplGrayMat->width - (iplGrayMat->width % 4), iplGrayMat->height),
			IPL_DEPTH_8U, iplGrayMat->nChannels);


		PicCutOut(iplGrayMat, cutImg, 0, 0);

		ASVLOFFSCREEN offscreen = { 0 };
		ColorSpaceConversion(cutImg, ASVL_PAF_GRAY, offscreen);

		res = MAIDetectFacesEx(m_hEngine, &offscreen, &detectedFaces);
	}

	if (res != MOK || detectedFaces.faceNum < 1)
	{
		cvReleaseImage(&cutImg);
		return -1;
	}

	int max = 0;
	int maxArea = 0;

	for (int i = 0; i < detectedFaces.faceNum; i++)
	{
		if (detectedFaces.faceRect[i].left < 0)
			detectedFaces.faceRect[i].left = 10;
		if (detectedFaces.faceRect[i].top < 0)
			detectedFaces.faceRect[i].top = 10;
		if (detectedFaces.faceRect[i].right < 0 || detectedFaces.faceRect[i].right > cutImg->width)
			detectedFaces.faceRect[i].right = cutImg->width - 10;
		if (detectedFaces.faceRect[i].bottom < 0 || detectedFaces.faceRect[i].bottom > cutImg->height)
			detectedFaces.faceRect[i].bottom = cutImg->height - 10;

		if ((detectedFaces.faceRect[i].right - detectedFaces.faceRect[i].left)*
			(detectedFaces.faceRect[i].bottom - detectedFaces.faceRect[i].top) > maxArea)
		{
			max = i;
			maxArea = (detectedFaces.faceRect[i].right - detectedFaces.faceRect[i].left)*
				(detectedFaces.faceRect[i].bottom - detectedFaces.faceRect[i].top);
		}
	}

	faceRect.faceRect.left = detectedFaces.faceRect[max].left;
	faceRect.faceRect.top = detectedFaces.faceRect[max].top;
	faceRect.faceRect.right = detectedFaces.faceRect[max].right;
	faceRect.faceRect.bottom = detectedFaces.faceRect[max].bottom;
	faceRect.faceOrient = detectedFaces.faceOrient[max];
	cvReleaseImage(&cutImg);

	return res;
}


// Pre-extract facial features
MRESULT MiniAIFaceEngine::PreExtractFeature(IplImage* image, MAI_FaceFeature& feature, MAI_SingleFaceInfo& faceRect)
{
	if (!image || image->imageData == NULL)
	{
		return -1;
	}
	IplImage* cutImg = cvCreateImage(cvSize(image->width - (image->width % 4), image->height),
		IPL_DEPTH_8U, image->nChannels);
	PicCutOut(image, cutImg, 0, 0);
	if (!cutImg)
	{
		cvReleaseImage(&cutImg);
		return -1;
	}

	MRESULT res = MOK;
	MAI_FaceFeature detectFaceFeature = { 0 };//feature
	ASVLOFFSCREEN offscreen = { 0 };
	ColorSpaceConversion(cutImg, ASVL_PAF_RGB24_B8G8R8, offscreen);
	res = MAIFaceFeatureExtractEx(m_hEngine, &offscreen, &faceRect, &detectFaceFeature);

	if (MOK != res)
	{
		cvReleaseImage(&cutImg);
		return res;
	}

	if (!feature.feature)
	{
		return -1;
	}
	memset(feature.feature, 0, detectFaceFeature.featureSize);
	memcpy(feature.feature, detectFaceFeature.feature, detectFaceFeature.featureSize);
	cvReleaseImage(&cutImg);

	return res;
}

MRESULT MiniAIFaceEngine::UnInitEngine()
{
	//destroy engine
	return MAIUninitEngine(m_hEngine);
}

void PicCutOut(IplImage* src, IplImage* dst, int x, int y)
{
	if (!src || !dst)
	{
		return;
	}

	CvSize size = cvSize(dst->width, dst->height);//area size
	cvSetImageROI(src, cvRect(x, y, size.width, size.height));//set source image ROI
	cvCopy(src, dst); //copy image
	cvResetImageROI(src);//When the source image is used up, empty ROI
}

//color space conversion
int ColorSpaceConversion(IplImage* image, MInt32 format, ASVLOFFSCREEN& offscreen)
{
	switch (format)
	{
	case ASVL_PAF_RGB24_B8G8R8:
		offscreen.u32PixelArrayFormat = (unsigned int)format;
		offscreen.i32Width = image->width;
		offscreen.i32Height = image->height;
		offscreen.pi32Pitch[0] = image->widthStep;
		offscreen.ppu8Plane[0] = (MUInt8*)image->imageData;
		break;
	case ASVL_PAF_GRAY:
		offscreen.u32PixelArrayFormat = (unsigned int)format;
		offscreen.i32Width = image->width;
		offscreen.i32Height = image->height;
		offscreen.pi32Pitch[0] = image->widthStep;
		offscreen.ppu8Plane[0] = (MUInt8*)image->imageData;
		break;
	default:
		return 0;
	}
	return 1;
}