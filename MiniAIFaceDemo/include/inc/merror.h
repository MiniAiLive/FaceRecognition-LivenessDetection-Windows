/*----------------------------------------------------------------------------------------------
*
* This file is MiniAI's property. It contains MiniAI's trade secret, proprietary and
* confidential information.
*
* The information and code contained in this file is only for authorized MiniAI employees
* to design, create, modify, or review.
*
* DO NOT DISTRIBUTE, DO NOT DUPLICATE OR TRANSMIT IN ANY FORM WITHOUT PROPER AUTHORIZATION.
*
* If you are not an intended recipient of this file, you must not copy, distribute, modify,
* or take any action in reliance on it.
*
* If you have received this file in error, please immediately notify MiniAI and
* permanently delete the original and any copy of any file and any printout thereof.
*
*-------------------------------------------------------------------------------------------------*/


#ifndef __MERROR_H__
#define __MERROR_H__


#define MERR_NONE						(0)
#define MOK								(0)


#define MERR_BASIC_BASE					0X0001							//generic error type
#define MERR_UNKNOWN					MERR_BASIC_BASE					//error unknown
#define MERR_INVALID_PARAM				(MERR_BASIC_BASE+1)				//invalid parameter
#define MERR_UNSUPPORTED				(MERR_BASIC_BASE+2)				//engine does not support
#define MERR_NO_MEMORY					(MERR_BASIC_BASE+3)				//Not enough storage
#define MERR_BAD_STATE					(MERR_BASIC_BASE+4)				//status error
#define MERR_USER_CANCEL				(MERR_BASIC_BASE+5)				//The user cancels the related operation
#define MERR_EXPIRED					(MERR_BASIC_BASE+6)				//Operation time expired
#define MERR_USER_PAUSE					(MERR_BASIC_BASE+7)				//user suspend operation
#define MERR_BUFFER_OVERFLOW		    (MERR_BASIC_BASE+8)				//buffer overflow
#define MERR_BUFFER_UNDERFLOW		    (MERR_BASIC_BASE+9)				//buffer underflow
#define MERR_NO_DISKSPACE				(MERR_BASIC_BASE+10)			//Insufficient storage space
#define	MERR_COMPONENT_NOT_EXIST		(MERR_BASIC_BASE+11)			//component does not exist
#define	MERR_GLOBAL_DATA_NOT_EXIST		(MERR_BASIC_BASE+12)			//global data does not exist


#define MERR_FSDK_BASE							0X7000					//Free SDK generic error type
#define MERR_FSDK_INVALID_APP_ID				(MERR_FSDK_BASE+1)		//Invalid App Id
#define MERR_FSDK_INVALID_SDK_ID				(MERR_FSDK_BASE+2)		//Invalid SDK key
#define MERR_FSDK_INVALID_ID_PAIR				(MERR_FSDK_BASE+3)		//AppId and SDKKey do not match
#define MERR_FSDK_MISMATCH_ID_AND_SDK			(MERR_FSDK_BASE+4)		//SDKKey does not match the SDK used
#define MERR_FSDK_SYSTEM_VERSION_UNSUPPORTED	(MERR_FSDK_BASE+5)		//The system version is not supported by the current SDK
#define MERR_FSDK_LICENCE_EXPIRED				(MERR_FSDK_BASE+6)		//The validity period of the SDK has expired and you need to re-download the update


#define MERR_FSDK_FR_ERROR_BASE					0x12000							//Face Recognition Error Types
#define MERR_FSDK_FR_INVALID_MEMORY_INFO		(MERR_FSDK_FR_ERROR_BASE+1)		//invalid input memory
#define MERR_FSDK_FR_INVALID_IMAGE_INFO			(MERR_FSDK_FR_ERROR_BASE+2)		//invalid input image parameter
#define MERR_FSDK_FR_INVALID_FACE_INFO			(MERR_FSDK_FR_ERROR_BASE+3)		//invalid facial information
#define MERR_FSDK_FR_NO_GPU_AVAILABLE			(MERR_FSDK_FR_ERROR_BASE+4)		//The current device has no GPU available
#define MERR_FSDK_FR_MISMATCHED_FEATURE_LEVEL	(MERR_FSDK_FR_ERROR_BASE+5)		//The versions of the two facial features to be compared are inconsistent


