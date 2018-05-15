#pragma once
#include "ICapture.h"
#include <osg/Vec3d>

namespace capture
{
	/*
	** brief 离屏渲染捕捉类
	*/
	class CCapture : public ICapture
	{
	public:
		CCapture();
		virtual ~CCapture();

		/*
		** brief 自动捕捉
		** param sceneFileName 捕捉的场景文件 全路径
		** param outFileName 输出的截图文件 全路径
		** param screenCaptureWidth screenCaptureHeight 截图宽高
		** param eye center up 视点、中心、相机方向
		*/
		virtual void autoCaptureImage(std::string sceneFileName, std::string outFileName, int screenCaptureWidth, int screenCaptureHeight,
			double eyeX, double eyeY, double eyeZ,
			double centerX, double centerY, double centerZ,
			double upX, double upY, double upZ);

	};
}

