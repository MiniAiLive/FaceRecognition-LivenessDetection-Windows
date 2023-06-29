
// MiniAIFaceDemoDlg.cpp : implementation file
//

#include "stdafx.h"

#include "MiniAIFaceDemo.h"
#include "MiniAIFaceDemoDlg.h"
#include "afxdialogex.h"
#include <afx.h>
#include <vector>
#include "opencv/cv.h"
#include "opencv/highgui.h"
#include <mutex>
#include <strmif.h>
#include <initguid.h>
#include <string>

#include "free/miniai_face_sdk.h"

#pragma comment(lib, "setupapi.lib")


using namespace std;
using namespace Gdiplus;

#define VIDEO_FRAME_DEFAULT_WIDTH 640
#define VIDEO_FRAME_DEFAULT_HEIGHT 480

#define FACE_FEATURE_SIZE 1032

#define THUMBNAIL_WIDTH  55
#define THUMBNAIL_HEIGHT  55
#define Threshold 0.80

#define RGB_CAMERA_ID		1
#define IR_CAMERA_ID		0
#define RGB_LIVE_THRESHOLD	0.5
#define IR_LIVE_THRESHOLD	0.7

#define VI_MAX_CAMERAS 20
DEFINE_GUID(CLSID_SystemDeviceEnum, 0x62be5d10, 0x60eb, 0x11d0, 0xbd, 0x3b, 0x00, 0xa0, 0xc9, 0x11, 0xce, 0x86);
DEFINE_GUID(CLSID_VideoInputDeviceCategory, 0x860bb310, 0x5d01, 0x11d0, 0xbd, 0x3b, 0x00, 0xa0, 0xc9, 0x11, 0xce, 0x86);
DEFINE_GUID(IID_ICreateDevEnum, 0x29840822, 0x5b84, 0x11d0, 0xbd, 0x3b, 0x00, 0xa0, 0xc9, 0x11, 0xce, 0x86);

#define SafeFree(p) { if ((p)) free(p); (p) = NULL; }
#define SafeArrayDelete(p) { if ((p)) delete [] (p); (p) = NULL; } 
#define SafeDelete(p) { if ((p)) delete (p); (p) = NULL; } 

mutex g_mutex;
vector<string> g_cameraName;
static int g_cameraNum = 0;

unsigned long _stdcall RunLoadThumbnailThread(LPVOID lpParam);
unsigned long _stdcall RunFaceFeatureOperation(LPVOID lpParam);
unsigned long _stdcall RunFaceDetectOperation(LPVOID lpParam);
unsigned long _stdcall ClearFaceFeatureOperation(LPVOID lpParam);
Bitmap* IplImage2Bitmap(const IplImage* pIplImg);
IplImage* Bitmap2IplImage(Bitmap* pBitmap);
CBitmap* IplImage2CBitmap(const IplImage *img);
BOOL SetTextFont(CFont* font, int fontHeight, int fontWidth, string fontStyle);
int listDevices(vector<string>& list);			//get webcam


// CAboutDlg dialog for the application's "About" menu item

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// dialog data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// accomplish
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMiniAIFaceDemoDlg dialog box

CMiniAIFaceDemoDlg::CMiniAIFaceDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMiniAIFaceDemoDlg::IDD, pParent),
	m_strEditThreshold(_T("")),
	m_curStaticImageFRSucceed(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON_MINIAI);

}

CMiniAIFaceDemoDlg::~CMiniAIFaceDemoDlg()
{

}


void CMiniAIFaceDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_IMAGE, m_ImageListCtrl);
	DDX_Control(pDX, IDC_EDIT_LOG, m_editLog);
	DDX_Control(pDX, IDC_EDIT_HWID, m_editHwid);
	DDX_Control(pDX, IDC_EDIT_LICENSE, m_editLicense);
	DDX_Text(pDX, IDC_EDIT_THRESHOLD, m_strEditThreshold);
	DDX_Text(pDX, IDC_EDIT_LICENSE, m_strEditLicense);
}

BEGIN_MESSAGE_MAP(CMiniAIFaceDemoDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_REGISTER, &CMiniAIFaceDemoDlg::OnBnClickedBtnRegister)
	ON_BN_CLICKED(IDC_BTN_RECOGNITION, &CMiniAIFaceDemoDlg::OnBnClickedBtnRecognition)
	ON_BN_CLICKED(IDC_BTN_COMPARE, &CMiniAIFaceDemoDlg::OnBnClickedBtnCompare)
	ON_BN_CLICKED(IDC_BTN_CLEAR, &CMiniAIFaceDemoDlg::OnBnClickedBtnClear)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_CAMERA, &CMiniAIFaceDemoDlg::OnBnClickedBtnCamera)
	ON_EN_CHANGE(IDC_EDIT_THRESHOLD, &CMiniAIFaceDemoDlg::OnEnChangeEditThreshold)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BTN_ACTIVATE, &CMiniAIFaceDemoDlg::OnBnClickedBtnActivate)
END_MESSAGE_MAP()


// CMiniAIFaceDemoDlg message handler
BOOL CMiniAIFaceDemoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();


	// Add an "About..." menu item to the system menu.

	// IDM_ABOUTBOX must be in system command scope。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE); 
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Sets the icon for this dialog. When the main application window is not a dialog, the frame will automatically
	//  do this
	SetIcon(m_hIcon, TRUE);			// set large icons
	SetIcon(m_hIcon, FALSE);		// set small icon

	// TODO:  Add additional initialization code here
	m_IconImageList.Create(THUMBNAIL_WIDTH,
		THUMBNAIL_HEIGHT,
		ILC_COLOR32,
		0,
		1);

	m_ImageListCtrl.SetImageList(&m_IconImageList, LVSIL_NORMAL);

	CString resStr = "";

	//Set the number of input boxes
	((CEdit*)GetDlgItem(IDC_EDIT_THRESHOLD))->SetLimitText(4);
	m_strEditThreshold.Format("%.2f", Threshold);
	UpdateData(FALSE);

	GetDlgItem(IDC_STATIC_VIEW)->GetWindowRect(&m_windowViewRect);

	//Face library button grayed out
	GetDlgItem(IDC_BTN_COMPARE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_CAMERA)->EnableWindow(FALSE);

	//Edit threshold grayed out
	GetDlgItem(IDC_EDIT_THRESHOLD)->EnableWindow(FALSE);

	m_curStaticImageFeature.featureSize = FACE_FEATURE_SIZE;
	m_curStaticImageFeature.feature = (MByte *)malloc(m_curStaticImageFeature.featureSize * sizeof(MByte));


	m_Font = new CFont;

	SetTextFont(m_Font, 20, 20, "微软雅黑");

	m_editLicense.ModifyStyle(0, ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | WS_VSCROLL);

	MRESULT activeRes = m_imageFaceEngine.CheckActiveSDK();
	resStr.Format("Activation Result: %s\n", activeRes == MAI_ACTIVE_SUCCESS ? "Success" : "Failed");
	EditOut(resStr, TRUE);


	if (activeRes == MOK)
	{
		initEngine();
	}
	else {
		//display activation window
		showActiveWindow(SW_SHOW);

	}

	return TRUE;  // Returns TRUE unless focus is set to the control
}

void CMiniAIFaceDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to the dialog, you need the following code
// to draw the icon. For MFC applications using document/view models,
// This will be done automatically by the framework.


void CMiniAIFaceDemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // the device context used for drawing

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center the icon in the workspace rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// draw icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		if (m_videoOpened)
		{
			lock_guard<mutex> lock(g_mutex);
			//text box
			CRect rect(m_curFaceInfo.faceRect.left - 10, m_curFaceInfo.faceRect.top - 50,
				m_curFaceInfo.faceRect.right, m_curFaceInfo.faceRect.bottom);
			IplDrawToHDC(TRUE, m_curVideoImage, rect, IDC_STATIC_VIEW);
		}
		else
		{
			if (m_curStaticImage)
			{
				CRect rect((int)m_curStringShowPosition.X + 10, (int)m_curStringShowPosition.Y + 10, 40, 40);
				IplDrawToHDC(FALSE, m_curStaticImage, rect, IDC_STATIC_VIEW);
			}
		}

		CDialogEx::OnPaint();
	}


}

//When the user drags the minimized window, the system calls this function to get the cursor
//show.
HCURSOR CMiniAIFaceDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL IsImageGDIPLUSValid(CString filePath)
{
	Bitmap image(filePath.AllocSysString());

	if (image.GetFlags() == ImageFlagsNone)
		return FALSE;
	else
		return TRUE;
}


//load thumbnail image
void CMiniAIFaceDemoDlg::LoadThumbnailImages()
{
	m_bLoadIconThreadRunning = TRUE;

	GetDlgItem(IDC_BTN_COMPARE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_CAMERA)->EnableWindow(FALSE);

	m_hLoadIconThread = CreateThread(
		NULL,
		0,
		RunLoadThumbnailThread,
		this,
		0,
		&m_dwLoadIconThreadID);

	if (m_hLoadIconThread == NULL)
	{
		::CloseHandle(m_hLoadIconThread);
	}
}


void CMiniAIFaceDemoDlg::OnBnClickedBtnRegister()
{
	// TODO:  Add your control notification handler code here
	GetDlgItem(IDC_BTN_REGISTER)->EnableWindow(FALSE);
	m_folderPath = SelectFolder();
	if (m_folderPath == "")
	{
		GetDlgItem(IDC_BTN_REGISTER)->EnableWindow(TRUE);
		return;
	}
	LoadThumbnailImages();
}

unsigned long _stdcall RunLoadThumbnailThread(LPVOID lpParam)
{
	CMiniAIFaceDemoDlg* dialog = (CMiniAIFaceDemoDlg*)(lpParam);

	if (dialog == nullptr)
	{
		dialog->m_bLoadIconThreadRunning = FALSE;
		return 1;
	}

	if (dialog->m_folderPath == "")
	{
		dialog->m_bLoadIconThreadRunning = FALSE;
		return 1;
	}

	int iExistFeatureSize = (int)dialog->m_featuresVec.size();

	CString resStr;
	resStr.Format("start enrolling faces");
	dialog->EditOut(resStr, TRUE);

	CFileFind finder;

	CString m_strCurrentDirectory(dialog->m_folderPath);
	CString strWildCard(m_strCurrentDirectory);
	vector<CString> m_vFileName;
	strWildCard += "\\*.*";

	BOOL bWorking = finder.FindFile(strWildCard);

	while (bWorking)
	{
		bWorking = finder.FindNextFile();

		if (finder.IsDots() || finder.IsDirectory())
		{
			continue;
		}

		CString filePath = finder.GetFileName();

		if (IsImageGDIPLUSValid(m_strCurrentDirectory + _T("\\") + filePath))//是否是图片
		{
			m_vFileName.push_back(filePath);
		}
	}

	resStr.Format("count of seleted pictures: %d", m_vFileName.size());
	dialog->EditOut(resStr, TRUE);

	dialog->GetDlgItem(IDC_BTN_CLEAR)->EnableWindow(FALSE);
	dialog->GetDlgItem(IDC_BTN_REGISTER)->EnableWindow(FALSE);
	dialog->GetDlgItem(IDC_BTN_COMPARE)->EnableWindow(FALSE);

	if (dialog->GetDlgItem(IDC_BTN_RECOGNITION)->IsWindowEnabled())
	{
		dialog->GetDlgItem(IDC_BTN_RECOGNITION)->EnableWindow(FALSE);
	}

	vector<CString>::const_iterator iter;

	int actualIndex = iExistFeatureSize;

	for (iter = m_vFileName.begin();
		iter != m_vFileName.end();
		iter++)
	{
		if (!dialog->m_bLoadIconThreadRunning)
		{
			dialog->m_bLoadIconThreadRunning = FALSE;
			return 1;
		}

		CString imagePath;
		imagePath.Empty();
		imagePath.Format("%s\\%s", m_strCurrentDirectory, *iter);

		USES_CONVERSION;
		IplImage* originImage = cvLoadImage(T2A(imagePath.GetBuffer(0)));
		imagePath.ReleaseBuffer();

		if (!originImage)
		{
			cvReleaseImage(&originImage);
			continue;
		}

		//FD 
		MAI_SingleFaceInfo faceInfo = { 0 };
		MRESULT detectRes = dialog->m_imageFaceEngine.PreDetectFace(originImage, faceInfo, true);
		if (MOK != detectRes)
		{
			cvReleaseImage(&originImage);
			continue;
		}

		//FR
		MAI_FaceFeature faceFeature = { 0 };
		faceFeature.featureSize = FACE_FEATURE_SIZE;
		faceFeature.feature = (MByte *)malloc(faceFeature.featureSize * sizeof(MByte));
		detectRes = dialog->m_imageFaceEngine.PreExtractFeature(originImage, faceFeature, faceInfo);

		if (MOK != detectRes)
		{
			free(faceFeature.feature);
			cvReleaseImage(&originImage);
			continue;
		}

		Bitmap* image = IplImage2Bitmap(originImage);
		dialog->m_featuresVec.push_back(faceFeature);

		//Calculate thumbnail display position
		int sourceWidth = image->GetWidth();
		int sourceHeight = image->GetHeight();

		int destX = 0;
		int destY = 0;

		float nPercent = 0;
		float nPercentW = ((float)THUMBNAIL_WIDTH / (float)sourceWidth);;
		float nPercentH = ((float)THUMBNAIL_HEIGHT / (float)sourceHeight);

		if (nPercentH < nPercentW)
		{
			nPercent = nPercentH;
			destX = (int)((THUMBNAIL_WIDTH - (sourceWidth * nPercent)) / 2);
		}
		else
		{
			nPercent = nPercentW;
			destY = (int)((THUMBNAIL_HEIGHT - (sourceHeight * nPercent)) / 2);
		}

		int destWidth = (int)(sourceWidth * nPercent);
		int destHeight = (int)(sourceHeight * nPercent);

		dialog->m_ImageListCtrl.InsertItem(actualIndex, to_string(actualIndex + 1).c_str(), actualIndex);

		actualIndex++;

		Bitmap* bmPhoto = new Bitmap(THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT, PixelFormat24bppRGB);

		bmPhoto->SetResolution(image->GetHorizontalResolution(), image->GetVerticalResolution());

		Graphics *grPhoto = Graphics::FromImage(bmPhoto);
		Gdiplus::Color colorW(255, 255, 255, 255);
		grPhoto->Clear(colorW);
		grPhoto->SetInterpolationMode(InterpolationModeHighQualityBicubic);
		grPhoto->DrawImage(image, Gdiplus::Rect(destX, destY, destWidth, destHeight));

		HBITMAP hbmReturn = NULL;
		bmPhoto->GetHBITMAP(colorW, &hbmReturn);

		CBitmap Bmp1;
		Bmp1.Attach(hbmReturn);

		dialog->m_IconImageList.Add(&Bmp1, RGB(0, 0, 0));

		delete grPhoto;
		delete bmPhoto;
		Bmp1.Detach();
		DeleteObject(hbmReturn);

		dialog->m_ImageListCtrl.RedrawItems(actualIndex, actualIndex);

		//redraw
		if (actualIndex % 10 == 0)
		{
			dialog->m_ImageListCtrl.SetRedraw(TRUE);
			dialog->m_ImageListCtrl.Invalidate();
			dialog->m_ImageListCtrl.EnsureVisible(actualIndex - 1, FALSE);
		}

		cvReleaseImage(&originImage);
		delete image;
	}

	resStr.Format("count of succeeded enrolling: %d", actualIndex - iExistFeatureSize);
	dialog->EditOut(resStr, TRUE);

	dialog->m_ImageListCtrl.SetRedraw(TRUE);
	dialog->m_ImageListCtrl.Invalidate();
	dialog->m_ImageListCtrl.EnsureVisible(actualIndex - 1, FALSE);

	if (dialog->m_featuresVec.empty())
	{

	}

	//After registering the face database, the button resets
	dialog->GetDlgItem(IDC_BTN_REGISTER)->EnableWindow(TRUE);

	if (!dialog->m_videoOpened)
	{
		dialog->GetDlgItem(IDC_BTN_COMPARE)->EnableWindow(TRUE);
		dialog->GetDlgItem(IDC_BTN_RECOGNITION)->EnableWindow(TRUE);
	}
	else
	{
		dialog->GetDlgItem(IDC_BTN_RECOGNITION)->EnableWindow(FALSE);
	}

	dialog->GetDlgItem(IDC_BTN_CAMERA)->EnableWindow(TRUE);
	dialog->GetDlgItem(IDC_BTN_REGISTER)->EnableWindow(TRUE);
	dialog->GetDlgItem(IDC_BTN_CLEAR)->EnableWindow(TRUE);

	dialog->m_bLoadIconThreadRunning = FALSE;

	return 0;
}

