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

	char* errMsg = "�����������������-h �� --help�鿴�÷�";
	char* supportMsg = "��֧�ֵĹ������������-h �� --help�鿴�÷�";
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
	buffer[size] = 0;  //ȷ���� '\0' ��β 
	return buffer;
}

//��������ĵ�
static void usage(const char *prog, const char *msg)
{
	if (msg)
	{
		osg::notify(osg::NOTICE) << std::endl;
		osg::notify(osg::NOTICE) << msg << std::endl;
	}

	// basic usage
	osg::notify(osg::NOTICE) << std::endl;
	osg::notify(osg::NOTICE) << "�÷�:" << std::endl;
	osg::notify(osg::NOTICE) << "    " << prog << " ���ܲ���" << std::endl;
	osg::notify(osg::NOTICE) << std::endl;

	// print tool options
	osg::notify(osg::NOTICE) << "���ܲ���:" << std::endl;
	osg::notify(osg::NOTICE) << "    --ui					- ��ʾUI����" << std::endl;
	osg::notify(osg::NOTICE) << std::endl;
	osg::notify(osg::NOTICE) << "    [��ͼ��������]" << std::endl;
	osg::notify(osg::NOTICE) << "    -m					- ����ģ���ļ�" << std::endl;
	osg::notify(osg::NOTICE) << "    -f					- ������沿�������ļ�" << std::endl;
	osg::notify(osg::NOTICE) << "    -o					- ��������ͼ·�������Ҫ��б�ܣ�" << std::endl;
	osg::notify(osg::NOTICE) << std::endl;
	osg::notify(osg::NOTICE) << "    [��ͼ������ѡ]" << std::endl;
	osg::notify(osg::NOTICE) << "    --height			- ���Ӱ��߶�" << std::endl;
	osg::notify(osg::NOTICE) << "    --width			- ���Ӱ����" << std::endl;
	osg::notify(osg::NOTICE) << "    --dx				- γ�ȼ��" << std::endl;
	osg::notify(osg::NOTICE) << "    --dy				- ���ȼ��" << std::endl;
	osg::notify(osg::NOTICE) << "    --minX				- γ����Сֵ" << std::endl;
	osg::notify(osg::NOTICE) << "    --maxX				- γ�����ֵ" << std::endl;
	osg::notify(osg::NOTICE) << "    --minY				- ������Сֵ" << std::endl;
	osg::notify(osg::NOTICE) << "    --maxY				- �������ֵ" << std::endl;
	osg::notify(osg::NOTICE) << std::endl;
	osg::notify(osg::NOTICE) << "   ע�⣺ �޽��������豣֤���в���������ȷ�����������н������г����ܲ�������������ѡ" << std::endl;
}



int main(int argc, char** argv)
{
	osg::ArgumentParser arguments(&argc, argv);
	arguments.getApplicationUsage()->setApplicationName(arguments.getApplicationName());
	arguments.getApplicationUsage()->setDescription(arguments.getApplicationName() + " ����osg���ݵĹ��߼���.");
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

	//���ȼ���Ƿ��н���
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

	//�޽�����������ʾ��Ϣ
	if (!hasUI)
	{
		osg::notify(osg::NOTICE) << "�޽����������������..." << std::endl;
	}
	//�ȴ��ⲿ���ó���ִ�����
	WaitForSingleObject(shExecInfo.hProcess, INFINITE);
	if (!hasUI)
	{
		osg::notify(osg::NOTICE) << "�޽���������н���..." << std::endl;
	}

	return 0;
}

