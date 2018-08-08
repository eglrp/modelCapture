
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
#include "osg/MatrixTransform"
#include "opencvApi.h"
#include "qcomm.h"

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

Quat HPRToQuat(double heading, double pitch, double roll)
{
	Quat q(roll, Vec3d(0, 1.0, 0), pitch, Vec3d(1.0, 0, 0),
		heading, Vec3d(0, 0, 1.0));
	return q;
}

void QuatToHPR(Quat q, double& heading, double& pitch, double& roll)
{
	double test = q.y() * q.z() + q.x() * q.w();
	if (test > 0.4999)
	{ // singularity at north pole
		heading = 2.0 * atan2(q.y(), q.w());
		pitch = PI_2;
		roll = 0.0;
		return;
	}
	if (test < -0.4999)
	{ // singularity at south pole
		heading = 2.0 * atan2(q.y(), q.w());
		pitch = -PI_2;
		roll = 0.0;
		return;
	}
	double sqx = q.x() * q.x();
	double sqy = q.y() * q.y();
	double sqz = q.z() * q.z();
	heading = atan2(2.0 * q.z() * q.w() - 2.0 * q.y() * q.x(), 1.0 - 2.0 * sqz - 2.0 * sqx);
	pitch = asin(2.0 * test);
	roll = atan2(2.0 * q.y() * q.w() - 2.0 * q.z() * q.x(), 1.0 - 2.0 * sqy - 2.0 * sqx);
}



CmodelCaptureDlg::CmodelCaptureDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CmodelCaptureDlg::IDD, pParent)
	, mUpSideDown(FALSE)
	, mPitch(0)
	, mYaw(0)
	, mRoll(0)
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
	DDX_Check(pDX, IDC_CHECK1, mUpSideDown);
	DDX_Text(pDX, ID_RADIUS7, mPitch);
	DDX_Text(pDX, ID_RADIUS8, mYaw);
	DDX_Text(pDX, ID_RADIUS9, mRoll);
	DDX_Text(pDX, ID_FACE_MASK, mSnapPara->mFaceMaskFileName);
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
	ON_BN_CLICKED(IDC_CHECK1, &CmodelCaptureDlg::OnBnClickedCheck1)
	ON_EN_CHANGE(ID_RADIUS7, &CmodelCaptureDlg::OnEnChangeRadius7)
	ON_EN_CHANGE(ID_RADIUS8, &CmodelCaptureDlg::OnEnChangeRadius8)
	ON_EN_CHANGE(ID_RADIUS9, &CmodelCaptureDlg::OnEnChangeRadius9)
	ON_BN_CLICKED(IDC_BUTTON13, &CmodelCaptureDlg::OnBnClickedButton13)
	ON_BN_CLICKED(IDC_BUTTON14, &CmodelCaptureDlg::OnBnClickedButton14)
	ON_BN_CLICKED(IDC_BUTTON15, &CmodelCaptureDlg::OnBnClickedButton15)
	ON_BN_CLICKED(IDC_BUTTON16, &CmodelCaptureDlg::OnBnClickedButton16)
	ON_BN_CLICKED(IDC_BUTTON17, &CmodelCaptureDlg::OnBnClickedButton17)
	ON_BN_CLICKED(IDC_BUTTON18, &CmodelCaptureDlg::OnBnClickedButton18)
	ON_BN_CLICKED(loadFaceMaskPath3, &CmodelCaptureDlg::OnBnClickedloadfacemaskpath3)
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
	BROWSEINFO bi;                        //BROWSEINFO结构体  
	TCHAR Buffer[512] = _T("");
	TCHAR FullPaths[512] = _T("");
	bi.hwndOwner = m_hWnd;                    //m_hWnd你的程序主窗口  
	bi.pidlRoot = NULL;
	bi.pszDisplayName = Buffer;                //返回选择的目录名的缓冲区  
	bi.lpszTitle = _T("请选择输出目录");            //弹出的窗口的文字提示  
	bi.ulFlags = NULL;//BIF_RETURNONLYFSDIRS|BIF_EDITBOX|BIF_BROWSEFORCOMPUTER ;    //只返回目录。其他标志看MSDN  
	bi.lpfn = NULL;                        //回调函数，有时很有用  
	bi.lParam = 0;
	bi.iImage = 0;
	ITEMIDLIST* pidl = ::SHBrowseForFolder(&bi);        //显示弹出窗口，ITEMIDLIST很重要  
	::SHGetPathFromIDList(pidl, FullPaths);        //在ITEMIDLIST中得到目录名的整个路径  
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
	Vec3d up = mSnapPara->mUp;

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
				p -= 90;
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
				para.up = up;

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

	AfxMessageBox(_T("正在生成360度截图"));
}

