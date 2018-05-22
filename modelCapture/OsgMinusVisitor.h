#pragma once
#include<osg/NodeVisitor>

class COsgMinusVisitor : public osg::NodeVisitor
{
public:
	COsgMinusVisitor(double x, double y, double z);
	virtual ~COsgMinusVisitor();

	virtual void apply(osg::Geode& geode);

protected:
	double mX;

	double mY;

	double mZ;

};

