#include "stdafx.h"
#include "EmptyCameraHandler.h"

using namespace capture;

CEmptyCameraHandler::CEmptyCameraHandler()
{
}


CEmptyCameraHandler::~CEmptyCameraHandler()
{
}


bool CEmptyCameraHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
{
	return false;
}