#include "stdafx.h"
#include "OsgMinusVisitor.h"
#include <osg/Geometry>
#include <osg/Geode>

COsgMinusVisitor::COsgMinusVisitor(double x, double y, double z) : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN),
	mX(x),
	mY(y),
	mZ(z)
{
}


COsgMinusVisitor::~COsgMinusVisitor()
{
}

void COsgMinusVisitor::apply(osg::Geode& geode)
{
	//�����ж��ٿɻ��ƽ�� 
	for (unsigned int i = 0; i < geode.getNumDrawables(); ++i)
	{
		osg::Geometry* geom = dynamic_cast<osg::Geometry*>(geode.getDrawable(i));

		if (!geom)
			continue; //�õ��ɻ��ƽ��Ķ������� 

		osg::Vec3Array* verts = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());

		if (!verts)
			continue; //�Ѷ������в��뵽���㼯���Ա���� 

		//������ת�������
		for (int j = 0; j < verts->size(); j++)
		{
			verts->at(j).x() -= mX;
			verts->at(j).y() -= mY;
			verts->at(j).z() -= mZ;
		}

	}
}