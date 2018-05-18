#include "stdafx.h"
#include "SnapPara.h"

using namespace capture;
using namespace osg;
using namespace std;

CSnapPara::CSnapPara()
{
}


CSnapPara::~CSnapPara()
{
}


vector<Vec3d> CSnapPara::calAllCameraPoint()
{
	vector<Vec3d> vecCameraPos;

	for (int latitude = -180; latitude <= 180; latitude = latitude + mInterval)
	{
		for (int longtitude = -180; longtitude <= 180; longtitude = longtitude + mInterval)
		{
			double t = latitude;
			double p = longtitude;

			if (t >= mMinLatitude && t <= mMaxLatitude && p >= mMinLongitude && p <= mMaxLongitude)
			{
				double x = mRadius * cos(t / 180 * PI) * cos(p / 180 * PI) + mCenter.x();
				double y = mRadius * cos(t / 180 * PI) * sin(p / 180 * PI) + mCenter.y();
				double z = mRadius * sin(t / 180 * PI) + mCenter.z();

				vecCameraPos.push_back(Vec3d(x, y, z));
			}
		
		}
	}

	return vecCameraPos;
}