//select folder
CString CMiniAIFaceDemoDlg::SelectFolder()
{
	TCHAR           szFolderPath[MAX_PATH] = { 0 };
	CString         strFolderPath = TEXT("");

	BROWSEINFO      sInfo;
	::ZeroMemory(&sInfo, sizeof(BROWSEINFO));
	sInfo.pidlRoot = 0;
	sInfo.lpszTitle = _T("please select a folder：");
	sInfo.ulFlags = BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE | BIF_EDITBOX;
	sInfo.lpfn = NULL;

	// Show folder selection dialog  
	LPITEMIDLIST lpidlBrowse = ::SHBrowseForFolder(&sInfo);
	if (lpidlBrowse != NULL)
	{
		// get folder name  
		if (::SHGetPathFromIDList(lpidlBrowse, szFolderPath))
		{
			strFolderPath = szFolderPath;
		}
	}
	if (lpidlBrowse != NULL)
	{
		::CoTaskMemFree(lpidlBrowse);
	}

	return strFolderPath;
}


void CMiniAIFaceDemoDlg::OnBnClickedBtnRecognition()
{
	// TODO:  Add your control notification handler code here

	CFileDialog fileDlg(TRUE, _T("bmp"), NULL, 0, _T("Picture Files|*.jpg;*.jpeg;*.png;*.bmp;||"), NULL);
	fileDlg.DoModal();
	CString strFilePath;
	strFilePath = fileDlg.GetPathName();

	if (strFilePath == _T(""))
		return;

	USES_CONVERSION;
	IplImage* image = cvLoadImage(T2A(strFilePath.GetBuffer(0)));
	strFilePath.ReleaseBuffer();
	if (!image)
	{
		cvReleaseImage(&image);
		return;
	}

	if (m_curStaticImage)
	{
		cvReleaseImage(&m_curStaticImage);
		m_curStaticImage = NULL;
	}

	m_curStaticImage = cvCloneImage(image);
	cvReleaseImage(&image);

	StaticImageFaceOp(m_curStaticImage);

	GetDlgItem(IDC_BTN_COMPARE)->EnableWindow(TRUE);
}

MRESULT CMiniAIFaceDemoDlg::StaticImageFaceOp(IplImage* image)
{
	Gdiplus::Rect showRect;
	CalculateShowPositon(image, showRect);
	m_curImageShowRect = showRect;
	//FD
	MAI_SingleFaceInfo faceInfo = { 0 };
	MRESULT detectRes = m_imageFaceEngine.PreDetectFace(image, faceInfo, true);

	//initialization
	m_curStaticShowAgeGenderString = "";
	m_curStaticShowCmpString = "";

	m_curFaceShowRect = Rect(0, 0, 0, 0);

	SendMessage(WM_PAINT);

	if (MOK == detectRes)
	{
		//show rect
		int n_top = showRect.Height*faceInfo.faceRect.top / image->height;
		int n_bottom = showRect.Height*faceInfo.faceRect.bottom / image->height;
		int n_left = showRect.Width*faceInfo.faceRect.left / image->width;
		int n_right = showRect.Width*faceInfo.faceRect.right / image->width;

		m_curFaceShowRect.X = n_left + showRect.X;
		m_curFaceShowRect.Y = n_top + showRect.Y;
		m_curFaceShowRect.Width = n_right - n_left;
		m_curFaceShowRect.Height = n_bottom - n_top;

		//Display text in the upper left corner of the image
		m_curStringShowPosition.X = (REAL)(showRect.X);
		m_curStringShowPosition.Y = (REAL)(showRect.Y);

		//age gender
		MAI_MultiFaceInfo multiFaceInfo = { 0 };
		multiFaceInfo.faceOrient = (MInt32*)malloc(sizeof(MInt32));
		multiFaceInfo.faceRect = (MRECT*)malloc(sizeof(MRECT));

		multiFaceInfo.faceNum = 1;
		multiFaceInfo.faceOrient[0] = faceInfo.faceOrient;
		multiFaceInfo.faceRect[0] = faceInfo.faceRect;

		MAI_AgeInfo ageInfo = { 0 };
		MAI_GenderInfo genderInfo = { 0 };
		MAI_Face3DAngle angleInfo = { 0 };
		MAI_LivenessInfo liveNessInfo = { 0 };

		//age 、gender 、3d angle information
		detectRes = m_imageFaceEngine.FaceMAIProcess(multiFaceInfo, image,
			ageInfo, genderInfo, angleInfo, liveNessInfo);

		if (MOK == detectRes)
		{
			CString showStr;
			showStr.Format("Age:%d,Gender:%s,Liveness:%s", ageInfo.ageArray[0], genderInfo.genderArray[0] == 0 ? "Male" : "Female",
				liveNessInfo.isLive[0] == 1 ? "Yes" : "No");
			m_curStaticShowAgeGenderString = showStr;
		}
		else
		{
			m_curStaticShowAgeGenderString = "";
		}

		SendMessage(WM_PAINT);

		free(multiFaceInfo.faceRect);
		free(multiFaceInfo.faceOrient);

		//FR
		detectRes = m_imageFaceEngine.PreExtractFeature(image, m_curStaticImageFeature, faceInfo);

		if (MOK == detectRes)
		{
			m_curStaticImageFRSucceed = TRUE;
		}
		else//Failed to extract features
		{
			m_curStaticImageFRSucceed = FALSE;
			CString resStr;
			resStr.Format("failed to get feature");
			EditOut(resStr, TRUE);
			return -1;
		}
		return MOK;
	}
	else
	{
		m_curStaticImageFRSucceed = FALSE;

		CString resStr;
		resStr.Format("no detected face");
		EditOut(resStr, TRUE);
		return -1;
	}
}

