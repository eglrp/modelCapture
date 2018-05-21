
// modelCaptureDlg.cpp : implementation file
//

#include "stdafx.h"
#include "modelCapture.h"
#include "modelCaptureDlg.h"
#include "afxdialogex.h"
#include "ICapture.h"
#include <memory>
#include <osg/Vec3d>
#include <osgDB/ReadFile>
#include <osg/Node>
#include <osg/BoundingSphere>
#include <thread>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace capture;
using namespace std;
using namespace osg;
using namespace osgDB;




// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
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


// CmodelCaptureDlg dialog



CmodelCaptureDlg::CmodelCaptureDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CmodelCaptureDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	shared_ptr<CSnapPara> temp(new CSnapPara);
	mSnapPara = temp;

	iCapture = ICaptureFactory::create();
}

void CmodelCaptureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, ID_ModelPath, mSnapPara->mSceneFileName);
	DDX_Text(pDX, ID_SAVE_PATH, mSnapPara->mOutFile);
	DDX_Text(pDX, ID_IMAGE_WIDTH, mSnapPara->mImageWidth);
	DDX_Text(pDX, ID_IMAGE_HEIGHT, mSnapPara->mImageHeight);
	DDX_Text(pDX, ID_RADIUS, mSnapPara->mRadius);
	DDX_Text(pDX, ID_RADIUS2, mSnapPara->mInterval);
	DDX_Text(pDX, ID_RADIUS3, mSnapPara->mMinLatitude);
	DDX_Text(pDX, ID_RADIUS4, mSnapPara->mMaxLatitude);
	DDX_Text(pDX, ID_RADIUS5, mSnapPara->mMinLongitude);
	DDX_Text(pDX, ID_RADIUS6, mSnapPara->mMaxLongitude);
}

BEGIN_MESSAGE_MAP(CmodelCaptureDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(loadSnapSavePath, &CmodelCaptureDlg::OnBnClickedloadsnapsavepath)
	ON_BN_CLICKED(IDSNAP_IMAGE, &CmodelCaptureDlg::OnBnClickedImage)
	ON_BN_CLICKED(ID_PREVIEW, &CmodelCaptureDlg::OnBnClickedPreview)
	ON_BN_CLICKED(loadSnapSavePath2, &CmodelCaptureDlg::OnBnClickedloadsnapsavepath2)
	ON_STN_CLICKED(ID_INTERVAL, &CmodelCaptureDlg::OnStnClickedInterval)
	ON_EN_CHANGE(ID_RADIUS4, &CmodelCaptureDlg::OnEnChangeRadius4)
	ON_EN_CHANGE(ID_RADIUS, &CmodelCaptureDlg::OnEnChangeRadius)
	ON_EN_CHANGE(ID_RADIUS3, &CmodelCaptureDlg::OnEnChangeRadius3)
	ON_EN_CHANGE(ID_RADIUS5, &CmodelCaptureDlg::OnEnChangeRadius5)
	ON_EN_CHANGE(ID_RADIUS6, &CmodelCaptureDlg::OnEnChangeRadius6)
	ON_BN_CLICKED(IDC_BUTTON1, &CmodelCaptureDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CmodelCaptureDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CmodelCaptureDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CmodelCaptureDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CmodelCaptureDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &CmodelCaptureDlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON9, &CmodelCaptureDlg::OnBnClickedButton9)
	ON_BN_CLICKED(IDC_BUTTON11, &CmodelCaptureDlg::OnBnClickedButton11)
	ON_BN_CLICKED(IDC_BUTTON7, &CmodelCaptureDlg::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON8, &CmodelCaptureDlg::OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON12, &CmodelCaptureDlg::OnBnClickedButton12)
	ON_BN_CLICKED(IDC_BUTTON10, &CmodelCaptureDlg::OnBnClickedButton10)
END_MESSAGE_MAP()


// CmodelCaptureDlg message handlers

BOOL CmodelCaptureDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	UpdateData(TRUE);
	mSnapPara->mImageWidth = 3000;
	mSnapPara->mImageHeight = 3000;
	mSnapPara->mRadius = 100;
	mSnapPara->mInterval = 45;
	mSnapPara->mMinLatitude = -45;
	mSnapPara->mMaxLatitude = 45;
	mSnapPara->mMinLongitude = -45;
	mSnapPara->mMaxLongitude = 45;

	UpdateData(FALSE);

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CmodelCaptureDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CmodelCaptureDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CmodelCaptureDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CmodelCaptureDlg::OnBnClickedloadsnapsavepath()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	// TODO: Add your control notification handler code her

	CString  FullPath;
	BROWSEINFO bi;                        //BROWSEINFO�ṹ��  
	TCHAR Buffer[512] = _T("");
	TCHAR FullPaths[512] = _T("");
	bi.hwndOwner = m_hWnd;                    //m_hWnd��ĳ���������  
	bi.pidlRoot = NULL;
	bi.pszDisplayName = Buffer;                //����ѡ���Ŀ¼���Ļ�����  
	bi.lpszTitle = _T("��ѡ�����Ŀ¼");            //�����Ĵ��ڵ�������ʾ  
	bi.ulFlags = NULL;//BIF_RETURNONLYFSDIRS|BIF_EDITBOX|BIF_BROWSEFORCOMPUTER ;    //ֻ����Ŀ¼��������־��MSDN  
	bi.lpfn = NULL;                        //�ص���������ʱ������  
	bi.lParam = 0;
	bi.iImage = 0;
	ITEMIDLIST* pidl = ::SHBrowseForFolder(&bi);        //��ʾ�������ڣ�ITEMIDLIST����Ҫ  
	::SHGetPathFromIDList(pidl, FullPaths);        //��ITEMIDLIST�еõ�Ŀ¼��������·��  
	if (FullPaths[0] != NULL)
	{
		mSnapPara->mOutFile = FullPaths;
		UpdateData(FALSE);
	}

	mSnapPara->mOutFile += _T("\\");

	UpdateData(FALSE);
}


