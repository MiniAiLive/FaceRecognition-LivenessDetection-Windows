#pragma once

#include "stdafx.h"
#include "merror.h"
#include <opencv2\opencv.hpp>
#include <vector>


#include "free/miniai_face_sdk.h"


class MiniAIFaceEngine
{
public:
	MiniAIFaceEngine();
	~MiniAIFaceEngine();

	//check activation
	MRESULT CheckActiveSDK();
	//Get the activation file information (you can get the validity period)
	MRESULT SetActiveInfo(CString license, int licenseLen);
	//Initialize the engine
	MRESULT InitEngine(MAI_DetectMode mode);	
	//release engine
	MRESULT UnInitEngine();	
	//Face Detection
	MRESULT PreDetectFace(IplImage* image, MAI_SingleFaceInfo& faceRect, bool isRGB);
	//Face Feature Extraction
	MRESULT PreExtractFeature(IplImage* image, MAI_FaceFeature& feature, MAI_SingleFaceInfo& faceRect);
	//face comparison
	MRESULT FacePairMatching(MFloat &confidenceLevel, MAI_FaceFeature feature1, MAI_FaceFeature feature2, 
		MAI_CompareModel compareModel = MAI_LIFE_PHOTO);
	//Set liveness threshold
	MRESULT SetLivenessThreshold(MFloat	rgbLiveThreshold, MFloat irLiveThreshold);
	//RGB Image face attribute detection
	MRESULT FaceMAIProcess(MAI_MultiFaceInfo detectedFaces, IplImage *img, MAI_AgeInfo &ageInfo,
		MAI_GenderInfo &genderInfo, MAI_Face3DAngle &angleInfo, MAI_LivenessInfo& liveNessInfo);
	//IR Liveness Detection
	MRESULT FaceMAIProcess_IR(MAI_MultiFaceInfo detectedFaces, IplImage *img, MAI_LivenessInfo& irLiveNessInfo);
	//Get version information
	const MAI_VERSION GetVersion();
	


private:
	MHandle m_hEngine;
};

//image cropping
void PicCutOut(IplImage* src, IplImage* dst, int x, int y);
//color space conversion
int ColorSpaceConversion(IplImage* image, MInt32 format, ASVLOFFSCREEN& offscreen);