void CMiniAIFaceDemoDlg::EditOut(CString str, bool add_endl)
{
	if (add_endl)
		str += "\r\n";
	int iLen = m_editLog.GetWindowTextLength();
	m_editLog.SetSel(iLen, iLen, TRUE);
	m_editLog.ReplaceSel(str, FALSE);
}


IplImage* Bitmap2IplImage(Bitmap* pBitmap)
{
	if (!pBitmap)
		return NULL;

	int w = pBitmap->GetWidth();
	int h = pBitmap->GetHeight();

	BitmapData bmpData;
	Gdiplus::Rect rect(0, 0, w, h);
	pBitmap->LockBits(&rect, ImageLockModeRead, PixelFormat24bppRGB, &bmpData);
	BYTE* temp = (bmpData.Stride > 0) ? ((BYTE*)bmpData.Scan0) : ((BYTE*)bmpData.Scan0 + bmpData.Stride*(h - 1));

	IplImage* pIplImg = cvCreateImage(cvSize(w, h), IPL_DEPTH_8U, 3);
	if (!pIplImg)
	{
		pBitmap->UnlockBits(&bmpData);
		return NULL;
	}

	memcpy(pIplImg->imageData, temp, abs(bmpData.Stride)*bmpData.Height);
	pBitmap->UnlockBits(&bmpData);

	//Judgment Top-Down or Bottom-Up
	if (bmpData.Stride < 0)
		cvFlip(pIplImg, pIplImg);

	return pIplImg;
}

// pBitmap also requires external release
Bitmap* IplImage2Bitmap(const IplImage* pIplImg)
{
	if (!pIplImg)
		return NULL;

	Bitmap *pBitmap = new Bitmap(pIplImg->width, pIplImg->height, PixelFormat24bppRGB);
	if (!pBitmap)
		return NULL;

	BitmapData bmpData;
	Gdiplus::Rect rect(0, 0, pIplImg->width, pIplImg->height);
	pBitmap->LockBits(&rect, ImageLockModeWrite, PixelFormat24bppRGB, &bmpData);
	//BYTE *pByte = (BYTE*)bmpData.Scan0;

	if (pIplImg->widthStep == bmpData.Stride) //likely
		memcpy(bmpData.Scan0, pIplImg->imageDataOrigin, pIplImg->imageSize);

	pBitmap->UnlockBits(&bmpData);
	return pBitmap;
}

void CMiniAIFaceDemoDlg::OnBnClickedBtnCompare()
{
	// TODO:  Add your control notification handler code here

	if (!m_curStaticImageFRSucceed)
	{
		AfxMessageBox(_T("failed comparing, please reselect a picture!"));
		return;
	}

	if (m_featuresVec.size() == 0)
	{
		AfxMessageBox(_T("no enrolled faces！"));
		return;
	}

	int maxIndex = 1;//Default starts from 1st
	MFloat maxThreshold = 0.0;
	int curIndex = 0;

	//FR Comparison
	for each (auto regisFeature in m_featuresVec)
	{
		curIndex++;
		MFloat confidenceLevel = 0;
		// You can choose a comparison model, the recommended threshold for the witness model: 0.82, the recommended threshold for the life photo model: 0.80
		MRESULT pairRes = m_imageFaceEngine.FacePairMatching(confidenceLevel, m_curStaticImageFeature, regisFeature);
		if (MOK == pairRes &&confidenceLevel > maxThreshold)
		{
			maxThreshold = confidenceLevel;
			maxIndex = curIndex;
		}
	}

	CString resStr;

	//Show results		
	resStr.Format("No. %d:%.4f\n", maxIndex, maxThreshold);
	EditOut(resStr, TRUE);
	m_curStaticShowCmpString = resStr;
	SendMessage(WM_PAINT);

	resStr.Format("finished comparing");
	EditOut(resStr, TRUE);
}


void CMiniAIFaceDemoDlg::OnBnClickedBtnClear()
{
	// TODO: Add your control notification handler code here

	if (m_videoOpened)
	{
		AfxMessageBox(_T("please turn off a camera！"));
		return;
	}

	// The button to register face database is grayed out
	GetDlgItem(IDC_BTN_COMPARE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_CAMERA)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_REGISTER)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_CLEAR)->EnableWindow(FALSE);

	//Clean up the original image and features
	ClearRegisterImages();

	GetDlgItem(IDC_BTN_REGISTER)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_CAMERA)->EnableWindow(TRUE);

}

BOOL CMiniAIFaceDemoDlg::TerminateLoadThread()
{
	while (m_bLoadIconThreadRunning)
	{
		MSG message;
		while (::PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&message);
			::DispatchMessage(&message);
		}
	}

	::CloseHandle(m_hLoadIconThread);

	return TRUE;
}

BOOL CMiniAIFaceDemoDlg::ClearRegisterImages()
{
	if (m_bLoadIconThreadRunning)
	{
		TerminateLoadThread();
	}
	else
	{
		m_bClearFeatureThreadRunning = TRUE;

		m_hClearFeatureThread = CreateThread(
			NULL,
			0,
			ClearFaceFeatureOperation,//
			this,
			0,
			&m_dwClearFeatureThreadID);

		if (m_dwClearFeatureThreadID == NULL)
		{
			::CloseHandle(m_hClearFeatureThread);
		}
	}
	return 0;
}

BOOL CMiniAIFaceDemoDlg::CalculateShowPositon(IplImage*curSelectImage, Gdiplus::Rect& showRect)
{
	//Calculate the actual display width and height
	int actualWidth = 0;
	int actualHeight = 0;

	int imageWidth = curSelectImage->width;
	int imageHeight = curSelectImage->height;

	int windowWidth = m_windowViewRect.Width();
	int windowHeight = m_windowViewRect.Height();

	int paddingLeft = 0;
	int paddingTop = 0;

	//height based on width
	actualHeight = windowWidth*imageHeight / imageWidth;
	if (actualHeight > windowHeight)
	{
		//width based on height
		actualWidth = windowHeight*imageWidth / imageHeight;
		actualHeight = windowHeight;
	}
	else
	{
		actualWidth = windowWidth;
	}

	paddingLeft = (windowWidth - actualWidth) / 2;
	paddingTop = (windowHeight - actualHeight) / 2;

	showRect.X = paddingLeft;
	showRect.Y = paddingTop;
	showRect.Width = actualWidth;
	showRect.Height = actualHeight;

	return 0;
}


void CMiniAIFaceDemoDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
}