void CmodelCaptureDlg::startThread(vector<threadPara> vecPara, ref_ptr<Node> node, 
	int imageWidth, int imageHeight, Vec3d center, Vec3d up)
{
	shared_ptr<ICapture> iCapture = ICaptureFactory::create();

	for (auto para : vecPara)
	{
		double x = para.x;
		double y = para.y;
		double z = para.z;
		string snapFile = para.snapFile;

		Vec3d up = para.up;

		ref_ptr<Node> model = dynamic_cast<Node*> (node->clone(CopyOp::DEEP_COPY_ALL));

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

	AfxMessageBox(_T("预览开始，按esc退出"));

	iCapture->preview(mSnapPara);

	iCapture->setPreview();
}

void CmodelCaptureDlg::OnBnClickedloadfacemaskpath3()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	CFileDialog dialog(TRUE, NULL, NULL, OFN_HIDEREADONLY, (LPCTSTR)_TEXT("faceMask file (*.txt)|*.*||"), NULL);

	if (dialog.DoModal() == IDOK)
	{
		mSnapPara->mFaceMaskFileName = dialog.GetPathName();

	}

	UpdateData(FALSE);
}


void CmodelCaptureDlg::OnBnClickedloadsnapsavepath2()
{
	UpdateData(TRUE);

	if (mSnapPara->mFaceMaskFileName == "")
	{
		AfxMessageBox(_T("没有加载面部特征文件"));
		return;
	}

	// TODO: Add your control notification handler code here
	CFileDialog dialog(TRUE, NULL, NULL, OFN_HIDEREADONLY, (LPCTSTR)_TEXT("model file (*.osgb, *.obj...)|*.*||"), NULL);
	if (dialog.DoModal() == IDOK)
	{
		mSnapPara->mSceneFileName = dialog.GetPathName();
		string sceneFileName = mSnapPara->mSceneFileName;

		ref_ptr<Node> model = osgDB::readNodeFile(sceneFileName);

		if (!model)
		{
			return;
		}

		model->getOrCreateStateSet()->setMode(GL_LIGHTING, 0x2);
		Vec3d center = model->getBound().center();
		ref_ptr<MatrixTransform> trans = new MatrixTransform;
		trans->addChild(model);
		vector<pair<int, Vec3d>> vecXyz;

		if (readFaceKeyPoints(vecXyz) == false)
		{
			return;
		}

		correctModel(vecXyz, trans);

		calculateDist(vecXyz);


		mSnapPara->mSceneNode = trans;
		mSnapPara->mCenter = Vec3d(0, 0, 0);
	}

	UpdateData(FALSE);
}

bool CmodelCaptureDlg::readFaceKeyPoints(vector<pair<int, Vec3d>> &vecXyz)
{
	//对模型进行纠正
	string str = mSnapPara->mFaceMaskFileName;
	const char* fileName = str.c_str();
	FILE* fp = std::fopen(fileName, "r");

	while (!feof(fp))
	{
		int id = 0;
		double x = 0.0;
		double y = 0.0;
		double z = 0.0;
		fscanf(fp, "点位%d:%lf %lf %lf\n", &id, &x, &y, &z);
		vecXyz.push_back(pair<int, Vec3d>(id, Vec3d(x, y, z)));
	}

	if (vecXyz.size() != 68)
	{
		AfxMessageBox(_T("特征点数量不等于68个"));
		return false;
	}

	return true;
}

