
// MiniAIFaceDemoDlg.h : head File
//

#pragma once
#include "afxcmn.h"
#include "MiniAIFaceEngine.h"

#include <memory>
#include <string>

//transfer GDI+
#include <GdiPlus.h>
#include "afxwin.h"
#pragma comment(lib, "Gdiplus.lib")
using namespace Gdiplus;


// CMiniAIFaceDemoDlg dialog box
class CMiniAIFaceDemoDlg : public CDialogEx
{
	// structure
public:
	CMiniAIFaceDemoDlg(CWnd* pParent = NULL);	// standard constructor
	~CMiniAIFaceDemoDlg();
	// dialog data
	enum { IDD = IDD_MINIAIFACEDEMO_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


	// accomplish
protected:
	HICON m_hIcon;

	// The generated message map function
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedBtnRegister();
	afx_msg void OnBnClickedBtnRecognition();
	afx_msg void OnBnClickedBtnCompare();
	afx_msg void OnBnClickedBtnClear();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedBtnCamera();
	afx_msg void OnEnChangeEditThreshold();

	void EditOut(CString str, bool add_endl);
	void IplDrawToHDC(BOOL isVideoMode, IplImage* rgbImage, CRect& showRect, UINT ID);

private:
	void LoadThumbnailImages();
	CString SelectFolder();
	BOOL TerminateLoadThread();
	BOOL ClearRegisterImages();
	BOOL CalculateShowPositon(IplImage*curSelectImage, Gdiplus::Rect& showRect);
	MRESULT StaticImageFaceOp(IplImage* image);
	void ClearShowWindow();
	void showActiveWindow(int);
	void initEngine();
public:
	CListCtrl m_ImageListCtrl;
	CImageList m_IconImageList;
	CEdit m_editLog;
	CEdit m_editLicense;
	CEdit m_editHwid;

	CString m_folderPath;
	CString m_license;
	std::vector<MAI_FaceFeature> m_featuresVec;

	BOOL m_bLoadIconThreadRunning;
	DWORD m_dwLoadIconThreadID;
	HANDLE m_hLoadIconThread;

	BOOL m_bClearFeatureThreadRunning;
	DWORD m_dwClearFeatureThreadID;
	HANDLE m_hClearFeatureThread;

	BOOL m_bFDThreadRunning;
	DWORD m_dwFDThreadID;
	HANDLE m_hFDThread;

	BOOL m_bFRThreadRunning;
	DWORD m_dwFRThreadID;
	HANDLE m_hFRThread;

	MiniAIFaceEngine m_imageFaceEngine;
	MiniAIFaceEngine m_videoFaceEngine;

	IplImage* m_curStaticImage;					//currently selected image
	MAI_FaceFeature m_curStaticImageFeature;	//Facial features of the current picture
	BOOL m_curStaticImageFRSucceed;
	Gdiplus::Rect m_curFaceShowRect;
	Gdiplus::Rect m_curImageShowRect;

	CString m_curStaticShowAgeGenderString;

	CString m_curStaticShowCmpString;

	IplImage* m_curVideoImage;
	IplImage* m_curIrVideoImage;
	MAI_SingleFaceInfo m_curFaceInfo;
	bool m_dataValid;
	bool m_irDataValid;

	bool m_videoOpened;
	CString m_strEditThreshold;
	CString m_strEditLicense;

	Gdiplus::PointF m_curStringShowPosition;	//The position where the current string is displayed
	CString m_curVideoShowString;
	CString m_curIRVideoShowString;
	CFont* m_Font;

private:
	CRect m_windowViewRect;						//Display the size of the control

public:
	afx_msg void OnClose();
	afx_msg void OnBnClickedBtnActivate();
};
