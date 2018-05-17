#include "stdafx.h"
#include "PickHandler.h"
#include <osgViewer/View>

CPickHandler::CPickHandler()
{
}


CPickHandler::~CPickHandler()
{
}


bool CPickHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
	if (!view) return false;

	switch (ea.getEventType())
	{
		case(osgGA::GUIEventAdapter::PUSH) :
		{

			break;
		}
		case(osgGA::GUIEventAdapter::RELEASE) :
		{

			break;
		}
		default:
			break;
	}
	return false;
}