void CMiniAIFaceDemoDlg::OnBnClickedBtnCamera()
{
	// TODO: Add your control notification handler code here

	CString btnLabel;

	GetDlgItem(IDC_BTN_CAMERA)->GetWindowText(btnLabel);

	//Get the number and name of the camera
	g_cameraNum = listDevices(g_cameraName);

	//Prevent clicking the button too often
	Sleep(3000);

	if (btnLabel == "open camera")
	{

		GetDlgItem(IDC_EDIT_THRESHOLD)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_COMPARE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_RECOGNITION)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_CAMERA)->SetWindowText("Off camera");

		//FD thread
		m_hFDThread = CreateThread(
			NULL,
			0,
			RunFaceDetectOperation,
			this,
			0,
			&m_dwFDThreadID);

		if (m_hFDThread == NULL)
		{
			::CloseHandle(m_hFDThread);
		}

		m_bFDThreadRunning = TRUE;

		//FR thread
		m_hFRThread = CreateThread(
			NULL,
			0,
			RunFaceFeatureOperation,
			this,
			0,
			&m_dwFRThreadID);

		if (m_hFRThread == NULL)
		{
			::CloseHandle(m_hFRThread);
		}
	}
	else
	{
		GetDlgItem(IDC_BTN_COMPARE)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_THRESHOLD)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_RECOGNITION)->EnableWindow(TRUE);

		//Clear previously stored information
		m_curFaceInfo = { 0 };
		m_curVideoShowString = "";
		{
			lock_guard<mutex> lock(g_mutex);
			if (m_curVideoImage)
			{
				cvReleaseImage(&m_curVideoImage);
				m_curVideoImage = NULL;
			}

			if (m_curIrVideoImage)
			{
				cvReleaseImage(&m_curIrVideoImage);
				m_curIrVideoImage = NULL;
			}
		}

		m_dataValid = false;
		m_videoOpened = false;

		Sleep(600);

		ClearShowWindow();

		if (m_hFDThread == NULL)
		{
			BOOL res = ::CloseHandle(m_hFDThread);
			if (!res)
			{
				GetLastError();
			}
		}

		m_bFDThreadRunning = FALSE;

		if (m_hFRThread == NULL)
		{
			::CloseHandle(m_hFRThread);
		}

		GetDlgItem(IDC_BTN_CAMERA)->SetWindowText("open camera");
	}
}

unsigned long _stdcall RunFaceDetectOperation(LPVOID lpParam)
{
	CMiniAIFaceDemoDlg* dialog = (CMiniAIFaceDemoDlg*)(lpParam);

	if (dialog == nullptr)
	{
		return 1;
	}

	cv::Mat irFrame;
	cv::VideoCapture irCapture;

	cv::Mat rgbFrame;
	cv::VideoCapture rgbCapture;
	if (g_cameraNum == 2)
	{
		if (!irCapture.isOpened())
		{
			if (rgbCapture.open(RGB_CAMERA_ID) && irCapture.open(IR_CAMERA_ID))
				dialog->m_videoOpened = true;
		}

		if (!(rgbCapture.set(CV_CAP_PROP_FRAME_WIDTH, VIDEO_FRAME_DEFAULT_WIDTH) &&
			rgbCapture.set(CV_CAP_PROP_FRAME_HEIGHT, VIDEO_FRAME_DEFAULT_HEIGHT)))
		{
			AfxMessageBox(_T("RGB Camera init failed！"));
			return 1;
		}

		if (!(irCapture.set(CV_CAP_PROP_FRAME_WIDTH, VIDEO_FRAME_DEFAULT_WIDTH) &&
			irCapture.set(CV_CAP_PROP_FRAME_HEIGHT, VIDEO_FRAME_DEFAULT_HEIGHT)))
		{
			AfxMessageBox(_T("IR Camera init failed！"));
			return 1;
		}
	}
	else if (g_cameraNum == 1)
	{
		if (!rgbCapture.isOpened())
		{
			bool res = rgbCapture.open(0);
			if (res)
				dialog->m_videoOpened = true;
		}

		if (!(rgbCapture.set(CV_CAP_PROP_FRAME_WIDTH, VIDEO_FRAME_DEFAULT_WIDTH) &&
			rgbCapture.set(CV_CAP_PROP_FRAME_HEIGHT, VIDEO_FRAME_DEFAULT_HEIGHT)))
		{
			AfxMessageBox(_T("RGB Camera init failed！"));
			return 1;
		}
	}
	else
	{
		AfxMessageBox(_T("no support camera amount！"));
		return 1;
	}

	while (dialog->m_videoOpened)
	{
		if (g_cameraNum == 2)
		{
			irCapture >> irFrame;

			rgbCapture >> rgbFrame;

			MAI_SingleFaceInfo faceInfo = { 0 };

			IplImage rgbImage(rgbFrame);
			IplImage irImage(irFrame);

			MRESULT detectRes = dialog->m_videoFaceEngine.PreDetectFace(&rgbImage, faceInfo, true);
			/*FILE *fp = NULL;
			fp = fopen("rect.txt", "a+");
			if (fp)
			{
				fprintf(fp, "RGB: (%d %d %d %d)\n",
					faceInfo.faceRect.left, faceInfo.faceRect.top, faceInfo.faceRect.right, faceInfo.faceRect.bottom);
				fflush(fp);
				fclose(fp);
			}*/
			if (MOK == detectRes)
			{
				cvRectangle(&rgbImage, cvPoint(faceInfo.faceRect.left, faceInfo.faceRect.top),
					cvPoint(faceInfo.faceRect.right, faceInfo.faceRect.bottom), cvScalar(0, 0, 255), 2);

				cvRectangle(&irImage, cvPoint(faceInfo.faceRect.left, faceInfo.faceRect.top),
					cvPoint(faceInfo.faceRect.right, faceInfo.faceRect.bottom), cvScalar(0, 0, 255), 2);

				dialog->m_curFaceInfo = faceInfo;
				dialog->m_dataValid = true;
			}
			else
			{
				//Do not display information without face
				dialog->m_curVideoShowString = "";
				dialog->m_curIRVideoShowString = "";
				dialog->m_dataValid = false;
			}

			MAI_SingleFaceInfo irFaceInfo = { 0 };
			MRESULT irRes = dialog->m_videoFaceEngine.PreDetectFace(&irImage, irFaceInfo, false);
			if (irRes == MOK)
			{
				if (abs(faceInfo.faceRect.left - irFaceInfo.faceRect.left) < 20 &&
					abs(faceInfo.faceRect.top - irFaceInfo.faceRect.top) < 20 &&
					abs(faceInfo.faceRect.right - irFaceInfo.faceRect.right) < 20 &&
					abs(faceInfo.faceRect.bottom - irFaceInfo.faceRect.bottom) < 20)
				{
					dialog->m_irDataValid = true;
				}
				else
				{
					dialog->m_irDataValid = false;
				}
			}
			else
			{
				dialog->m_irDataValid = false;
			}

			//copy again
			{
				lock_guard<mutex> lock(g_mutex);
				cvReleaseImage(&dialog->m_curVideoImage);
				dialog->m_curVideoImage = cvCloneImage(&rgbImage);

				cvReleaseImage(&dialog->m_curIrVideoImage);
				dialog->m_curIrVideoImage = cvCloneImage(&irImage);
			}
		}
		else if (g_cameraNum == 1)
		{
			rgbCapture >> rgbFrame;

			MAI_SingleFaceInfo faceInfo = { 0 };

			IplImage rgbImage(rgbFrame);

			MRESULT detectRes = dialog->m_videoFaceEngine.PreDetectFace(&rgbImage, faceInfo, true);
			if (MOK == detectRes)
			{
				cvRectangle(&rgbImage, cvPoint(faceInfo.faceRect.left, faceInfo.faceRect.top),
					cvPoint(faceInfo.faceRect.right, faceInfo.faceRect.bottom), cvScalar(0, 0, 255), 2);

				dialog->m_curFaceInfo = faceInfo;
				dialog->m_dataValid = true;
			}
			else
			{
				//Do not display information without face
				dialog->m_curVideoShowString = "";
				dialog->m_dataValid = false;
			}


			//copy again
			{
				lock_guard<mutex> lock(g_mutex);
				cvReleaseImage(&dialog->m_curVideoImage);
				dialog->m_curVideoImage = cvCloneImage(&rgbImage);
			}
		}
		else
		{
			AfxMessageBox(_T("no support camera amount！"));
		}
		
		dialog->SendMessage(WM_PAINT);
	}

	rgbCapture.release();
	irCapture.release();

	return 0;
}

