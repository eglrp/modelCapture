#pragma once
#include <vector>
#include <osg/Vec3d>
#include <osg/Node>

namespace capture
{
	class CSnapPara
	{
	public:
		CSnapPara();
		virtual ~CSnapPara();

		std::vector<osg::Vec3d> calAllCameraPoint();

	public:

		double mRadius = 0;

		int mInterval = 0;

		osg::Vec3d mCenter = osg::Vec3d(0, 0, 0);

		CString mSceneFileName = "";

		CString mOutFile = "";

		int mImageWidth = 0;

		int mImageHeight = 0;

		osg::ref_ptr<osg::Node> mSceneNode;

		double mMinLatitude = 0;

		double mMaxLatitude = 0;	

		double mMinLongitude = 0;

		double mMaxLongitude = 0;
	};

}

