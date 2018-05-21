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
		** brief ���ƻ�������
		** param center ��������
		** param radius �뾶����
		*/
		virtual osg::Node* drawBaseShpere(const osg::Vec3d &center, double radius) = 0;

		/*
		** brief ��������
		** param center ��������
		** param radius �뾶����
		** param γ������ latMin latMax
		** param �������� longMin longMax
		*/
		virtual osg::Node* drawFan(const osg::Vec3d &center, double radius, double latMin, double latMax, double longMin, double longMax) = 0;

		/*
		** brief �������λ��
		** param interval ���
		*/
		virtual osg::Geode* drawBasePoint(osg::Vec3d pt, osg::Vec4d clr) = 0;
	};

	class IDrawerFactory
	{
	public:
		static std::shared_ptr<IDrawer> create();
	};
}