unsigned long _stdcall RunFaceFeatureOperation(LPVOID lpParam)
{
	CMiniAIFaceDemoDlg* dialog = (CMiniAIFaceDemoDlg*)(lpParam);

	if (dialog == nullptr)
	{
		return 1;
	}

	//Set the liveness detection threshold, the sdk internal default RGB: 0.5 IR: 0.7, you can choose whether to call this interface
	dialog->m_imageFaceEngine.SetLivenessThreshold(RGB_LIVE_THRESHOLD, IR_LIVE_THRESHOLD);

	//initialization feature
	MAI_FaceFeature faceFeature = { 0 };
	faceFeature.featureSize = FACE_FEATURE_SIZE;
	faceFeature.feature = (MByte *)malloc(faceFeature.featureSize * sizeof(MByte));

	MAI_MultiFaceInfo multiFaceInfo = { 0 };
	multiFaceInfo.faceOrient = (MInt32*)malloc(sizeof(MInt32));
	multiFaceInfo.faceRect = (MRECT*)malloc(sizeof(MRECT));

	while (dialog->m_bFDThreadRunning)
	{
		if (dialog->m_bLoadIconThreadRunning ||
			dialog->m_bClearFeatureThreadRunning)
		{
			//Do not display messages during loading and clearing of registry
			dialog->m_curVideoShowString = "";
			continue;
		}

		if (!dialog->m_dataValid)
		{
			continue;
		}

		//Make a copy first to prevent read-write conflicts
		IplImage* tempImage = NULL;
		{
			lock_guard<mutex> lock(g_mutex);
			if (dialog->m_curVideoImage)
			{
				tempImage = cvCloneImage(dialog->m_curVideoImage);
			}
		}

		//send a copy to live
		MAI_SingleFaceInfo singleFaceInfo = dialog->m_curFaceInfo;

		multiFaceInfo.faceNum = 1;
		multiFaceInfo.faceOrient[0] = dialog->m_curFaceInfo.faceOrient;
		multiFaceInfo.faceRect[0] = dialog->m_curFaceInfo.faceRect;

		MAI_AgeInfo ageInfo = { 0 };
		MAI_GenderInfo genderInfo = { 0 };
		MAI_Face3DAngle angleInfo = { 0 };
		MAI_LivenessInfo liveNessInfo = { 0 };

		//IR Liveness Detection
		bool isIRAlive = false;
		if (g_cameraNum == 2)
		{
			IplImage* tempIRImage = NULL;
			lock_guard<mutex> lock(g_mutex);
			{
				if (dialog->m_curIrVideoImage)
				{
					tempIRImage = cvCloneImage(dialog->m_curIrVideoImage);
				}
			}
			
			if (dialog->m_irDataValid)
			{
				MAI_LivenessInfo irLiveNessInfo = { 0 };
				MRESULT irRes = dialog->m_imageFaceEngine.FaceMAIProcess_IR(multiFaceInfo, tempIRImage, irLiveNessInfo);
				if (irRes == 0 && irLiveNessInfo.num > 0)
				{
					if (irLiveNessInfo.isLive[0] == 1)
					{
						dialog->m_curIRVideoShowString = "IR Liveness";
						isIRAlive = true;
					}
					else if (irLiveNessInfo.isLive[0] == 0)
					{
						dialog->m_curIRVideoShowString = "IR Fake";
					}
					else
					{
						//-1: Not sure; -2: The number of incoming faces > 1; -3: The face is too small; -4: The angle is too large; -5: The face exceeds the boundary 
						dialog->m_curIRVideoShowString = "unknown";
					}
				}
				else
				{
					dialog->m_curIRVideoShowString = "";
				}
			}
			else
			{
				dialog->m_curIRVideoShowString = "";
			}

			cvReleaseImage(&tempIRImage);
		}
		else if (g_cameraNum == 1)
		{
			isIRAlive = true;
		}
		else
		{
			break;
		}

		//RGB attribute detection
		MRESULT detectRes = dialog->m_imageFaceEngine.FaceMAIProcess(multiFaceInfo, tempImage,
			ageInfo, genderInfo, angleInfo, liveNessInfo);

		bool isRGBAlive = false;
		if (detectRes == 0 && liveNessInfo.num > 0)
		{
			if (liveNessInfo.isLive[0] == 1)
			{
				isRGBAlive = true;
			}
			else if (liveNessInfo.isLive[0] == 0)
			{
				dialog->m_curVideoShowString = "RGB Fake";
			}
			else
			{
				//-1: Not sure; -2: The number of incoming faces > 1; -3: The face is too small; -4: The angle is too large; -5: The face exceeds the boundary
				dialog->m_curVideoShowString = "unknown";  
			}
		}
		else
		{
			dialog->m_curVideoShowString = "";
		}

		if (!(isRGBAlive && isIRAlive))
		{
			if (isRGBAlive && !isIRAlive)
			{
				dialog->m_curVideoShowString = "RGB Liveness";
			}
			cvReleaseImage(&tempImage);
			continue;
		}

		//feature extraction
		detectRes = dialog->m_videoFaceEngine.PreExtractFeature(tempImage,
			faceFeature, singleFaceInfo);

		cvReleaseImage(&tempImage);

		if (MOK != detectRes)
		{
			continue;
		}

		int maxIndex = 0;
		MFloat maxThreshold = 0.0;
		int curIndex = 0;

		if (dialog->m_bLoadIconThreadRunning ||
			dialog->m_bClearFeatureThreadRunning)
		{
			continue;
		}

		for each (auto regisFeature in dialog->m_featuresVec)
		{
			curIndex++;
			MFloat confidenceLevel = 0;
			MRESULT pairRes = dialog->m_videoFaceEngine.FacePairMatching(confidenceLevel, faceFeature, regisFeature);

			if (MOK == pairRes && confidenceLevel > maxThreshold)
			{
				maxThreshold = confidenceLevel;
				maxIndex = curIndex;
			}
		}

		if (atof(dialog->m_strEditThreshold) >= 0 &&
			maxThreshold >= atof(dialog->m_strEditThreshold) &&
			isRGBAlive && isIRAlive)
		{
			CString resStr;
			resStr.Format("No.%d :%.2f", maxIndex, maxThreshold);
			dialog->m_curVideoShowString = resStr + ",RGB Liveness";
		}
		else if (isRGBAlive)
		{
			dialog->m_curVideoShowString = "RGB Liveness";
		}
	}
	
	SafeFree(multiFaceInfo.faceOrient);
	SafeFree(multiFaceInfo.faceRect);
	SafeFree(faceFeature.feature);
	return 0;
}


