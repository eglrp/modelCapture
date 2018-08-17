// modelCaptureCmd.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <osg/ArgumentParser>
#include <osg/Notify>
#include <windows.h>
#include <vector>

namespace cmd
{
	#define _FACE_RECOGNITION "--face"
	#define INVALIDVAL	"null" 
	#define _bUI		"--ui"
	#define _MODEL		"-m"
	#define _FACEMASK	"-f"
	#define _OUT		"-o"
	#define _HEIGHT	    "--height"
	#define _WIDTH		"--width"
	#define _INTERVAL_X "--dx"
	#define _INTERVAL_Y	"--dy"
	#define _MINX 		"--minX"
	#define _MAXX		"--maxX"
	#define _MINY		"--minY"
	#define _MAXY		"--maxY"

	#define _BACKGROUND_SEPERATE "--back"
	#define _FOCAL		"--focal"
	#define _CCD        "--ccd"
	#define _TRANSLATE  "-t"
	#define _ROTATION   "-r"

	#define SPLITSYMBOL	","

	char* errMsg = "命令参数错误，请输入-h 或 --help查看用法";
	char* supportMsg = "不支持的功能命令，请输入-h 或 --help查看用法";
	char* funcMsg = "功能参数错误，请输入-h 或 --help查看用法";
}


using namespace std;
using namespace cmd;

std::string TCHAR2STRING(TCHAR *STR)
{
	int iLen = WideCharToMultiByte(CP_ACP, 0, STR, -1, NULL, 0, NULL, NULL);
	char* chRtn = new char[iLen*sizeof(char)];
	WideCharToMultiByte(CP_ACP, 0, STR, -1, chRtn, iLen, NULL, NULL);
	std::string str(chRtn);
	return str;
}

wchar_t* STRINGTOWCHAR(string str)
{
	size_t size = str.length();
	wchar_t *buffer = new wchar_t[size + 1];
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), size, buffer, size * sizeof(wchar_t));
	buffer[size] = 0;  //确保以 '\0' 结尾 
	return buffer;
}

//输出帮助文档
static void usage(const char *prog, const char *msg)
{
	if (msg)
	{
		osg::notify(osg::NOTICE) << std::endl;
		osg::notify(osg::NOTICE) << msg << std::endl;
	}

	// basic usage
	osg::notify(osg::NOTICE) << std::endl;
	osg::notify(osg::NOTICE) << "用法:" << std::endl;
	osg::notify(osg::NOTICE) << "    " << prog << " 功能参数" << std::endl;
	osg::notify(osg::NOTICE) << std::endl;

	// print tool options
	osg::notify(osg::NOTICE) << "功能参数:" << std::endl;
	osg::notify(osg::NOTICE) << "    [功能命令]" << std::endl;
	osg::notify(osg::NOTICE) << "    --face					- 启动采集面部识别训练样本功能" << std::endl;
	osg::notify(osg::NOTICE) << "    --back					- 启动采集前后背景训练样本功能" << std::endl;
	osg::notify(osg::NOTICE) << std::endl;
	osg::notify(osg::NOTICE) << "    --ui					- 显示UI界面" << std::endl;
	osg::notify(osg::NOTICE) << std::endl;
	osg::notify(osg::NOTICE) << "    [采集面部识别训练样本功能参数必填]" << std::endl;
	osg::notify(osg::NOTICE) << "    -m					- 输入模型文件" << std::endl;
	osg::notify(osg::NOTICE) << "    -f					- 输入的面部特征点文件" << std::endl;
	osg::notify(osg::NOTICE) << "    -o					- 输出保存截图路径（最后要带斜杠）" << std::endl;
	osg::notify(osg::NOTICE) << std::endl;
	osg::notify(osg::NOTICE) << "    [采集面部识别训练样本功能参数可选]" << std::endl;
	osg::notify(osg::NOTICE) << "    --height			- 输出影像高度" << std::endl;
	osg::notify(osg::NOTICE) << "    --width			- 输出影像宽度" << std::endl;
	osg::notify(osg::NOTICE) << "    --dx				- 纬度间隔" << std::endl;
	osg::notify(osg::NOTICE) << "    --dy				- 经度间隔" << std::endl;
	osg::notify(osg::NOTICE) << "    --minX				- 纬度最小值" << std::endl;
	osg::notify(osg::NOTICE) << "    --maxX				- 纬度最大值" << std::endl;
	osg::notify(osg::NOTICE) << "    --minY				- 经度最小值" << std::endl;
	osg::notify(osg::NOTICE) << "    --maxY				- 经度最大值" << std::endl;
	osg::notify(osg::NOTICE) << std::endl;

	osg::notify(osg::NOTICE) << "    [采集前后背景训练样本功能参数必填]" << std::endl;
	osg::notify(osg::NOTICE) << "    -m					- 输入模型文件" << std::endl;
	osg::notify(osg::NOTICE) << "    -o					- 输出保存截图" << std::endl;
	osg::notify(osg::NOTICE) << "    --height			- 输出影像高度" << std::endl;
	osg::notify(osg::NOTICE) << "    --width			- 输出影像宽度" << std::endl;
	osg::notify(osg::NOTICE) << "    --focal			- 输入的相机焦距" << std::endl;
	osg::notify(osg::NOTICE) << "    --ccd				- 相元大小" << std::endl;
	osg::notify(osg::NOTICE) << "    -t					- rt矩阵中的t" << std::endl;
	osg::notify(osg::NOTICE) << "    -r					- rt矩阵中的r" << std::endl;
	osg::notify(osg::NOTICE) << std::endl;
	osg::notify(osg::NOTICE) << "    注意：坐标格式如下  X1,Y1,X2,Y2,X3,Y3 ... 同一个参数之间用“,”连接，无界面运行需保证所有参数输入正确并且完整，有界面运行除功能参数以外其他可选" << std::endl;
}


