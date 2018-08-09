// modelCaptureCmd.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <osg/ArgumentParser>
#include <osg/Notify>
#include <windows.h>
#include <vector>

namespace cmd
{
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

	char* errMsg = "命令参数错误，请输入-h 或 --help查看用法";
	char* supportMsg = "不支持的功能命令，请输入-h 或 --help查看用法";
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
	osg::notify(osg::NOTICE) << "    --ui					- 显示UI界面" << std::endl;
	osg::notify(osg::NOTICE) << std::endl;
	osg::notify(osg::NOTICE) << "    [截图参数必填]" << std::endl;
	osg::notify(osg::NOTICE) << "    -m					- 输入模型文件" << std::endl;
	osg::notify(osg::NOTICE) << "    -f					- 输入的面部特征点文件" << std::endl;
	osg::notify(osg::NOTICE) << "    -o					- 输出保存截图路径（最后要带斜杠）" << std::endl;
	osg::notify(osg::NOTICE) << std::endl;
	osg::notify(osg::NOTICE) << "    [截图参数可选]" << std::endl;
	osg::notify(osg::NOTICE) << "    --height			- 输出影像高度" << std::endl;
	osg::notify(osg::NOTICE) << "    --width			- 输出影像宽度" << std::endl;
	osg::notify(osg::NOTICE) << "    --dx				- 纬度间隔" << std::endl;
	osg::notify(osg::NOTICE) << "    --dy				- 经度间隔" << std::endl;
	osg::notify(osg::NOTICE) << "    --minX				- 纬度最小值" << std::endl;
	osg::notify(osg::NOTICE) << "    --maxX				- 纬度最大值" << std::endl;
	osg::notify(osg::NOTICE) << "    --minY				- 经度最小值" << std::endl;
	osg::notify(osg::NOTICE) << "    --maxY				- 经度最大值" << std::endl;
	osg::notify(osg::NOTICE) << std::endl;
	osg::notify(osg::NOTICE) << "   注意： 无界面运行需保证所有参数输入正确并且完整，有界面运行除功能参数以外其他可选" << std::endl;
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

	return 0;
}

