/*******************************************************************************
* Copyright(c) MiniAI, All right reserved.
*
* This file is MiniAI's property. It contains MiniAI's trade secret, proprietary
* and confidential information.
*
* DO NOT DISTRIBUTE, DO NOT DUPLICATE OR TRANSMIT IN ANY FORM WITHOUT PROPER
* AUTHORIZATION.
*
* If you are not an intended recipient of this file, you must not copy,
* distribute, modify, or take any action in reliance on it.
*
* If you have received this file in error, please immediately notify MiniAI and
* permanently delete the original and any copy of any file and any printout
* thereof.
*********************************************************************************/

#ifndef _MINIAI_SDK_MAI_H_
#define _MINIAI_SDK_MAI_H_

#include "amcomdef.h"
#include "asvloffscreen.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAI_NONE				0x00000000	//No attribute
#define MAI_FACE_DETECT			0x00000001	//Here detect can be one of the two engines of tracking or detection, the specific choice is determined by the detect mode
#define MAI_FACERECOGNITION		0x00000004	//Facial features
#define MAI_AGE					0x00000008	//Age
#define MAI_GENDER				0x00000010	//Gender
#define MAI_FACE3DANGLE			0x00000020	//3D angle
#define MAI_LIVENESS			0x00000080	//RGB liveness
#define MAI_IR_LIVENESS			0x00000400	//IR liveness



	//detection mode
	enum MAI_DetectMode{
		MAI_DETECT_MODE_VIDEO = 0x00000000,		//Video mode, generally used for multi-frame continuous detection
		MAI_DETECT_MODE_IMAGE = 0xFFFFFFFF		//Image mode, generally used for single detection of static images
	};


	//The priority of the face angle during detection is illustrated in the initialization interface in the document, please refer to
	enum MAI_OrientPriority {
		MAI_OP_0_ONLY = 0x1,		// Normal preview positive direction
		MAI_OP_90_ONLY = 0x2,		// Orientation rotated 90° counterclockwise based on 0°
		MAI_OP_270_ONLY = 0x3,		// Orientation rotated 270° counterclockwise based on 0°
		MAI_OP_180_ONLY = 0x4,		// Rotate 180° based on 0° (counterclockwise and clockwise have the same effect)
		MAI_OP_ALL_OUT = 0x5		// full angle
	};

	//orientation angle, counterclockwise
	enum MAI_OrientCode {
		MAI_OC_0 = 0x1,			// 0 degree 
		MAI_OC_90 = 0x2,		// 90 degree 
		MAI_OC_270 = 0x3,		// 270 degree 
		MAI_OC_180 = 0x4,   	// 180 degree 
		MAI_OC_30 = 0x5,		// 30 degree 
		MAI_OC_60 = 0x6,		// 60 degree 
		MAI_OC_120 = 0x7,		// 120 degree 
		MAI_OC_150 = 0x8,		// 150 degree 
		MAI_OC_210 = 0x9,		// 210 degree 
		MAI_OC_240 = 0xa,		// 240 degree 
		MAI_OC_300 = 0xb,		// 300 degree 
		MAI_OC_330 = 0xc		// 330 degree 
	};

	//detection model
	enum MAI_DetectModel {
		MAI_DETECT_MODEL_RGB = 0x1	//RGB image detection model
		//Reserved to expand other detection models
	};

	//Optional models for face comparison
	enum MAI_CompareModel{
		MAI_LIFE_PHOTO = 0x1,	//For feature comparison between life photos, the recommended threshold is 0.80
		MAI_ID_PHOTO = 0x2		//Used for feature comparison between ID photos or life photos and ID photos, the recommended threshold is 0.82
	};

	//Version Information
	typedef struct {
		MPChar Version;				// version number
		MPChar BuildDate;			// build date
		MPChar CopyRight;			// Copyright
	}MAI_VERSION, *LPMAI_VERSION;

	//image data
	typedef LPASVLOFFSCREEN LPMAI_ImageData;

	//Single face information
	typedef struct {
		MRECT		faceRect;		// Face frame information
		MInt32		faceOrient;		// The angle of the input image, you can refer to MiniAIFaceCompare_OrientCode
	} MAI_SingleFaceInfo, *LPMAI_SingleFaceInfo;

	//Multiple face information
	typedef struct {
		MRECT* 		 faceRect;			// Face frame information
		MInt32*		 faceOrient;		// The angle of the input image, you can refer to MiniAIFaceCompare_OrientCode . 
		MInt32		 faceNum;			// The number of detected faces
		MInt32*      faceID;			// face ID, FaceID is not returned in IMAGE mode
	}MAI_MultiFaceInfo, *LPMAI_MultiFaceInfo;

	// Activation file information
	typedef struct {
		MPChar startTime;		//Starting time
		MPChar endTime;			//deadline
		MPChar platform;		//platform
		MPChar sdkType;			//sdk type
		MPChar appId;			//APPID
		MPChar sdkKey;			//SDKKEY
		MPChar sdkVersion;		//SDK version number
		MPChar fileVersion;		//Activation file version number
	}MAI_ActiveFileInfo, *LPMAI_ActiveFileInfo;

	/*******************************************************************************************
	* API for obtaining activation file information
	*******************************************************************************************/
	MRESULT MAIGetActiveFileInfo(
		LPMAI_ActiveFileInfo  activeFileInfo  // [out] Activation file information
		);

	/*******************************************************************************************
	* Online activation interface
	*******************************************************************************************/
	MRESULT MAIOnlineActivation(
		MPChar				AppId,			// [in]  APPID	
		MPChar				SDKKey			// [in]  SDKKEY	
		);

	/*******************************************************************************************
	* Online activation interface, the function of this interface is consistent with that of MAIOnlineActivation interface, it is recommended to use this interface
	*******************************************************************************************/
	MRESULT MAIActivation(
		MPChar				AppId,			// [in]  APPID	
		MPChar				SDKKey			// [in]  SDKKEY
		);


	// Activation Result
	enum MAI_ActivationResult {
		MAI_ACTIVE_SUCCESS = 0x0	// Activate Success
	};

	int MAIGetCurrentHWID(char* hwid, int size);
	
	int MAISetSerialNumber(const char* serial);
	
	/************************************************************************
	* Initialize the engine
	************************************************************************/
	MRESULT MAIInitEngine(
		MAI_DetectMode		detectMode,					// [in]	AF_DETECT_MODE_VIDEO Video mode: suitable for camera preview, video file recognition
		//		AF_DETECT_MODE_IMAGE Picture mode: suitable for the recognition of still pictures
		MAI_OrientPriority	detectFaceOrientPriority,	// [in]	Detect the angle priority value of the face, refer to MiniAIFaceCompare_OrientPriority
		MInt32				detectFaceScaleVal,			// [in] The minimum face size used for numerical representation, which represents the proportion of the face size relative to the long side of the picture
		// The valid value range of video mode is [2, 32], and the recommended value is 16
		// The valid value range of image mode is [2, 32], the recommended value is 32
		MInt32				detectFaceMaxNum,			// [in] The maximum number of faces to be detected
		MInt32				combinedMask,				// [in] The user selects the combination of functions to be detected, single or multiple
		MHandle*			hEngine						// [out] Initialize the returned engine handle
		);

	/******************************************************
	* VIDEO mode: face tracking IMAGE mode: face detection
	******************************************************/
	MRESULT MAIDetectFaces(
		MHandle				hEngine,							// [in] engine handle
		MInt32				width,								// [in] image width
		MInt32				height,								// [in] image height
		MInt32				format,								// [in] color space format
		MUInt8*				imgData,							// [in] image data
		LPMAI_MultiFaceInfo	detectedFaces,						// [out] Detected face information
		MAI_DetectModel		detectModel = MAI_DETECT_MODEL_RGB	// [in] Reserved field, the current version can use the default parameters
		);

	/******************************************************
	* VIDEO mode: face tracking IMAGE mode: face detection
	* The image data is passed in as a structure, which is more compatible with images with higher byte alignment
	******************************************************/
	MRESULT MAIDetectFacesEx(
		MHandle				hEngine,							// [in] engine handle
		LPMAI_ImageData		imgData,							// [in] image data
		LPMAI_MultiFaceInfo	detectedFaces,						// [out] Detected face information
		MAI_DetectModel		detectModel = MAI_DETECT_MODEL_RGB	// [in]	Reserved field, the current version can use the default parameters
		);

	//******************************** liveness threshold setting **********************************************
	typedef struct {
		MFloat		thresholdmodel_BGR;
		MFloat		thresholdmodel_IR;
	}MAI_LivenessThreshold, *LPMAI_LivenessThreshold;

	/************************************************************************
	* Value range [0-1], default value BGR:0.5 IR:0.7, users can set different thresholds according to actual needs
	************************************************************************/
	MRESULT MAISetLivenessParam(
		MHandle					hEngine,		// [in] engine handle
		LPMAI_LivenessThreshold threshold		// [in] Liveness Confidence
		);

	/************************************************************************
	* Age/gender/face 3D angle (this interface only supports RGB images), supports up to 4 face information detection, more than part returns unknown
	* RGB liveness only supports single face detection, this interface does not support detection of IR liveness
	************************************************************************/
	MRESULT MAIProcess(
		MHandle				hEngine,			// [in] engine handle
		MInt32				width,				// [in] image width
		MInt32				height,				// [in] image height
		MInt32				format,				// [in] color space format
		MUInt8*				imgData,			// [in] image data
		LPMAI_MultiFaceInfo	detectedFaces,		// [in] Face information, the user selects the face to be used according to the function to be detected.
		MInt32				combinedMask		// [in] It only supports specifying the functions that need to be detected during initialization, and further filtering in the specified function set during the process
		//      For example, specify to detect age and gender during initialization, only age can be detected during process, but functions other than age and gender cannot be detected    
		);

	/************************************************************************
	* Age/gender/face 3D angle (this interface only supports RGB images), supports up to 4 face information detection, more than part returns unknown
	* RGB live body only supports single face detection, this interface does not support detection of IR liveness
	* The image data is passed in as a structure, which is more compatible with images with higher byte alignment
	************************************************************************/
	MRESULT MAIProcessEx(
		MHandle				hEngine,			// [in] engine handle
		LPMAI_ImageData		imgData,			// [in] image data
		LPMAI_MultiFaceInfo detectedFaces,		// [in] 人脸信息，用户根据待检测的功能选择需要使用的人脸。
		MInt32				combinedMask		// [in] It only supports specifying the functions that need to be detected during initialization, and further filtering in the specified function set during the process
		//      For example, specify to detect age and gender during initialization, only age can be detected during process, but functions other than age and gender cannot be detected 
		);

	/************************************************************************
	* This interface currently only supports single-face IR liveness detection (does not support age, gender, and 3D angle detection), and takes the first face by default
	************************************************************************/
	MRESULT MAIProcess_IR(
		MHandle				hEngine,			// [in] engine handle
		MInt32				width,				// [in] image width
		MInt32				height,				// [in] image height
		MInt32				format,				// [in] color space format
		MUInt8*				imgData,			// [in] image data
		LPMAI_MultiFaceInfo	detectedFaces,		// [in] Face information, the user selects the face to be used according to the function to be detected。
		MInt32				combinedMask		// [in] Currently only supports passing in the MAI_IR_LIVENESS attribute, and the initialization interface needs to be passed in 
		);

	/************************************************************************
	* This interface currently only supports single-face IR liveness detection (does not support age, gender, and 3D angle detection), and takes the first face by default
	* The image data is passed in as a structure, which is more compatible with images with higher byte alignment
	************************************************************************/
	MRESULT MAIProcessEx_IR(
		MHandle				hEngine,			// [in] engine handle
		LPMAI_ImageData		imgData,			// [in] image data
		LPMAI_MultiFaceInfo detectedFaces,		// [in] Face information, the user selects the face to be used according to the function to be detected。
		MInt32				combinedMask		// [in] Currently only supports passing in the MAI_IR_LIVENESS attribute, and the initialization interface needs to be passed in
		);

	/************************************************************************
	* destroy engine
	************************************************************************/
	MRESULT MAIUninitEngine(
		MHandle hEngine
		);

	/************************************************************************
	* Get version information
	************************************************************************/
	const MAI_VERSION MAIGetVersion();

	//******************************** Face recognition related *********************************************
	typedef struct {
		MByte*		feature;		// facial feature information
		MInt32		featureSize;	// Length of face feature information	
	}MAI_FaceFeature, *LPMAI_FaceFeature;

	/************************************************************************
	* Single face feature extraction
	************************************************************************/
	MRESULT MAIFaceFeatureExtract(
		MHandle					hEngine,		// [in]	engine handle
		MInt32					width,			// [in] image width
		MInt32					height,			// [in] image height
		MInt32					format,			// [in] color space format
		MUInt8*					imgData,		// [in] image Data
		LPMAI_SingleFaceInfo	faceInfo,		// [in] Single face position and angle information
		LPMAI_FaceFeature		feature			// [out] facial features
		);

	/************************************************************************
	* Single face feature extraction
	* The image data is passed in as a structure, which is more compatible with images with higher byte alignment
	************************************************************************/
	MRESULT MAIFaceFeatureExtractEx(
		MHandle					hEngine,		// [in]	engine handle
		LPMAI_ImageData			imgData,		// [in] image data
		LPMAI_SingleFaceInfo	faceInfo,		// [in] Single face position and angle information
		LPMAI_FaceFeature		feature			// [out] facial features
		);

	/************************************************************************
	* Face feature comparison, recommended threshold MAI_LIFE_PHOTO: 0.80 MAI_ID_PHOTO: 0.82
	************************************************************************/
	MRESULT MAIFaceFeatureCompare(
		MHandle					hEngine,						// [in] engine handle
		LPMAI_FaceFeature		feature1,						// [in] Face features to be compared 1
		LPMAI_FaceFeature		feature2,						// [in] Face features to be compared 2
		MFloat*					confidenceLevel,				// [out] Comparison result, confidence value
		MAI_CompareModel		compareModel = MAI_LIFE_PHOTO	// [in] MAI_LIFE_PHOTO: used for feature comparison between life photos
		//		MAI_ID_PHOTO: used for feature comparison between ID photos or ID photos and life photos
		);

	//******************************** age related **********************************************
	typedef struct {
		MInt32*	ageArray;				// "0" means uncertain, and a value greater than 0 means the detected age result
		MInt32	num;					// The number of detected faces
	}MAI_AgeInfo, *LPMAI_AgeInfo;

	/************************************************************************
	* Get age information
	************************************************************************/
	MRESULT MAIGetAge(
		MHandle hEngine,				// [in] engine handle
		LPMAI_AgeInfo ageInfo			// [out] Detected age information
		);

	//******************************** gender related **********************************************
	typedef struct {
		MInt32*	genderArray;			//"0" means male, "1" means female, "-1" means uncertain
		MInt32	num;					// The number of detected faces	
	}MAI_GenderInfo, *LPMAI_GenderInfo;

	/************************************************************************
	* Get gender information
	************************************************************************/
	MRESULT MAIGetGender(
		MHandle hEngine,				// [in] engine handle
		LPMAI_GenderInfo genderInfo		// [out] Detected gender information
		);

	//******************************** 	Face 3D angle information ***********************************
	typedef struct {
		MFloat* roll;
		MFloat* yaw;
		MFloat* pitch;
		MInt32* status;					//0: OK, other values: error
		MInt32 num;
	}MAI_Face3DAngle, *LPMAI_Face3DAngle;

	/************************************************************************
	* Get 3D angle information
	************************************************************************/
	MRESULT MAIGetFace3DAngle(
		MHandle hEngine,					// [in] engine handle
		LPMAI_Face3DAngle p3DAngleInfo		// [out] Detected face 3D angle information
		);

	//******************************** 活体信息 ***********************************
	typedef struct {
		MInt32*	isLive;			// [out] Judging whether it is a real person, 0: not a real person;
		//						1：real person；
		//						-1：uncertain； 
		//						-2:The number of incoming faces > 1；
		//                      -3: face is too small
		//                      -4: Angle is too large
		//                      -5: face out of bounds 
		MInt32 num;
	}MAI_LivenessInfo, *LPMAI_LivenessInfo;

	/************************************************************************
	* 	Get RGB in liveness results
	************************************************************************/
	MRESULT MAIGetLivenessScore(
		MHandle hEngine,					// [in] engine handle
		LPMAI_LivenessInfo livenessInfo		// [out] Detection of RGB liveness results
		);

	/************************************************************************
	* Get IR Liveness Results
	************************************************************************/
	MRESULT MAIGetLivenessScore_IR(
		MHandle				hEngine,			// [in] engine handle
		LPMAI_LivenessInfo	 irLivenessInfo		// [out] IR liveness results detected
		);


#ifdef __cplusplus
}
#endif
#endif