bool CmodelCaptureDlg::correctModel(vector<pair<int, Vec3d>> vecXyz, ref_ptr<MatrixTransform> trans)
{
	//计算中心
	vector<Vec3d> xyzs;
	Vec3d center;

	for (auto pt : vecXyz)
	{
		xyzs.push_back(pt.second);

		center += pt.second;
	}

	center = center / vecXyz.size();

	//计算正面法向量
	double a = 0.0; double b = 0.0; double c = 0.0; double d = 0.0;
	CVPlane(xyzs, a, b, c, d);

	Vec3d normal = Vec3d(a, b, c);
	Vec3d diff = xyzs[31] - center;

	if (diff * normal < 0)
	{
		normal *= -1;
	}

	//位移到原点
	Vec3d translate = Vec3d(0, 0, 0) - center;
	Matrix mat1;
	mat1.makeTranslate(translate);

	//把模型的正面法向量旋转到vec3d(0, -1, 0);
	Matrix mat2;
	mat2.makeRotate(normal, Vec3d(0, -1, 0));
	Matrix mat;
	mat = mat1 * mat2;

	//计算向上方向
	calculateUpDir(xyzs, mat);
	trans->setMatrix(mat);
	mCorrectMat = mat;

	return true;
}

bool CmodelCaptureDlg::calculateDist(vector<pair<int, Vec3d>> vecXyz)
{
	Vec3d min = Vec3d(0, 0, 0);
	Vec3d max = Vec3d(0, 0, 0);

	for (int i = 0; i < vecXyz.size(); i++)
	{
		Vec3d pt = vecXyz[i].second;
		pt = pt * mCorrectMat;

		if (i == 0)
		{
			min.x() = pt.x();
			min.y() = pt.y();
			min.z() = pt.z();

			max.x() = pt.x();
			max.y() = pt.y();
			max.z() = pt.z();
		}

		if (pt.x() < min.x())
		{
			min.x() = pt.x();
		}

		if (pt.y() < min.y())
		{
			min.y() = pt.y();
		}

		if (pt.z() < min.z())
		{
			min.z() = pt.z();
		}

		if (pt.x() > max.x())
		{
			max.x() = pt.x();
		}

		if (pt.y() > max.y())
		{
			max.y() = pt.y();
		}

		if (pt.z() < max.z())
		{
			max.z() = pt.z();
		}
	}

	double height = DisplaySettings::instance()->getScreenHeight();
	double width = DisplaySettings::instance()->getScreenWidth();
	double distance = DisplaySettings::instance()->getScreenDistance();
	double vfov = RadiansToDegrees(atan2(height / 2.0f, distance)*2.0);

	vfov /= 2.0;
	double radius = sqrt(0.25 * ((max - min).length2()));
	mSnapPara->mRadius = radius / sin(vfov /180.0 * PI);

	return true;
}

