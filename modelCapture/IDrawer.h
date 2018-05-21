#pragma once
#include <osg/Node>
#include <osg/Vec3d>
#include <memory>

namespace capture
{
	class IDrawer
	{
	public:
		IDrawer();
		virtual ~IDrawer();

		/*
		** brief 绘制基础球体
		** param center 球体中心
		** param radius 半径长度
		*/
		virtual osg::Node* drawBaseShpere(const osg::Vec3d &center, double radius) = 0;

		/*
		** brief 绘制扇体
		** param center 球体中心
		** param radius 半径长度
		** param 纬度限制 latMin latMax
		** param 经度限制 longMin longMax
		*/
		virtual osg::Node* drawFan(const osg::Vec3d &center, double radius, double latMin, double latMax, double longMin, double longMax) = 0;

		/*
		** brief 绘制相机位置
		** param interval 间隔
		*/
		virtual osg::Geode* drawBasePoint(osg::Vec3d pt, osg::Vec4d clr) = 0;
	};

	class IDrawerFactory
	{
	public:
		static std::shared_ptr<IDrawer> create();
	};
}