//字符串分割
void SplitString(const string& s, vector<string>& v, const string& c)
{
	string::size_type pos1, pos2;
	pos2 = s.find(c);
	pos1 = 0;
	while (string::npos != pos2)
	{
		v.push_back(s.substr(pos1, pos2 - pos1));

		pos1 = pos2 + c.size();
		pos2 = s.find(c, pos1);
	}
	if (pos1 != s.length())
		v.push_back(s.substr(pos1));
}

int faceRecognition(osg::ArgumentParser arguments)
{
	char msg[1024];
	std::string sPara;

	//首先检测是否有界面
	bool hasUI = false;

	if (arguments.read(_bUI))
	{
		hasUI = true;
	}

	std::string sInModel, sInFaceMask, sOut, sImageHeight, sImageWidth, sIntervalX, sIntervalY, sMinX, sMinY, sMaxX, sMaxY;

	if (!hasUI)
	{
		if (!arguments.read(_MODEL, sInModel))
		{
			osg::notify(osg::NOTICE) << errMsg << std::endl;
			return -1;
		}

		if (!arguments.read(_FACEMASK, sInFaceMask))
		{
			osg::notify(osg::NOTICE) << errMsg << std::endl;
			return -1;
		}

		if (!arguments.read(_OUT, sOut))
		{
			osg::notify(osg::NOTICE) << errMsg << std::endl;
			return -1;
		}
	}
	else
	{
		if (!arguments.read(_MODEL, sInModel))
		{
			sInModel = "none";
		}

		if (!arguments.read(_FACEMASK, sInFaceMask))
		{
			sInFaceMask = "none";
		}

		if (!arguments.read(_OUT, sOut))
		{
			sOut = "none";
		}
	}

	if (!arguments.read(_HEIGHT, sImageHeight))
	{
		sImageHeight = "none";
	}

	if (!arguments.read(_WIDTH, sImageWidth))
	{
		sImageWidth = "none";
	}

	if (!arguments.read(_INTERVAL_X, sIntervalX))
	{
		sIntervalX = "none";
	}

	if (!arguments.read(_INTERVAL_Y, sIntervalY))
	{
		sIntervalY = "none";
	}

	if (!arguments.read(_MINX, sMinX))
	{
		sMinX = "none";
	}

	if (!arguments.read(_MAXX, sMaxX))
	{
		sMaxX = "none";
	}

	if (!arguments.read(_MINY, sMinY))
	{
		sMinY = "none";
	}

	if (!arguments.read(_MAXY, sMaxY))
	{
		sMaxY = "none";
	}

	if (hasUI)
	{
		sPara += "1 ";
	}
	else
	{
		sPara += "0 ";
	}

	sPara += sInModel + " " + sInFaceMask + " " + sOut + " " + sImageHeight + " " + sImageWidth + " " + sIntervalX + " " + sIntervalY + " " + sMinX + " " + sMaxX + " " + sMinY + " " + sMaxY;

	TCHAR module[_MAX_PATH] = { 0 };
	GetModuleFileName(NULL, module, _MAX_PATH);
	string curPath = TCHAR2STRING(module);
	int pos = curPath.find_last_of("\\");
	curPath = curPath.substr(0, pos + 1);
	string exePath = curPath + "modelCapture.exe";

	SHELLEXECUTEINFO shExecInfo = { 0 };
	shExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	shExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	shExecInfo.hwnd = NULL;
	shExecInfo.lpVerb = _T("open");
	shExecInfo.lpFile = STRINGTOWCHAR(exePath);
	shExecInfo.lpParameters = STRINGTOWCHAR(sPara);
	shExecInfo.lpDirectory = STRINGTOWCHAR(curPath);
	shExecInfo.nShow = hasUI ? SW_SHOW : SW_HIDE;
	shExecInfo.hInstApp = NULL;
	ShellExecuteEx(&shExecInfo);

	//无界面程序输出提示信息
	if (!hasUI)
	{
		osg::notify(osg::NOTICE) << "无界面程序正在运行中..." << std::endl;
	}
	//等待外部调用程序执行完毕
	WaitForSingleObject(shExecInfo.hProcess, INFINITE);
	if (!hasUI)
	{
		osg::notify(osg::NOTICE) << "无界面程序运行结束..." << std::endl;
	}
}

