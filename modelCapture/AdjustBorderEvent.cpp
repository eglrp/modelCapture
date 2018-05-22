#include "stdafx.h"
#include "AdjustBorderEvent.h"
#include <osgViewer/View>
#include "SnapPara.h"
#include <osgUtil/LineSegmentIntersector>
#include "Capture.h"
#include "windows.h"
#include <WinUser.h>
#include "qcomm.h"
#include "IDrawer.h"

using namespace osg;
using namespace capture;
using namespace osgUtil;


void convertLatLongToXyz(double lat, double longtitude, osg::Vec3d &xyz, const osg::Vec3d &center, double radius)
{
	double x = radius * cos(lat / 180.0 * PI) * cos(longtitude / 180.0 * PI) + center.x();
	double y = radius * cos(lat / 180.0 * PI) * sin(longtitude / 180.0 * PI) + center.y();
	double z = radius * sin(lat / 180.0 * PI) + center.z();

	xyz = osg::Vec3d(x, y, z);
}

void convertXyzToLatLong(const osg::Vec3d &xyz, double &latitude, double &longtitude, const osg::Vec3d &center, double radius)
{
	double diff = DBL_MAX;

	for (double lat = -90; lat < 90; lat++)
	{
		for (double lon = -90; lon < 90; lon++)
		{
			double x = radius * cos(lat / 180.0 * PI) * cos(lon / 180.0 * PI) + center.x();
			double y = radius * cos(lat / 180.0 * PI) * sin(lon / 180.0 * PI) + center.y();
			double z = radius * sin(lat / 180.0 * PI) + center.z();

			double temp = sqrt((xyz.x() - x) * (xyz.x() - x) + (xyz.y() - y) * (xyz.y() - y) + (xyz.z() - z) * (xyz.z() - z));

			if (diff > temp)
			{
				diff = temp;
				latitude = lat;
				longtitude = lon;
			}
		}

	}
}


CAdjustBorderEvent::CAdjustBorderEvent(std::shared_ptr<CSnapPara> snapPara, osg::Group* root, CCapture* cap) :
	mSnapPara(snapPara),
	mRoot(root),
	mCapture(cap)
{

}


CAdjustBorderEvent::~CAdjustBorderEvent()
{
}

bool CAdjustBorderEvent::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
	if (!view) return false;

	switch (ea.getEventType())
	{
		case(osgGA::GUIEventAdapter::PUSH) :
		{
			bool bFlag = (GetKeyState(VK_CONTROL) < 0);

			if (!bFlag)
			{
				return false;
			}

			float mouseX = ea.getX();
			float mouseY = ea.getY();

			osg::ref_ptr<osg::Group> highLightGroup = mRoot->getChild(4)->asGroup();
			osgUtil::LineSegmentIntersector::Intersections intersections;
			view->computeIntersections(mouseX, mouseY, intersections);

			osg::Vec3d worldPt = intersections.begin()->getWorldIntersectPoint();

			double lat = 0;
			double lon = 0;
			osg::Vec3d center = mSnapPara->mCenter;
			double radius = mSnapPara->mRadius;
			convertXyzToLatLong(worldPt, lat, lon, center, radius);

			char strMsg[MAX_PATH];
			sprintf_s(strMsg, "world pt: %lf %lf %lf", worldPt.x(), worldPt.y(), worldPt.z());
			_logop(strMsg);

			sprintf_s(strMsg, "lat: %lf lon: %lf\n", lat, lon);
			_logop(strMsg);

			if (mSnapPara->mSelIndex == -1)
			{
				return false;
			}

			if (mSnapPara->mSelIndex == 0)
			{
				mSnapPara->mMaxLongitude = lon;
			}
			else if (mSnapPara->mSelIndex == 1)
			{
				mSnapPara->mMinLongitude = lon;
			}
			else if (mSnapPara->mSelIndex == 2)
			{
				mSnapPara->mMinLatitude = lat;
			}
			else if (mSnapPara->mSelIndex == 3)
			{
				mSnapPara->mMaxLatitude = lat;
			}

			mCapture->clearGraphic(mRoot);
			mCapture->drawGraphic(mSnapPara, mRoot);

			return true;
		}

		case(osgGA::GUIEventAdapter::KEYDOWN) :
		{
			if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Down)					 //BackSpace
			{
				switchIndex();

				mCapture->clearGraphic(mRoot);
				mCapture->drawGraphic(mSnapPara, mRoot);
			}

			return true;
		}
	}

	return false;
}

void CAdjustBorderEvent::switchIndex()
{
	mSnapPara->mSelIndex++;

	if (mSnapPara->mSelIndex == 4)
	{
		mSnapPara->mSelIndex = 0;
	}
}