void CmodelCaptureDlg::OnBnClickedImage()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	if (mSnapPara->mSceneNode == NULL)
	{
		return;
	}

	string savePath = mSnapPara->mOutFile;
	int interval = mSnapPara->mInterval;
	double radius = mSnapPara->mRadius;
	Vec3d center = mSnapPara->mCenter;
	Vec3d up(0, 0, 1);

	struct threadFunc
	{
		std::vector<threadPara> vecPara;
	};

	const int threadMax = std::thread::hardware_concurrency() - 1;

	vector<threadFunc> thr;
	thr.resize(threadMax);

	int totalNum = 0;

	for (int latitude = -180; latitude <= 180; latitude = latitude + interval)
	{
		for (int longtitude = -180; longtitude <= 180; longtitude = longtitude + interval)
		{
			double t = latitude;
			double p = longtitude;

			if (t >= mSnapPara->mMinLatitude && t <= mSnapPara->mMaxLatitude && p >= mSnapPara->mMinLongitude && p <= mSnapPara->mMaxLongitude)
			{
				totalNum++;
				double x = radius * cos(t / 180 * PI) * cos(p / 180 * PI) + center.x();
				double y = radius * cos(t / 180 * PI) * sin(p / 180 * PI) + center.y();
				double z = radius * sin(t / 180 * PI) + center.z();

				string snapFile = savePath + to_string(latitude) + "_" + to_string(longtitude) + ".jpg";
				threadPara para;
				para.x = x;
				para.y = y;
				para.z = z;
				para.snapFile = snapFile;

				int groupNum = totalNum % threadMax;
				thr[groupNum].vecPara.push_back(para);
			}
		}
	}

	std::vector<std::thread> vecThread;

	for (auto func : thr)
	{
		ref_ptr<Node> model = mSnapPara->mSceneNode;
		vector<threadPara> vecPara = func.vecPara;
		int imageWidth = mSnapPara->mImageWidth;
		int imageHeight = mSnapPara->mImageHeight;
		vecThread.push_back(std::thread(&CmodelCaptureDlg::startThread, this, vecPara, model, imageWidth, imageHeight, center, up));
	}

	std::for_each(vecThread.begin(), vecThread.end(),
		std::mem_fn(&std::thread::detach));

	UpdateData(FALSE);

	AfxMessageBox(_T("��������360�Ƚ�ͼ"));
}

void CmodelCaptureDlg::startThread(vector<threadPara> vecPara, ref_ptr<Node> node, 
	int imageWidth, int imageHeight, osg::Vec3d center, osg::Vec3d up)
{
	shared_ptr<ICapture> iCapture = ICaptureFactory::create();

	for (auto para : vecPara)
	{
		double x = para.x;
		double y = para.y;
		double z = para.z;
		string snapFile = para.snapFile;

		Vec3d up(0, 0, 1);

		ref_ptr<Node> model = dynamic_cast<osg::Node*> (node->clone(osg::CopyOp::DEEP_COPY_ALL));

		iCapture->autoCaptureImage(model, snapFile, imageWidth, imageHeight,
			x, y, z, center.x(), center.y(), center.z(),
			up.x(), up.y(), up.z());
	}
}

