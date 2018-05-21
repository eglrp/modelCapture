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
		** brief ���ƻ�������
		** param center ��������
		** param radius �뾶����
		*/
		virtual osg::Node* drawBaseShpere(const osg::Vec3d &center, double radius);

		/*
		** brief ��������
		** param center ��������
		** param radius �뾶����
		** param γ������ latMin latMax
		** param �������� longMin longMax
		*/
		virtual osg::Node* drawFan(const osg::Vec3d &center, double radius, double latMin, double latMax, double longMin, double longMax);

		/*
		** brief �������λ��
		** param interval ���
		*/
		virtual osg::Geode* drawBasePoint(osg::Vec3d pt, osg::Vec4d clr);

		virtual osg::Geode* drawPolygon(std::vector<osg::Vec3d> coord, osg::Vec4d color);

	protected:
		/*
		** brief ��������ʽ
		** param size ���С
		*/
		osg::StateSet* makePtState(int size);

		/*
		** brief ת������γ�ȵ�xyz
		** param lat γ��
		** param longtitude ����
		** param center ����
		** param radius �뾶
		*/
		void convertLatLongToXyz(double lat, double longtitude, osg::Vec3d &xyz, const osg::Vec3d &center, double radius);

		osg::Geode* drawBaseCurLine(std::vector<osg::Vec3d> coord);

		osg::StateSet* makeLineState();
	};
}



