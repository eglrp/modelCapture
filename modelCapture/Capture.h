#pragma once
#include "ICapture.h"
#include <osg/Vec3d>
#include <osg/Node>
#include <mutex>

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

		/*
		** brief 预览
		** param sceneFileName 捕捉的场景文件 全路径
		** param radius 半径长度
		** param interval 间距
		*/
		virtual void preview(std::string sceneFileName, double radius, int interval);

		/*
		** brief 具体执行预览的函数
		** param 
		*/
		virtual void previewImplement(std::string sceneFileName, double radius, int interval);

	protected:
		/*
		** brief 计算所有球面上的拍摄点
		** param center 球体中心
		** param radius 半径长度
		*/
		std::vector<osg::Vec3d> calAllCameraPoint(int interval, double radius, osg::Vec3d center);

		/*
		** brief 绘制基础球体
		** param center 球体中心
		** param radius 半径长度
		*/
		osg::Node* drawBaseShpere(const osg::Vec3d &center, double radius);

		/*
		** brief 绘制相机位置
		** param interval 间隔
		** param radius 半径
		** param center 球中心
		*/
		osg::Node* drawCameraPosition(int interval, double radius, osg::Vec3d center);

		/*
		** brief 绘制相机位置
		** param interval 间隔
		*/
		osg::Geode* drawBasePoint(osg::Vec3d pt, osg::Vec4d clr);

		/*
		** brief 创建点样式
		** param size 点大小
		*/
		osg::StateSet* makePtState(int size);

	protected:
		std::vector<osg::Vec3d> mVecCameraPos;


	};
}