bool CmodelCaptureDlg::calculateUpDir(vector<Vec3d> xyzs, Matrix &mat)
{
	Vec3d total = Vec3d(0, 0, 0);
	vector<Vec3d> pts;
	vector<Vec3d> xys;

	for (int i = 0; i < xyzs.size(); i++)
	{
		auto pt = xyzs[i] * mat;
		xys.emplace_back(pt.x(), pt.z(), 0);
	}

	vector<Vec3d> keyPoints;
	for (int i = 0; i < 8; i++)
	{
		auto pt = xys[0 + i] + xys[16 - i];
		pt /= 2.0;
		keyPoints.push_back(pt);
	}

	for (int i = 0; i < 5; i++)
	{
		auto pt = xys[17 + i] + xys[26 - i];
		pt /= 2.0;
		keyPoints.push_back(pt);
	}

	//非对称点
	for (int i = 0; i < 4; i++)
	{
		auto pt = xys[27 + i];
		keyPoints.push_back(pt);
	}

	for (int i = 0; i < 6; i++)
	{
		auto pt = xys[36 + i] + xys[47 - i];
		pt /= 2.0;
		keyPoints.push_back(pt);
	}


	float nx = 0.0; float ny = 0.0; float x0 = 0.0; float y0 = 0.0;
	CVLine(keyPoints, nx, ny, x0, y0);
	double k = ny / nx;

	double pt1x = 0.0;
	double pt1Z = k * (pt1x - x0) + y0;
	double pt2x = 10.0;
	double pt2Z = k * (pt2x - x0) + y0;
	vector<Vec3d> vecPts;
	vecPts.emplace_back(pt1x, 0, pt1Z);
	vecPts.emplace_back(pt2x, 0, pt2Z);
	Vec3d lineDir = vecPts[1] - vecPts[0];
	Vec3d correctDir = xys[27] - xys[30];

	if (lineDir * correctDir < 0)
	{
		lineDir *= -1;
	}

	Matrix mat3;
	mat3.makeRotate(lineDir, Vec3d(0, 0, 1));
	mat *= mat3;

	return true;
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
	mSnapPara->mMinLongitude++;
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
	mSnapPara->mMinLatitude++;
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


void CmodelCaptureDlg::OnBnClickedCheck1()
{
	// TODO: Add your control notification handler code here
	if (mUpSideDown == TRUE)
	{
		mSnapPara->mUp = Vec3d(0, 0, 1);
	}
	else
	{
		mSnapPara->mUp = Vec3d(0, 0, -1);
	}
}


void CmodelCaptureDlg::OnEnChangeRadius7()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData(TRUE);
	rotateModel(mPitch, mYaw, mRoll);
	UpdateData(FALSE);
}


void CmodelCaptureDlg::OnEnChangeRadius8()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData(TRUE);
	rotateModel(mPitch, mYaw, mRoll);
	UpdateData(FALSE);
}


void CmodelCaptureDlg::OnEnChangeRadius9()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData(TRUE);
	rotateModel(mPitch, mYaw, mRoll);
	UpdateData(FALSE);
}

void CmodelCaptureDlg::rotateModel(double pitch, double yaw, double roll)
{
	double p = pitch / 180.0 * PI;
	double y = yaw / 180.0 * PI;
	double r = roll / 180.0 * PI;

	Quat quat = HPRToQuat(p, y, r);
	Vec3d center = mSnapPara->mCenter;
	ref_ptr<MatrixTransform> mTrans = dynamic_cast<MatrixTransform*> (mSnapPara->mSceneNode->asTransform());

	if (mTrans)
	{
		Matrix reverseTransMat;
		reverseTransMat.setTrans(center * (-1));
		Matrix rotMat;
		rotMat.setRotate(quat);
		Matrix transMat;
		transMat.setTrans(center);

		Matrix mat = mCorrectMat * reverseTransMat * rotMat * transMat;
		mTrans->setMatrix(mat);
	}
}

void CmodelCaptureDlg::OnBnClickedButton13()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	mPitch--;
	rotateModel(mPitch, mYaw, mRoll);
	UpdateData(FALSE);
}


void CmodelCaptureDlg::OnBnClickedButton14()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	mPitch++;
	rotateModel(mPitch, mYaw, mRoll);
	UpdateData(FALSE);
}


void CmodelCaptureDlg::OnBnClickedButton15()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	mYaw--;
	rotateModel(mPitch, mYaw, mRoll);
	UpdateData(FALSE);
}


void CmodelCaptureDlg::OnBnClickedButton16()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	mYaw++;
	rotateModel(mPitch, mYaw, mRoll);
	UpdateData(FALSE);
}


void CmodelCaptureDlg::OnBnClickedButton17()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	mRoll--;
	rotateModel(mPitch, mYaw, mRoll);
	UpdateData(FALSE);
}


void CmodelCaptureDlg::OnBnClickedButton18()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	mRoll++;
	rotateModel(mPitch, mYaw, mRoll);
	UpdateData(FALSE);
}


