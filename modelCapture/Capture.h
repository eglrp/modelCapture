#pragma once
#include "ICapture.h"
#include <osg/Vec3d>
#include <osg/Node>
#include <mutex>
#include <osg/Group>

namespace capture
{
	class CSnapPara;
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
		virtual void autoCaptureImage(osg::ref_ptr<osg::Node> loadedModel, std::string outFileName, int screenCaptureWidth, int screenCaptureHeight,
			double eyeX, double eyeY, double eyeZ,
			double centerX, double centerY, double centerZ,
			double upX, double upY, double upZ);

		/*
		** brief 预览
		** param sceneFileName 捕捉的场景文件 全路径
		** param para 界面参数
		*/
		virtual void preview(std::shared_ptr<CSnapPara> para);

		/*
		** brief 具体执行预览的函数
		** param para 界面参数
		*/
		virtual void previewImplement(std::shared_ptr<CSnapPara> para);

	protected:
		void drawGraphic(std::shared_ptr<CSnapPara> para, osg::ref_ptr<osg::Group> root);
		/*
		** brief 绘制相机位置
		** param para 界面参数
		*/
		osg::Node* drawCameraPosition(std::shared_ptr<CSnapPara> para);
	};
}

