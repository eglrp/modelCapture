
// modelCapture.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "modelCapture.h"
#include "modelCaptureDlg.h"
#include <vector>
#include <sstream> 
#include "SnapPara.h"
#include <osg/Vec3d>
#include <osgDB/ReadFile>
#include "ICapture.h"

using namespace std;
using namespace capture;
using namespace osg;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


vector<string> split_black_space(string s){
	stringstream ss;
	ss << s;//存到字符流里  
	vector<string> vs;
	string word;
	while (ss >> word){//从字符流里读单词，会天然地过滤到多余的空格  
		vs.push_back(word);
	}
	return vs;
}

//模板函数：将string类型变量转换为常用的数值类型（此方法具有普遍适用性）  
template <class Type>
Type stringToNum(const string& str)
{
	istringstream iss(str);
	Type num;
	iss >> num;
	return num;
}

// CmodelCaptureApp

BEGIN_MESSAGE_MAP(CmodelCaptureApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CmodelCaptureApp construction

CmodelCaptureApp::CmodelCaptureApp()
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CmodelCaptureApp object

CmodelCaptureApp theApp;


// CmodelCaptureApp initialization

BOOL CmodelCaptureApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
	CShellManager *pShellManager = new CShellManager;

	// Activate "Windows Native" visual manager for enabling themes in MFC controls
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	CString cmdLines = theApp.m_lpCmdLine;
	string strCmd = cmdLines;
	vector<string> strs = split_black_space(strCmd);

	if (strs.empty())
	{
		CmodelCaptureDlg dlg;
		m_pMainWnd = &dlg;
		INT_PTR nResponse = dlg.DoModal();
		if (nResponse == IDOK)
		{
			// TODO: Place code here to handle when the dialog is
			//  dismissed with OK
		}
		else if (nResponse == IDCANCEL)
		{
			// TODO: Place code here to handle when the dialog is
			//  dismissed with Cancel
		}
		else if (nResponse == -1)
		{
			TRACE(traceAppMsg, 0, "Warning: dialog creation failed, so application is terminating unexpectedly.\n");
			TRACE(traceAppMsg, 0, "Warning: if you are using MFC controls on the dialog, you cannot #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS.\n");
		}

		// Delete the shell manager created above.
		if (pShellManager != NULL)
		{
			delete pShellManager;
		}

	}
	else
	{
		if (strs.size() == 12)
		{
			int hasUi = atoi(strs[0].c_str());
			shared_ptr<CSnapPara> para(new CSnapPara());
			para->mImageHeight = 3000;
			para->mImageWidth = 3000;
			para->mRadius = 100;
			para->mIntervalX = 5;
			para->mIntervalY = 5;
			para->mMinLatitude = -7;
			para->mMaxLatitude = 1;
			para->mMinLongitude = -60;
			para->mMaxLongitude = 60;

			if (strs[1] != "none")
			{
				para->mSceneFileName = strs[1].c_str();
			}

			if (strs[2] != "none")
			{
				para->mFaceMaskFileName = strs[2].c_str();
			}

			if (strs[3] != "none")
			{
				para->mOutFile = strs[3].c_str();
			}

			if (strs[4] != "none")
			{
				para->mImageHeight = stringToNum<int>(strs[4].c_str());
			}

			if (strs[5] != "none")
			{
				para->mImageWidth = stringToNum<int>(strs[5].c_str());
			}

			if (strs[6] != "none")
			{
				para->mIntervalX = stringToNum<double>(strs[6].c_str());
			}

			if (strs[7] != "none")
			{
				para->mIntervalY = stringToNum<double>(strs[7].c_str());
			}

			if (strs[8] != "none")
			{
				para->mMinLatitude = stringToNum<double>(strs[8].c_str());
			}

			if (strs[9] != "none")
			{
				para->mMaxLatitude = stringToNum<double>(strs[9].c_str());
			}

			if (strs[10] != "none")
			{
				para->mMinLongitude = stringToNum<double>(strs[10].c_str());
			}

			if (strs[11] != "none")
			{
				para->mMaxLongitude = stringToNum<double>(strs[11].c_str());
			}

			if (hasUi)
			{
				CmodelCaptureDlg dlg(para);
				m_pMainWnd = &dlg;
				dlg.loadModel();
				INT_PTR nResponse = dlg.DoModal();
				if (nResponse == IDOK)
				{
					// TODO: Place code here to handle when the dialog is
					//  dismissed with OK
				}
				else if (nResponse == IDCANCEL)
				{
					// TODO: Place code here to handle when the dialog is
					//  dismissed with Cancel
				}
				else if (nResponse == -1)
				{
					TRACE(traceAppMsg, 0, "Warning: dialog creation failed, so application is terminating unexpectedly.\n");
					TRACE(traceAppMsg, 0, "Warning: if you are using MFC controls on the dialog, you cannot #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS.\n");
				}

				// Delete the shell manager created above.
				if (pShellManager != NULL)
				{
					delete pShellManager;
				}
			}
			else if (!hasUi)
			{
				CmodelCaptureDlg dlg(para);
				dlg.run();
			}

			return FALSE;
		}
		else if (strs.size() == 18)
		{
			string modelName = strs[0].c_str();
			string outFileName = strs[1].c_str();
			double focal = stringToNum<double>(strs[2].c_str());
			double ccdSize = stringToNum<double>(strs[3].c_str());

			//t0 = 0.125821, t1 = -0.028482, t2 = 0.897292
			double tx = stringToNum<double>(strs[4].c_str());;
			double ty = stringToNum<double>(strs[5].c_str());;
			double tz = stringToNum<double>(strs[6].c_str());;
			double imageWidth = stringToNum<double>(strs[7].c_str());;
			double imageHeight = stringToNum<double>(strs[8].c_str());;

			double r1 = stringToNum<double>(strs[9].c_str());
			double r2 = stringToNum<double>(strs[10].c_str());
			double r3 = stringToNum<double>(strs[11].c_str());
			double r4 = stringToNum<double>(strs[12].c_str());
			double r5 = stringToNum<double>(strs[13].c_str());
			double r6 = stringToNum<double>(strs[14].c_str());
			double r7 = stringToNum<double>(strs[15].c_str());
			double r8 = stringToNum<double>(strs[16].c_str());
			double r9 = stringToNum<double>(strs[17].c_str());
			
			snap(modelName, focal, ccdSize, imageHeight, imageWidth, outFileName, tx, ty, tz, r1, r2, r3, r4, r5, r6, r7, r8, r9);

			return FALSE;
		}

		return TRUE;
		
	}


	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}


