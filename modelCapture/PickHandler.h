#pragma once
#include <osgGA/GUIEventHandler>

/*
* ѡ��㲢���Ըı��ӵ�
*/
class CPickHandler : public osgGA::GUIEventHandler
{
public:
	CPickHandler();
	virtual ~CPickHandler();

	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
};