//double buffered drawing
void CMiniAIFaceDemoDlg::IplDrawToHDC(BOOL isVideoMode, IplImage* rgbImage, CRect& strShowRect, UINT ID)
{
	if (!rgbImage)
	{
		return;
	}

	if (rgbImage->width <= 0 || rgbImage->height <= 0)
	{
		return;
	}

	CDC MemDC;

	CClientDC pDc(GetDlgItem(ID));

	//Create a memory DC (MyDC) compatible with the window DC
	MemDC.CreateCompatibleDC(&pDc);
	
	IplImage* cutImg;
	if (m_curIrVideoImage)
	{
		//Scaling and copying of infrared images
		IplImage* shrinkIrImage = cvCreateImage(cvSize(m_curIrVideoImage->width / 3, m_curIrVideoImage->height / 3), m_curIrVideoImage->depth, m_curIrVideoImage->nChannels);
		cvResize(m_curIrVideoImage, shrinkIrImage, CV_INTER_AREA);

		//Fusion IR image onto RGB image
		cv::Mat matRGBImage = cv::cvarrToMat(rgbImage);
		cv::Mat matIRImage = cv::cvarrToMat(shrinkIrImage);
		cv::Mat imageROI = matRGBImage(cv::Rect(10, 10, matIRImage.cols, matIRImage.rows));
		matIRImage.copyTo(imageROI);
		IplImage* roiImage = &IplImage(matRGBImage);	//shallow copy

		//crop picture
		cutImg = cvCreateImage(cvSize(roiImage->width - (roiImage->width % 4), roiImage->height), IPL_DEPTH_8U, roiImage->nChannels);
		PicCutOut(roiImage, cutImg, 0, 0);
		cvReleaseImage(&shrinkIrImage);
	}
	else
	{
		cutImg = cvCreateImage(cvSize(rgbImage->width - (rgbImage->width % 4), rgbImage->height), IPL_DEPTH_8U, rgbImage->nChannels);
		PicCutOut(rgbImage, cutImg, 0, 0);
	}

	CBitmap* bmp = IplImage2CBitmap(cutImg);

	//Select the memory bitmap into the memory DC to save the graphics drawn in the memory DC
	CBitmap *oldbmp = MemDC.SelectObject(bmp);

	CPen pen(PS_SOLID, 4, RGB(255, 0, 0));
	pDc.SelectStockObject(NULL_BRUSH);

	pDc.SetBkMode(TRANSPARENT);
	pDc.SetTextColor(RGB(0, 0, 255));

	CRect rect;
	GetDlgItem(ID)->GetClientRect(&rect);

	//Paste the graphics in the memory DC into the window；
	pDc.SetStretchBltMode(HALFTONE);

	strShowRect.left = strShowRect.left < 0 ? 0 : strShowRect.left;
	strShowRect.top = strShowRect.top < 0 ? 0 : strShowRect.top;
	strShowRect.right = strShowRect.right > rect.right ? 0 : strShowRect.right;
	strShowRect.bottom = strShowRect.bottom > rect.bottom ? rect.bottom : strShowRect.bottom;

	if (isVideoMode)
	{
		pDc.StretchBlt(0, 0, rect.Width(), rect.Height(), &MemDC, 0, 0, VIDEO_FRAME_DEFAULT_WIDTH, VIDEO_FRAME_DEFAULT_HEIGHT, SRCCOPY);

		//In order to make the text not stick to the edge
		strShowRect.left += 4;
		strShowRect.top += 4;

		//Make the text not exceed the video frame
		GetDlgItem(ID)->SetFont(m_Font);

		SIZE size;
		GetTextExtentPoint32A(pDc, m_curVideoShowString, (int)strlen(m_curVideoShowString), &size);

		if (strShowRect.left + size.cx > rect.Width())
		{
			strShowRect.left = rect.Width() - size.cx;
		}
		if (strShowRect.top + size.cy > rect.Height())
		{
			strShowRect.top = rect.Height() - size.cy;
		}

		//Draw comparison information
		if (m_curVideoShowString == "RGB Fake")
		{
			pDc.SetTextColor(RGB(255, 242, 0));
		}
		pDc.DrawText(m_curVideoShowString, &strShowRect, DT_TOP | DT_LEFT | DT_NOCLIP);

		if (m_curIRVideoShowString == "IR Fake")
		{
			pDc.SetTextColor(RGB(255, 242, 0));
		}
		pDc.DrawText(m_curIRVideoShowString, CRect(20,20,100,100), DT_TOP | DT_LEFT | DT_NOCLIP);
	}
	else
	{
		//The image needs to redraw the background due to inconsistent size
		HBRUSH hBrush = ::CreateSolidBrush(RGB(255, 255, 255));
		::FillRect(pDc.m_hDC, CRect(0, 0, m_windowViewRect.Width(), m_windowViewRect.Height()), hBrush);

		pDc.StretchBlt(m_curImageShowRect.X + 2, m_curImageShowRect.Y + 2,
			m_curImageShowRect.Width - 2, m_curImageShowRect.Height - 5, &MemDC, 0, 0, cutImg->width, cutImg->height, SRCCOPY);

		Gdiplus::Graphics graphics(pDc.m_hDC);
		Gdiplus::Pen pen(Gdiplus::Color::Red, 2);
		graphics.DrawRectangle(&pen, m_curFaceShowRect);

		//Draw age gender information
		pDc.DrawText(m_curStaticShowAgeGenderString, &strShowRect, DT_TOP | DT_LEFT | DT_NOCLIP);

		//Put the comparison information under the age gender information
		strShowRect.top += 20;
		strShowRect.bottom += 20;

		//Make the text not exceed the video frame
		GetDlgItem(ID)->SetFont(m_Font);

		SIZE size;
		GetTextExtentPoint32A(pDc, m_curVideoShowString, (int)strlen(m_curVideoShowString), &size);

		if (strShowRect.left + size.cx > rect.Width())
		{
			strShowRect.left = rect.Width() - size.cx;
		}
		if (strShowRect.top + size.cy > rect.Height())
		{
			strShowRect.top = rect.Height() - size.cy;
		}

		//Draw comparison information
		pDc.DrawText(m_curStaticShowCmpString, &strShowRect, DT_TOP | DT_LEFT | DT_NOCLIP);
	}

	cvReleaseImage(&cutImg);

	//Select the original bitmap, delete the memory bitmap object and memory DC
	MemDC.SelectObject(oldbmp);
	bmp->DeleteObject();
	MemDC.DeleteDC();

}


//Image format conversion
CBitmap* IplImage2CBitmap(const IplImage *img)
{
	if (!img)
	{
		return NULL;
	}

	CBitmap* bitmap = new CBitmap;//new a CWnd object
	BITMAPINFO bmpInfo;  //create bitmap        
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = img->width;
	bmpInfo.bmiHeader.biHeight = img->origin ? abs(img->height) : -abs(img->height);//img->height;//high
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = 24;
	bmpInfo.bmiHeader.biCompression = BI_RGB;

	void *pArray = NULL;
	HBITMAP hbmp = CreateDIBSection(NULL, &bmpInfo, DIB_RGB_COLORS, &pArray, NULL, 0);//Create a DIB, which can be written directly and has nothing to do with the device, which is equivalent to creating a bitmap window
	ASSERT(hbmp != NULL);
	UINT uiTotalBytes = img->width * img->height * 3;
	memcpy(pArray, img->imageData, uiTotalBytes);

	bitmap->Attach(hbmp);// The HWND member of a CWnd object points to the window handle

	return bitmap;
}

void CMiniAIFaceDemoDlg::OnEnChangeEditThreshold()
{
	//update threshold
	UpdateData(TRUE);
	if (atof(m_strEditThreshold) < 0)
	{
		AfxMessageBox(_T("threshold must be bigger than 0！"));
		SetDlgItemTextA(IDC_EDIT_THRESHOLD, "0.80");
	}

}


