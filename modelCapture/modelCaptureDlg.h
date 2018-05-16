
// modelCaptureDlg.h : header file
//

#pragma once
#include <memory>
#include <vector>
#include <string>
#include <osg/Vec3d>
#include "afxwin.h"

struct threadPara
{
	double x;
	double y;
	double z;
	std::string snapFile;
};


// CmodelCaptureDlg dialog
class CmodelCaptureDlg : public CDialogEx
{
// Construction
public:
	CmodelCaptureDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_MODELCAPTURE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	void startThread(std::vector<threadPara> vecPara, std::string modelFileName,
		int imageWidth, int imageHeight, osg::Vec3d center, osg::Vec3d up);


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedloadsnapsavepath();
	afx_msg void OnBnClickedImage();
	afx_msg void OnBnClickedPreview();
	afx_msg void OnBnClickedloadsnapsavepath2();

public:

	CString mModelPath;

	CString mSnapSavePath;

	int mWidth;

	int mHeight;

	double mR;
	int mInterval;
};
