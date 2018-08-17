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

	char* errMsg = "�����������������-h �� --help�鿴�÷�";
	char* supportMsg = "��֧�ֵĹ������������-h �� --help�鿴�÷�";
	char* funcMsg = "���ܲ�������������-h �� --help�鿴�÷�";
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
	osg::notify(osg::NOTICE) << "    [��������]" << std::endl;
	osg::notify(osg::NOTICE) << "    --face					- �����ɼ��沿ʶ��ѵ����������" << std::endl;
	osg::notify(osg::NOTICE) << "    --back					- �����ɼ�ǰ�󱳾�ѵ����������" << std::endl;
	osg::notify(osg::NOTICE) << std::endl;
	osg::notify(osg::NOTICE) << "    --ui					- ��ʾUI����" << std::endl;
	osg::notify(osg::NOTICE) << std::endl;
	osg::notify(osg::NOTICE) << "    [�ɼ��沿ʶ��ѵ���������ܲ�������]" << std::endl;
	osg::notify(osg::NOTICE) << "    -m					- ����ģ���ļ�" << std::endl;
	osg::notify(osg::NOTICE) << "    -f					- ������沿�������ļ�" << std::endl;
	osg::notify(osg::NOTICE) << "    -o					- ��������ͼ·�������Ҫ��б�ܣ�" << std::endl;
	osg::notify(osg::NOTICE) << std::endl;
	osg::notify(osg::NOTICE) << "    [�ɼ��沿ʶ��ѵ���������ܲ�����ѡ]" << std::endl;
	osg::notify(osg::NOTICE) << "    --height			- ���Ӱ��߶�" << std::endl;
	osg::notify(osg::NOTICE) << "    --width			- ���Ӱ����" << std::endl;
	osg::notify(osg::NOTICE) << "    --dx				- γ�ȼ��" << std::endl;
	osg::notify(osg::NOTICE) << "    --dy				- ���ȼ��" << std::endl;
	osg::notify(osg::NOTICE) << "    --minX				- γ����Сֵ" << std::endl;
	osg::notify(osg::NOTICE) << "    --maxX				- γ�����ֵ" << std::endl;
	osg::notify(osg::NOTICE) << "    --minY				- ������Сֵ" << std::endl;
	osg::notify(osg::NOTICE) << "    --maxY				- �������ֵ" << std::endl;
	osg::notify(osg::NOTICE) << std::endl;

	osg::notify(osg::NOTICE) << "    [�ɼ�ǰ�󱳾�ѵ���������ܲ�������]" << std::endl;
	osg::notify(osg::NOTICE) << "    -m					- ����ģ���ļ�" << std::endl;
	osg::notify(osg::NOTICE) << "    -o					- ��������ͼ" << std::endl;
	osg::notify(osg::NOTICE) << "    --height			- ���Ӱ��߶�" << std::endl;
	osg::notify(osg::NOTICE) << "    --width			- ���Ӱ����" << std::endl;
	osg::notify(osg::NOTICE) << "    --focal			- ������������" << std::endl;
	osg::notify(osg::NOTICE) << "    --ccd				- ��Ԫ��С" << std::endl;
	osg::notify(osg::NOTICE) << "    -t					- rt�����е�t" << std::endl;
	osg::notify(osg::NOTICE) << "    -r					- rt�����е�r" << std::endl;
	osg::notify(osg::NOTICE) << std::endl;
	osg::notify(osg::NOTICE) << "    ע�⣺�����ʽ����  X1,Y1,X2,Y2,X3,Y3 ... ͬһ������֮���á�,�����ӣ��޽��������豣֤���в���������ȷ�����������н������г����ܲ�������������ѡ" << std::endl;
}


//�ַ����ָ�
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

	//��ⷶΧ����
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

	osg::notify(osg::NOTICE) << "����ģ�� " << sInModel << std::endl;
	osg::notify(osg::NOTICE) << "�����Ƭ " << sOut << std::endl;
	osg::notify(osg::NOTICE) << "������� " << sFocal << std::endl;
	osg::notify(osg::NOTICE) << "�����Ԫ��С " << sCCD << std::endl;
	osg::notify(osg::NOTICE) << "���rt������t " << sTMat << std::endl;
	osg::notify(osg::NOTICE) << "Ӱ��� " << sImageWidth << "Ӱ��� "<< sImageHeight << std::endl;
	osg::notify(osg::NOTICE) << "���rt������r " << sRMat << std::endl;

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

	//�޽�����������ʾ��Ϣ
	osg::notify(osg::NOTICE) << "�޽����������������..." << std::endl;

	//�ȴ��ⲿ���ó���ִ�����
	WaitForSingleObject(shExecInfo.hProcess, INFINITE);

	osg::notify(osg::NOTICE) << "�޽���������н���..." << std::endl;

	return 1;
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