void CMiniAIFaceDemoDlg::OnClose()
{
	// TODO: Add message handler code and/or call defaults here
	if (m_videoOpened)
	{
		AfxMessageBox(_T("please close the camera！"));
		return;
	}

	CDialogEx::OnClose();

	m_bLoadIconThreadRunning = FALSE;
	TerminateLoadThread();
	m_bClearFeatureThreadRunning = FALSE;
	ClearRegisterImages();

	m_videoOpened = false;
	Sleep(500);

	m_bFDThreadRunning = FALSE;

	::CloseHandle(m_hFDThread);
	::CloseHandle(m_hFRThread);

	Sleep(500);

	m_imageFaceEngine.UnInitEngine();
	m_videoFaceEngine.UnInitEngine();
}

void CMiniAIFaceDemoDlg::ClearShowWindow()
{
	//clear background
	CDC* pCDC = GetDlgItem(IDC_STATIC_VIEW)->GetDC();
	HDC hDC = pCDC->m_hDC;
	HBRUSH hBrush = ::CreateSolidBrush(RGB(255, 255, 255));
	::FillRect(hDC, CRect(0, 0, m_windowViewRect.Width(), m_windowViewRect.Height()), hBrush);
	DeleteObject(hBrush);
}

void CMiniAIFaceDemoDlg::showActiveWindow(int showOption)
{
	GetDlgItem(IDC_STATIC_HWID)->ShowWindow(showOption);
	GetDlgItem(IDC_STATIC_COMMENT)->ShowWindow(showOption);
	GetDlgItem(IDC_STATIC_LICENSE)->ShowWindow(showOption);
	GetDlgItem(IDC_EDIT_HWID)->ShowWindow(showOption);
	GetDlgItem(IDC_EDIT_LICENSE)->ShowWindow(showOption);
	GetDlgItem(IDC_BTN_ACTIVATE)->ShowWindow(showOption);

	if (showOption == SW_SHOW)
	{
		char hwid[256] = { 0 };
		MAIGetCurrentHWID(hwid, sizeof(hwid));
		
		m_editHwid.SetWindowTextA(hwid);

		GetDlgItem(IDC_BTN_RECOGNITION)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_REGISTER)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_CLEAR)->EnableWindow(FALSE);
		GetDlgItem(IDC_STATIC)->EnableWindow(FALSE);
	}
	else {
		GetDlgItem(IDC_BTN_RECOGNITION)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_REGISTER)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_CLEAR)->EnableWindow(TRUE);
		GetDlgItem(IDC_STATIC)->EnableWindow(TRUE);
	}
}

void CMiniAIFaceDemoDlg::initEngine()
{
	MRESULT faceRes;
	CString resStr;

	resStr = "";
	faceRes = m_imageFaceEngine.InitEngine(MAI_DETECT_MODE_IMAGE);//Image
	resStr.Format("IMAGE mode init result: %d", faceRes);
	EditOut(resStr, TRUE);

	resStr = "";
	faceRes = m_videoFaceEngine.InitEngine(MAI_DETECT_MODE_VIDEO);//Video
	resStr.Format("VIDEO mode init result: %d", faceRes);
	EditOut(resStr, TRUE);
}


unsigned long _stdcall ClearFaceFeatureOperation(LPVOID lpParam)
{
	CMiniAIFaceDemoDlg* dialog = (CMiniAIFaceDemoDlg*)(lpParam);

	if (dialog == nullptr)
	{
		return 1;
	}

	int iImageCount = dialog->m_IconImageList.GetImageCount();

	dialog->m_IconImageList.Remove(-1);

	dialog->m_ImageListCtrl.DeleteAllItems();

	iImageCount = dialog->m_IconImageList.SetImageCount(0);

	//clear feature
	for (auto feature : dialog->m_featuresVec)
	{
		free(feature.feature);
	}

	dialog->m_featuresVec.clear();

	dialog->m_bClearFeatureThreadRunning = FALSE;

	return 0;
}


BOOL SetTextFont(CFont* font, int fontHeight, int fontWidth, string fontStyle)
{
	return font->CreateFont(
		fontHeight,					// nHeight
		fontWidth,					// nWidth
		0,							// nEscapement
		0,							// nOrientation
		FW_BOLD,					// nWeight
		FALSE,						// bItalic
		FALSE,						// bUnderline
		0,							// cStrikeOut
		DEFAULT_CHARSET,				// nCharSet
		OUT_DEFAULT_PRECIS,			// nOutPrecision
		CLIP_DEFAULT_PRECIS,			// nClipPrecision
		DEFAULT_QUALITY,				// nQuality
		DEFAULT_PITCH | FF_SWISS,		// nPitchAndFamily
		fontStyle.c_str());			// lpszFacename
}

//list hardware devices
int listDevices(vector<string>& list)
{
	ICreateDevEnum *pDevEnum = NULL;
	IEnumMoniker *pEnum = NULL;
	int deviceCounter = 0;
	CoInitialize(NULL);

	HRESULT hr = CoCreateInstance(
		CLSID_SystemDeviceEnum,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_ICreateDevEnum,
		reinterpret_cast<void**>(&pDevEnum)
	);

	if (SUCCEEDED(hr))
	{
		hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnum, 0);
		if (hr == S_OK) {

			IMoniker *pMoniker = NULL;
			while (pEnum->Next(1, &pMoniker, NULL) == S_OK)
			{
				IPropertyBag *pPropBag;
				hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag,
					(void**)(&pPropBag));

				if (FAILED(hr)) {
					pMoniker->Release();
					continue; // Skip this one, maybe the next one will work.
				}

				VARIANT varName;
				VariantInit(&varName);
				hr = pPropBag->Read(L"Description", &varName, 0);
				if (FAILED(hr))
				{
					hr = pPropBag->Read(L"FriendlyName", &varName, 0);
				}

				if (SUCCEEDED(hr))
				{
					hr = pPropBag->Read(L"FriendlyName", &varName, 0);
					int count = 0;
					char tmp[255] = { 0 };
					while (varName.bstrVal[count] != 0x00 && count < 255)
					{
						tmp[count] = (char)varName.bstrVal[count];
						count++;
					}
					list.push_back(tmp);
				}

				pPropBag->Release();
				pPropBag = NULL;
				pMoniker->Release();
				pMoniker = NULL;

				deviceCounter++;
			}

			pDevEnum->Release();
			pDevEnum = NULL;
			pEnum->Release();
			pEnum = NULL;
		}
	}
	return deviceCounter;
}


void CMiniAIFaceDemoDlg::OnBnClickedBtnActivate()
{
	// TODO: Add your control notification handler code here
	//Set activation info

	CString resStr = "";
	
	m_editLicense.GetWindowTextA(m_license);
	m_imageFaceEngine.SetActiveInfo(m_license, m_license.GetLength());

	MRESULT activeRes = m_imageFaceEngine.CheckActiveSDK();
	resStr.Format("Activation Result: %s\n", activeRes == MAI_ACTIVE_SUCCESS ? "Success" : "Failed");
	EditOut(resStr, TRUE);

	if (activeRes == MAI_ACTIVE_SUCCESS)
	{
		initEngine();
		showActiveWindow(SW_HIDE);
	}
	else
	{
		AfxMessageBox("Activate Failed!", MB_OK | MB_ICONINFORMATION);
	}
	
}
