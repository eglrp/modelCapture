
// modelCaptureDlg.h : header file
//

#pragma once
#include <memory>
#include <vector>
#include <string>
#include <osg/Vec3d>
#include "afxwin.h"
#include "SnapPara.h"

namespace capture
{
	class ICapture;
}

struct threadPara
{
	double x;
	double y;
	double z;
	std::string snapFile;
	osg::Vec3d up;
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

	void startThread(std::vector<threadPara> vecPara, osg::ref_ptr<osg::Node> node,
		int imageWidth, int imageHeight, osg::Vec3d center, osg::Vec3d up);


// Implementation
protected:
	HICON m_hIcon;

	std::shared_ptr<capture::ICapture> iCapture;

public:
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
	std::shared_ptr<capture::CSnapPara> mSnapPara;
	afx_msg void OnStnClickedInterval();
	afx_msg void OnEnChangeRadius4();
	afx_msg void OnEnChangeRadius();
	afx_msg void OnEnChangeRadius3();
	afx_msg void OnEnChangeRadius5();
	afx_msg void OnEnChangeRadius6();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton9();
	afx_msg void OnBnClickedButton11();
	afx_msg void OnBnClickedButton7();
	afx_msg void OnBnClickedButton8();
	afx_msg void OnBnClickedButton12();
	afx_msg void OnBnClickedButton10();
	afx_msg void OnBnClickedCheck1();
	BOOL mUpSideDown;

protected:
	bool judgeLatOutSide();

	bool judgeLonOutSide();

public:
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedRadio3();
	afx_msg void OnBnClickedRadio4();
	afx_msg void OnBnClickedRadio5();
	afx_msg void OnBnClickedRadio6();
	int mManualSetMain = 0;
};