#define MERR_FSDK_FACEFEATURE_ERROR_BASE			0x14000									//Facial Feature Detection Error Types
#define MERR_FSDK_FACEFEATURE_UNKNOWN				(MERR_FSDK_FACEFEATURE_ERROR_BASE+1)	//Face feature detection error is unknown
#define MERR_FSDK_FACEFEATURE_MEMORY				(MERR_FSDK_FACEFEATURE_ERROR_BASE+2)	//Face feature detection memory error
#define MERR_FSDK_FACEFEATURE_INVALID_FORMAT		(MERR_FSDK_FACEFEATURE_ERROR_BASE+3)	//Face feature detection format error
#define MERR_FSDK_FACEFEATURE_INVALID_PARAM			(MERR_FSDK_FACEFEATURE_ERROR_BASE+4)	//Facial feature detection parameter error
#define MERR_FSDK_FACEFEATURE_LOW_CONFIDENCE_LEVEL	(MERR_FSDK_FACEFEATURE_ERROR_BASE+5)	//Low confidence in face feature detection results

#define MERR_MAI_EX_BASE								0x15000							//MAI error type
#define MERR_MAI_EX_FEATURE_UNSUPPORTED_ON_INIT			(MERR_MAI_EX_BASE+1)			//Engine Unsupported detection property
#define MERR_MAI_EX_FEATURE_UNINITED					(MERR_MAI_EX_BASE+2)			//The property to be checked is not initialized
#define MERR_MAI_EX_FEATURE_UNPROCESSED					(MERR_MAI_EX_BASE+3)			//The attribute to be obtained has not been processed in the process
#define MERR_MAI_EX_FEATURE_UNSUPPORTED_ON_PROCESS		(MERR_MAI_EX_BASE+4)			//The combination of detection attributes not supported by PROCESS, such as FR, has its own independent processing function
#define MERR_MAI_EX_INVALID_IMAGE_INFO					(MERR_MAI_EX_BASE+5)			//invalid input image
#define MERR_MAI_EX_INVALID_FACE_INFO					(MERR_MAI_EX_BASE+6)			//invalid facial information

#define MERR_MAI_BASE									0x16000							//Face comparison basic error types
#define MERR_MAI_ACTIVATION_FAIL						(MERR_MAI_BASE+1)				//SDK Activation failed, please open read and write permissions
#define MERR_MAI_ALREADY_ACTIVATED						(MERR_MAI_BASE+2)				//SDK activated
#define MERR_MAI_NOT_ACTIVATED							(MERR_MAI_BASE+3)				//SDK inactivated
#define MERR_MAI_SCALE_NOT_SUPPORT						(MERR_MAI_BASE+4)				//detectFaceScaleVal not support
#define MERR_MAI_ACTIVEFILE_SDKTYPE_MISMATCH			(MERR_MAI_BASE+5)				//The activation file does not match the SDK type, please confirm the SDK used
#define MERR_MAI_DEVICE_MISMATCH						(MERR_MAI_BASE+6)				//device mismatch
#define MERR_MAI_UNIQUE_IDENTIFIER_ILLEGAL				(MERR_MAI_BASE+7)				//The unique identifier is invalid
#define MERR_MAI_PARAM_NULL								(MERR_MAI_BASE+8)				//parameter is empty
#define MERR_MAI_LIVENESS_EXPIRED						(MERR_MAI_BASE+9)				//Liveness has expired
#define MERR_MAI_VERSION_NOT_SUPPORT					(MERR_MAI_BASE+10)				//version does not support
#define MERR_MAI_SIGN_ERROR								(MERR_MAI_BASE+11)				//signature error
#define MERR_MAI_DATABASE_ERROR							(MERR_MAI_BASE+12)				//Activation information save exception
#define MERR_MAI_UNIQUE_CHECKOUT_FAIL					(MERR_MAI_BASE+13)				//Unique identifier validation failed
#define MERR_MAI_COLOR_SPACE_NOT_SUPPORT				(MERR_MAI_BASE+14)				//color space not supported
#define	MERR_MAI_IMAGE_WIDTH_HEIGHT_NOT_SUPPORT			(MERR_MAI_BASE+15)				//The image width and height are not supported, and the width needs to be aligned with four bytes

