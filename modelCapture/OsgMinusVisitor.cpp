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
	//看看有多少可绘制结点 
	for (unsigned int i = 0; i < geode.getNumDrawables(); ++i)
	{
		osg::Geometry* geom = dynamic_cast<osg::Geometry*>(geode.getDrawable(i));

		if (!geom)
			continue; //得到可绘制结点的顶点序列 

		osg::Vec3Array* verts = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());

		if (!verts)
			continue; //把顶点序列插入到顶点集中以便输出 

		//根据旋转矩阵计算
		for (int j = 0; j < verts->size(); j++)
		{
			verts->at(j).x() -= mX;
			verts->at(j).y() -= mY;
			verts->at(j).z() -= mZ;
		}

	}
}