#pragma once
#include <osgGA/GUIEventHandler>
#include <memory>
#include <osgViewer/View>

namespace capture
{
	class CCapture;
	class CSnapPara;

	class CAdjustBorderEvent : public osgGA::GUIEventHandler
	{
	public:
		CAdjustBorderEvent(std::shared_ptr<CSnapPara> snapPara, osg::Group* root, CCapture* cap);
		virtual ~CAdjustBorderEvent();

		virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

	protected:
		void switchIndex();

	protected:
		osg::ref_ptr<osg::Group> mRoot;

		std::shared_ptr<CSnapPara> mSnapPara;

		CCapture* mCapture;
	};
}
