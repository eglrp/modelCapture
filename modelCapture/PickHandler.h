#pragma once
#include <osgGA/GUIEventHandler>

/*
* 选择点并可以改变视点
*/
class CPickHandler : public osgGA::GUIEventHandler
{
public:
	CPickHandler();
	virtual ~CPickHandler();

	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
};