void CmodelCaptureDlg::OnBnClickedPreview()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	/*IViewer->showCaptureSphere(radius);*/

	AfxMessageBox(_T("Ԥ����ʼ����esc�˳�"));

	iCapture->preview(mSnapPara);

	iCapture->setPreview();
}


void CmodelCaptureDlg::OnBnClickedloadsnapsavepath2()
{
	UpdateData(TRUE);
	// TODO: Add your control notification handler code here
	CFileDialog dialog(TRUE, NULL, NULL, OFN_HIDEREADONLY, (LPCTSTR)_TEXT("model file (*.osgb, *.obj...)|*.*||"), NULL);
	if (dialog.DoModal() == IDOK)
	{
		mSnapPara->mSceneFileName = dialog.GetPathName();
		string sceneFileName = mSnapPara->mSceneFileName;
		mSnapPara->mSceneNode = osgDB::readNodeFile(sceneFileName);
		
		mSnapPara->mCenter = mSnapPara->mSceneNode->getBound().center();
	}

	UpdateData(FALSE);
}


void CmodelCaptureDlg::OnStnClickedInterval()	
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	if (mSnapPara->mInterval == 0)
	{
		return;
	}

	iCapture->refresh(mSnapPara);
	UpdateData(FALSE);
}


void CmodelCaptureDlg::OnEnChangeRadius4()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	UpdateData(TRUE);
	// TODO:  Add your control notification handler code here
	iCapture->refresh(mSnapPara);

	UpdateData(FALSE);
}


void CmodelCaptureDlg::OnEnChangeRadius()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData(TRUE);
	iCapture->refresh(mSnapPara);

	UpdateData(FALSE);
}


void CmodelCaptureDlg::OnEnChangeRadius3()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData(TRUE);
	iCapture->refresh(mSnapPara);

	UpdateData(FALSE);
}


void CmodelCaptureDlg::OnEnChangeRadius5()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData(TRUE);
	iCapture->refresh(mSnapPara);

	UpdateData(FALSE);
}


void CmodelCaptureDlg::OnEnChangeRadius6()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData(TRUE);
	iCapture->refresh(mSnapPara);

	UpdateData(FALSE);
}


void CmodelCaptureDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code 
	UpdateData(TRUE);
	mSnapPara->mRadius--;
	iCapture->refresh(mSnapPara);
	UpdateData(FALSE);
}


void CmodelCaptureDlg::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	mSnapPara->mRadius++;
	iCapture->refresh(mSnapPara);
	UpdateData(FALSE);
}


void CmodelCaptureDlg::OnBnClickedButton3()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	mSnapPara->mInterval--;
	iCapture->refresh(mSnapPara);
	UpdateData(FALSE);
}


void CmodelCaptureDlg::OnBnClickedButton4()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	mSnapPara->mInterval++;
	iCapture->refresh(mSnapPara);
	UpdateData(FALSE);
}


void CmodelCaptureDlg::OnBnClickedButton5()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	mSnapPara->mMinLatitude--;
	iCapture->refresh(mSnapPara);
	UpdateData(FALSE);
}


void CmodelCaptureDlg::OnBnClickedButton6()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	mSnapPara->mMinLatitude++;
	iCapture->refresh(mSnapPara);
	UpdateData(FALSE);
}


void CmodelCaptureDlg::OnBnClickedButton9()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	mSnapPara->mMaxLatitude--;
	iCapture->refresh(mSnapPara);
	UpdateData(FALSE);
}


void CmodelCaptureDlg::OnBnClickedButton11()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	mSnapPara->mMaxLatitude++;
	iCapture->refresh(mSnapPara);
	UpdateData(FALSE);
}


void CmodelCaptureDlg::OnBnClickedButton7()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	mSnapPara->mMinLongitude--;
	iCapture->refresh(mSnapPara);
	UpdateData(FALSE);
}


void CmodelCaptureDlg::OnBnClickedButton8()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	mSnapPara->mMinLongitude++;
	iCapture->refresh(mSnapPara);
	UpdateData(FALSE);

}


void CmodelCaptureDlg::OnBnClickedButton12()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	mSnapPara->mMaxLongitude--;
	iCapture->refresh(mSnapPara);
	UpdateData(FALSE);
}


void CmodelCaptureDlg::OnBnClickedButton10()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	mSnapPara->mMaxLongitude++;
	iCapture->refresh(mSnapPara);
	UpdateData(FALSE);
}