#define MERR_MAI_BASE_EXTEND							0x16010							//Face comparison basic error types
#define MERR_MAI_READ_PHONE_STATE_DENIED				(MERR_MAI_BASE_EXTEND)			//android.permission.READ_PHONE_STATE permission denied
#define	MERR_MAI_ACTIVATION_DATA_DESTROYED				(MERR_MAI_BASE_EXTEND+1)		//The activation data is damaged, please delete the activation file and re-activate
#define	MERR_MAI_SERVER_UNKNOWN_ERROR					(MERR_MAI_BASE_EXTEND+2)		//server unknown error
#define MERR_MAI_INTERNET_DENIED				        (MERR_MAI_BASE_EXTEND+3)		//INTERNET permission denied
#define MERR_MAI_ACTIVEFILE_SDK_MISMATCH				(MERR_MAI_BASE_EXTEND+4)		//The activation file does not match the SDK version, please reactivate
#define MERR_MAI_DEVICEINFO_LESS						(MERR_MAI_BASE_EXTEND+5)		//There is too little device information to generate a device fingerprint
#define MERR_MAI_LOCAL_TIME_NOT_CALIBRATED				(MERR_MAI_BASE_EXTEND+6)		//The difference between the client time and the server time (i.e. Beijing time) is more than 30 minutes
#define MERR_MAI_APPID_DATA_DECRYPT						(MERR_MAI_BASE_EXTEND+7)		//Data verification exception
#define MERR_MAI_APPID_APPKEY_SDK_MISMATCH				(MERR_MAI_BASE_EXTEND+8)		//The AppId and AppKey passed in are inconsistent with the SDK version used
#define MERR_MAI_NO_REQUEST								(MERR_MAI_BASE_EXTEND+9)		//A large number of requests in a short period of time will be banned, and will be unblocked after 30 minutes
#define MERR_MAI_ACTIVE_FILE_NO_EXIST					(MERR_MAI_BASE_EXTEND+10)		//Activation file does not exist
#define MERR_MAI_DETECT_MODEL_UNSUPPORTED				(MERR_MAI_BASE_EXTEND+11)		//The detection model does not support it. Please check the corresponding interface description and use the currently supported detection model
#define MERR_MAI_CURRENT_DEVICE_TIME_INCORRECT			(MERR_MAI_BASE_EXTEND+12)		//The current device time is incorrect, please adjust the device time
#define MERR_MAI_ACTIVATION_QUANTITY_OUT_OF_LIMIT		(MERR_MAI_BASE_EXTEND+13)		//The number of annual activations exceeds the limit and will be cleared in the next year
#define MERR_MAI_IP_BLACK_LIST							(MERR_MAI_BASE_EXTEND+14)		//Frequent requests, unlock after 4 hours

#define MERR_MAI_NETWORK_BASE							0x17000							//network error type
#define MERR_MAI_NETWORK_COULDNT_RESOLVE_HOST			(MERR_MAI_NETWORK_BASE+1)		//Unable to resolve host address
#define MERR_MAI_NETWORK_COULDNT_CONNECT_SERVER			(MERR_MAI_NETWORK_BASE+2)		//can not reach server
#define MERR_MAI_NETWORK_CONNECT_TIMEOUT				(MERR_MAI_NETWORK_BASE+3)		//network connection timed out
#define MERR_MAI_NETWORK_UNKNOWN_ERROR					(MERR_MAI_NETWORK_BASE+4)		//network unknown error


#endif

