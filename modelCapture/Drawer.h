#pragma once
#include <osg/Node>
#include <osg/Vec3d>
#include "IDrawer.h"

namespace capture
{
	class CDrawer : public IDrawer
	{
	public:
		CDrawer();
		virtual ~CDrawer();

		/*
		** brief 绘制基础球体
		** param center 球体中心
		** param radius 半径长度
		*/
		virtual osg::Node* drawBaseShpere(const osg::Vec3d &center, double radius);

		/*
		** brief 绘制扇体
		** param center 球体中心
		** param radius 半径长度
		** param 纬度限制 latMin latMax
		** param 经度限制 longMin longMax
		*/
		virtual osg::Node* drawFan(const osg::Vec3d &center, double radius, double latMin, double latMax, double longMin, double longMax);

		/*
		** brief 绘制相机位置
		** param interval 间隔
		*/
		virtual osg::Geode* drawBasePoint(osg::Vec3d pt, osg::Vec4d clr);

		virtual osg::Geode* drawPolygon(std::vector<osg::Vec3d> coord, osg::Vec4d color);

	protected:
		/*
		** brief 创建点样式
		** param size 点大小
		*/
		osg::StateSet* makePtState(int size);

		/*
		** brief 转换经度纬度到xyz
		** param lat 纬度
		** param longtitude 经度
		** param center 中心
		** param radius 半径
		*/
		void convertLatLongToXyz(double lat, double longtitude, osg::Vec3d &xyz, const osg::Vec3d &center, double radius);
	};
}



