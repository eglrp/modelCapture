#pragma once
#include <osgGA/TrackballManipulator>
#include "EmptyCameraHandler.h"

namespace capture
{
	/*
	*  去掉trackball操控器中所有的控制事件, 使得画面静止
	*/
	class CEmptyCameraHandler : public osgGA::TrackballManipulator
	{
	public:
		CEmptyCameraHandler();
		virtual ~CEmptyCameraHandler();

		virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);

	protected:

	};
}



