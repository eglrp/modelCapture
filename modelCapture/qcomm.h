//---------------------------------------------------------------------------

#ifndef QCOMM_DEFINE
#define QCOMM_DEFINE

#define MAX_LOG_LENGTH	255

#include <Sys/timeb.h>

#ifndef DEPENDENCYAPI
#define DEPENDENCYAPI __declspec(dllexport)
#endif

struct timeb _curMsTime(int flagSet=1,char*ptext = NULL);//��������һ�ε��õļ��ʱ�䣬1���������ã�2��������

int  _log(char* ptext,int level=0);//������־

DEPENDENCYAPI int  _logop(char* ptext,int level=0);//������־

void test(int &ia,int &ib);

struct timeb _getCurMillionSeconds();//��ȡ��ǰϵͳʱ�䣨timeb ��ȷ�����룩

char* _getCurrentStrTimeb(char*pca,int flag=0);//��ȡ��ǰ���뼶�ַ���


void _getLastError();//��ȡϵͳ������Ϣ
//---------------------------------------------------------------------------
#endif
