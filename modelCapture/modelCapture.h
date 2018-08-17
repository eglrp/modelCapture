
// modelCapture.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include <string>

// CmodelCaptureApp:
// See modelCapture.cpp for the implementation of this class
//

class CmodelCaptureApp : public CWinApp
{
public:
	CmodelCaptureApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	void snap(std::string modelName, double focal, double ccdSize, double imageHeight, double imageWidth, std::string outFileName, double t1x, double t1y, double t1z,
		double r1, double r2, double r3, double r4, double r5, double r6, double r7, double r8, double r9);

	DECLARE_MESSAGE_MAP()
};

extern CmodelCaptureApp theApp;