void CmodelCaptureApp::snap(string modelName, double focal, double ccdSize, double imageHeight, double imageWidth, string outFileName, double t1x, double t1y, double t1z,
	double r1, double r2, double r3, double r4, double r5, double r6, double r7, double r8, double r9)
{
	double db[16] = { 0 };
	db[0] = r1;			db[1] = r2;			db[2] = r3;			db[3] = 0.0;
	db[4] = r4;			db[5] = r5;			db[6] = r6;			db[7] = 0.0;
	db[8] = r7;			db[9] = r8;			db[10] = r9;		db[11] = 0.0;
	db[12] = 0.0;	    db[13] = 0.0;       db[14] = 0.0;       db[15] = 1.0;

	ref_ptr<Node> node = osgDB::readNodeFile(modelName);

	if (!node)
	{
		return;
	}

	double focalLength = focal * ccdSize;
	double height = imageHeight * ccdSize;
	double width = imageWidth * ccdSize;
	double angle = atan(height / 2 / focalLength);
	double fov = (angle * 2) * 180 / PI;

	Vec3d t1(t1x, t1y, t1z);

	Matrix mat;
	mat.set(db);
	Matrix invertMat = mat.inverse(mat);
	Vec3d eye1 = -(invertMat * t1);
	double size = 3.5;
	Vec3d up = mat.transform3x3(mat, osg::Vec3f(0.0, -1.0, 0.0));
	Vec3d c1 = mat.transform3x3(mat, osg::Vec3f(0.0, 0.0, 1));
	c1.normalize();
	double lookDistance = 4.0f;
	Vec3d center1 = eye1 + c1*lookDistance;

	vector<Vec3d> pts;
	pts.push_back(eye1);
	pts.push_back(center1);

	shared_ptr<ICapture> icapture = capture::ICaptureFactory::create();

	if (icapture)
	{
		icapture->autoCaptureImage(node, outFileName, imageWidth, imageHeight, eye1.x(), eye1.y(), eye1.z(), center1.x(), center1.y(), center1.z(), up.x(), up.y(), up.z(), fov);
	}

}

