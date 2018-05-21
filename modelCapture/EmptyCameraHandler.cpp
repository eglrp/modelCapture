#include "stdafx.h"
#include "EmptyCameraHandler.h"

using namespace capture;
using namespace std;
using namespace osg;
using namespace osgGA;

CEmptyCameraHandler::CEmptyCameraHandler()
{
}


CEmptyCameraHandler::~CEmptyCameraHandler()
{
}


bool CEmptyCameraHandler::handle(const GUIEventAdapter& ea, GUIActionAdapter& us)
{
	return false;
}