#pragma once
#include <osgGA/TrackballManipulator>
#include "EmptyCameraHandler.h"

namespace capture
{
	/*
	*  ȥ��trackball�ٿ��������еĿ����¼�, ʹ�û��澲ֹ
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



