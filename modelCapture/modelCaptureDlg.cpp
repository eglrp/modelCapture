
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

const int interval = 5;


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
	, mModelPath(_T(""))
	, mSnapSavePath(_T(""))
	, mWidth(0)
	, mHeight(0)
	, mR(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CmodelCaptureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, ID_ModelPath, mModelPath);
	DDX_Text(pDX, ID_SAVE_PATH, mSnapSavePath);
	DDX_Text(pDX, ID_IMAGE_WIDTH, mWidth);
	DDX_Text(pDX, ID_IMAGE_HEIGHT, mHeight);
	DDX_Text(pDX, ID_RADIUS, mR);
}

BEGIN_MESSAGE_MAP(CmodelCaptureDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(loadSnapSavePath, &CmodelCaptureDlg::OnBnClickedloadsnapsavepath)
	ON_BN_CLICKED(IDSNAP_IMAGE, &CmodelCaptureDlg::OnBnClickedImage)
	ON_BN_CLICKED(ID_PREVIEW, &CmodelCaptureDlg::OnBnClickedPreview)
	ON_BN_CLICKED(loadSnapSavePath2, &CmodelCaptureDlg::OnBnClickedloadsnapsavepath2)
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
	mWidth = 3000;
	mHeight = 3000;
	mR = 100;

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
		mSnapSavePath = FullPaths;
		UpdateData(FALSE);
	}

	mSnapSavePath += _T("\\");

	UpdateData(FALSE);
}


void CmodelCaptureDlg::OnBnClickedImage()
{
	// TODO: Add your control notification handler code here
	
	string modelFileName = mModelPath;
	string savePath = mSnapSavePath;
	int imageWidth = mWidth;
	int imageHeight = mHeight;
	double radius = mR;

	ref_ptr<Node> model = readNodeFile(modelFileName);

	if (model == NULL)
	{
		return;
	}

	BoundingSphere bs = model->getBound();
	Vec3d center = bs.center();
	Vec3d up(0, 0, 1);

	struct threadFunc
	{
		std::vector<threadPara> vecPara;
	};

	const int threadMax = std::thread::hardware_concurrency() - 1;

	vector<threadFunc> thr;
	thr.resize(threadMax);

	int totalNum = 0;

	for (int t = -180; t <= 180; t = t + interval)
	{
		for (int p = -180; p <= 180; p = p + interval)
		{
			totalNum++;
			double x = radius * sin(t / PI) * cos(p / PI) + center.x();
			double y = radius * sin(t / PI) * sin(p / PI) + center.y();
			double z = radius * sin(t / PI) + center.z();

			string snapFile = savePath + to_string(t) + "_" + to_string(p) + ".jpg";
			threadPara para;
			para.x = x;
			para.y = y;
			para.z = z;
			para.snapFile = snapFile;

			int groupNum = totalNum % threadMax;
			thr[groupNum].vecPara.push_back(para);
		}
	}

	std::vector<std::thread> vecThread;

	for (auto func : thr)
	{
		vector<threadPara> vecPara = func.vecPara;
		vecThread.push_back(std::thread(&CmodelCaptureDlg::startThread, this, vecPara, modelFileName, imageWidth, imageHeight, center, up));
	}

	std::for_each(vecThread.begin(), vecThread.end(),
		std::mem_fn(&std::thread::detach));

	AfxMessageBox(_T("��������360�Ƚ�ͼ"));
}

void CmodelCaptureDlg::startThread(vector<threadPara> vecPara, string modelFileName, 
	int imageWidth, int imageHeight, osg::Vec3d center, osg::Vec3d up)
{
	shared_ptr<ICapture> iCapture = ICaptureFactory::create();

	for (auto para : vecPara)
	{
		double x = para.x;
		double y = para.y;
		double z = para.z;
		string snapFile = para.snapFile;

		iCapture->autoCaptureImage(modelFileName, snapFile, imageWidth, imageHeight,
			x, y, z, center.x(), center.y(), center.z(),
			up.x(), up.y(), up.z());
	}
}

void CmodelCaptureDlg::OnBnClickedPreview()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	double radius = mR;
	/*IViewer->showCaptureSphere(radius);*/

	AfxMessageBox(_T("Ԥ����ʼ����esc�˳�"));

	shared_ptr<ICapture> iCapture = ICaptureFactory::create();
	string sceneFile = mModelPath;
	iCapture->preview(sceneFile, radius);
}


void CmodelCaptureDlg::OnBnClickedloadsnapsavepath2()
{
	UpdateData(TRUE);
	// TODO: Add your control notification handler code here
	CFileDialog dialog(TRUE, NULL, NULL, OFN_HIDEREADONLY, (LPCTSTR)_TEXT("model file (*.osgb, *.obj...)|*.*||"), NULL);
	if (dialog.DoModal() == IDOK)
	{
		mModelPath = dialog.GetPathName();

		
	}

	UpdateData(FALSE);
}