int backGroundSeperate(osg::ArgumentParser arguments)
{
	std::string sPara;
	std::string sInModel, sOut, sFocal, sCCD, sImageWidth, sImageHeight, sTMat, sRMat;

	if (!arguments.read(_MODEL, sInModel))
	{
		osg::notify(osg::NOTICE) << errMsg << std::endl;
		return 0;
	}
	else if (!arguments.read(_OUT, sOut))
	{
		osg::notify(osg::NOTICE) << errMsg << std::endl;
		return 0;
	}
	else if (!arguments.read(_FOCAL, sFocal))
	{
		osg::notify(osg::NOTICE) << errMsg << std::endl;
		return 0;
	}
	else if (!arguments.read(_CCD, sCCD))
	{
		osg::notify(osg::NOTICE) << errMsg << std::endl;
		return 0;
	}
	else if (!arguments.read(_TRANSLATE, sTMat))
	{
		osg::notify(osg::NOTICE) << errMsg << std::endl;
		return 0;
	}
	else if (!arguments.read(_HEIGHT, sImageHeight))
	{
		osg::notify(osg::NOTICE) << errMsg << std::endl;
		return 0;
	}

	else if (!arguments.read(_WIDTH, sImageWidth))
	{
		osg::notify(osg::NOTICE) << errMsg << std::endl;
		return 0;
	}
	else if (!arguments.read(_ROTATION, sRMat))
	{
		osg::notify(osg::NOTICE) << errMsg << std::endl;
		return 0;
	}

	//检测范围坐标
	vector<string> translate;
	SplitString(sTMat, translate, SPLITSYMBOL);

	vector<string> rot;
	SplitString(sRMat, rot, SPLITSYMBOL);

	sPara += sInModel + " " + sOut + " " + sFocal + " " + sCCD + " ";

	for (auto t : translate)
	{
		sPara += t + " ";
	}

	sPara += sImageWidth + " " + sImageHeight + " ";

	for (auto r : rot)
	{
		sPara += r + " ";
	}

	osg::notify(osg::NOTICE) << "输入模型 " << sInModel << std::endl;
	osg::notify(osg::NOTICE) << "输出照片 " << sOut << std::endl;
	osg::notify(osg::NOTICE) << "输出焦距 " << sFocal << std::endl;
	osg::notify(osg::NOTICE) << "输出相元大小 " << sCCD << std::endl;
	osg::notify(osg::NOTICE) << "输出rt矩阵中t " << sTMat << std::endl;
	osg::notify(osg::NOTICE) << "影像宽 " << sImageWidth << "影像高 "<< sImageHeight << std::endl;
	osg::notify(osg::NOTICE) << "输出rt矩阵中r " << sRMat << std::endl;

	TCHAR module[_MAX_PATH] = { 0 };
	GetModuleFileName(NULL, module, _MAX_PATH);
	string curPath = TCHAR2STRING(module);
	int pos = curPath.find_last_of("\\");
	curPath = curPath.substr(0, pos + 1);
	string exePath = curPath + "modelCapture.exe";

	SHELLEXECUTEINFO shExecInfo = { 0 };
	shExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	shExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	shExecInfo.hwnd = NULL;
	shExecInfo.lpVerb = _T("open");
	shExecInfo.lpFile = STRINGTOWCHAR(exePath);
	shExecInfo.lpParameters = STRINGTOWCHAR(sPara);
	shExecInfo.lpDirectory = STRINGTOWCHAR(curPath);
	shExecInfo.nShow = SW_SHOW;
	shExecInfo.hInstApp = NULL;
	ShellExecuteEx(&shExecInfo);

	//无界面程序输出提示信息
	osg::notify(osg::NOTICE) << "无界面程序正在运行中..." << std::endl;

	//等待外部调用程序执行完毕
	WaitForSingleObject(shExecInfo.hProcess, INFINITE);

	osg::notify(osg::NOTICE) << "无界面程序运行结束..." << std::endl;

	return 1;
}

int main(int argc, char** argv)
{
	osg::ArgumentParser arguments(&argc, argv);
	arguments.getApplicationUsage()->setApplicationName(arguments.getApplicationName());
	arguments.getApplicationUsage()->setDescription(arguments.getApplicationName() + " 用于osg数据的工具集合.");
	arguments.getApplicationUsage()->setCommandLineUsage(arguments.getApplicationName() + " [options]");

	// if user request help write it out to cout.
	if (arguments.read("-h") || arguments.read("--help"))
	{
		osg::setNotifyLevel(osg::NOTICE);
		usage(arguments.getApplicationName().c_str(), 0);
		return 1;
	}
	
	if (arguments.read(_FACE_RECOGNITION))
	{
		faceRecognition(arguments);
	}
	else if (arguments.read(_BACKGROUND_SEPERATE))
	{
		backGroundSeperate(arguments);
	}
	else
	{
		osg::notify(osg::NOTICE) << funcMsg << std::endl;
		return 0;
	}
	

	return 0;
}

