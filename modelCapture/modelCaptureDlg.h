
// modelCaptureDlg.h : header file
//

#pragma once
#include <memory>
#include <vector>
#include <string>
#include <osg/Vec3d>
#include "afxwin.h"
#include "SnapPara.h"
#include <osg/MatrixTransform>

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
	afx_msg void OnEnChangeRadius7();
	afx_msg void OnEnChangeRadius8();
	afx_msg void OnEnChangeRadius9();
	afx_msg void OnBnClickedButton13();
	afx_msg void OnBnClickedButton14();

	double mPitch;
	double mYaw;
	double mRoll;

	osg::Matrix mCorrectMat;
	

	void rotateModel(double pitch, double yaw, double roll);
	
	
	afx_msg void OnBnClickedButton15();
	afx_msg void OnBnClickedButton16();
	afx_msg void OnBnClickedButton17();
	afx_msg void OnBnClickedButton18();

protected:
	/*
	** brief 从文件读取面部特征点
	** param vecXyz 特征点编号及坐标
	*/

	bool readFaceKeyPoints(std::vector<std::pair<int, osg::Vec3d>> &vecXyz);
	
	/*
	** brief 根据特征点校正模型，使得面部中心在（0，0，0），头部向上方向（0， 0， 1），面部正面方向（0， -1，0）
	** param vecXyz 特征点编号及坐标
	*/
	bool correctModel(std::vector<std::pair<int, osg::Vec3d>> vecXyz, osg::ref_ptr<osg::MatrixTransform> trans);

	bool calculateDist(std::vector<std::pair<int, osg::Vec3d>> vecXyz);

	bool calculateUpDir(std::vector<osg::Vec3d> xyzs, osg::Matrix &mat);
public:
	afx_msg void OnBnClickedloadfacemaskpath3();
};
