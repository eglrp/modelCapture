
// modelCapture.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "modelCapture.h"
#include "modelCaptureDlg.h"
#include <vector>
#include <sstream> 
#include "SnapPara.h"

using namespace std;
using namespace capture;

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
		if (strs.size() != 12)
		{
			return FALSE;
		}

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
			para->mIntervalX = stringToNum<int>(strs[6].c_str());
		}

		if (strs[7] != "none")
		{
			para->mIntervalY = stringToNum<int>(strs[7].c_str());
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
		else 
		{
			CmodelCaptureDlg dlg(para);
			dlg.run();
		}
	}


	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

