#pragma once
#include <osgGA/GUIEventHandler>
#include <osgViewer/View>
#include <memory>

namespace capture
{
	class CEmptyCameraHandler;
	class CSnapPara;

	/*
	* 选择点并可以改变视点
	*/
	class CPickHandler : public osgGA::GUIEventHandler
	{
	public:
		CPickHandler(CEmptyCameraHandler* emptyCamera, std::shared_ptr<CSnapPara> para);
		virtual ~CPickHandler();

		virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

	protected:
		virtual void switchSnapView(osg::Vec3d eye, osg::Vec3d center, osg::Vec3d up);

		virtual void clearGroup(osg::ref_ptr<osg::Group> group);

		const osg::Matrix computeMVPW(osgViewer::View* view) const;

	protected:
		osg::observer_ptr<CEmptyCameraHandler> mEmptyCamera;

		std::shared_ptr<CSnapPara> mPara;
	};
}

