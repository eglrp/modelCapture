#include "stdafx.h"
#include "Drawer.h"
#include <osg/Shape>
#include <osg/Geode>
#include <osg/BlendFunc>
#include <osg/ShapeDrawable>
#include <osg/Geometry>
#include <osg/Point>

using namespace capture;
using namespace osg;


CDrawer::CDrawer()
{
}


CDrawer::~CDrawer()
{
}



Node* CDrawer::drawBaseShpere(const osg::Vec3d &center, double radius)
{
	ref_ptr<Sphere> sphere = new Sphere(center, radius);
	ref_ptr<Geode> unitSphere = new Geode;

	osg::ref_ptr<osg::BlendFunc> blendFunc = new osg::BlendFunc();
	blendFunc->setSource(osg::BlendFunc::SRC_ALPHA);
	blendFunc->setDestination(osg::BlendFunc::ONE_MINUS_CONSTANT_ALPHA);


	ref_ptr<osg::ShapeDrawable> shapeDrawable = new osg::ShapeDrawable(sphere.get());
	shapeDrawable->setColor(osg::Vec4d(0, 1, 0, 0.5));
	unitSphere->getOrCreateStateSet()->setAttributeAndModes(blendFunc);

	unitSphere->addDrawable(shapeDrawable.get());

	return unitSphere.release();
}

osg::Node* CDrawer::drawFan(const osg::Vec3d &center, double radius, double latMin, double latMax, double longMin, double longMax)
{
	Vec3d leftBottom, rightBottom, leftTop, rightTop;
	convertLatLongToXyz(latMin, longMin, leftBottom, center, radius);
	convertLatLongToXyz(latMax, longMin, rightBottom, center, radius);
	convertLatLongToXyz(latMin, longMax, leftTop, center, radius);
	convertLatLongToXyz(latMax, longMax, rightTop, center, radius);

	osg::ref_ptr<osg::Group> group = new osg::Group;

	{
		std::vector<osg::Vec3d> vecLeftSide;
		vecLeftSide.push_back(center);
		vecLeftSide.push_back(leftBottom);
		vecLeftSide.push_back(leftTop);
		osg::ref_ptr<osg::Node> leftSide = drawPolygon(vecLeftSide, osg::Vec4d(1, 1, 0, 0.5));
		group->addChild(leftSide);
	}

	{
		std::vector<osg::Vec3d> vecRightSide;
		vecRightSide.push_back(center);
		vecRightSide.push_back(rightBottom);
		vecRightSide.push_back(rightTop);
		osg::ref_ptr<osg::Node> rightSide = drawPolygon(vecRightSide, osg::Vec4d(1, 1, 0, 0.5));
		group->addChild(rightSide);
	}

	{
		std::vector<osg::Vec3d> vecTopSide;
		vecTopSide.push_back(center);
		vecTopSide.push_back(leftTop);
		vecTopSide.push_back(rightTop);
		osg::ref_ptr<osg::Node> topSide = drawPolygon(vecTopSide, osg::Vec4d(1, 1, 0, 0.5));
		group->addChild(topSide);
	}

	{
		std::vector<osg::Vec3d> vecBottomSide;
		vecBottomSide.push_back(center);
		vecBottomSide.push_back(leftBottom);
		vecBottomSide.push_back(rightBottom);
		osg::ref_ptr<osg::Node> bottomSide = drawPolygon(vecBottomSide, osg::Vec4d(1, 1, 0, 0.5));
		group->addChild(bottomSide);
	}

	return group.release();
}

Geode* CDrawer::drawBasePoint(Vec3d pt, Vec4d clr)
{
	//绘制点
	ref_ptr<Geode> geode = new Geode();
	ref_ptr<Geometry> geomPt = new Geometry();
	//设定点
	ref_ptr<Vec3Array> ptArray = new Vec3Array();
	ptArray->push_back(pt);
	geomPt->setVertexArray(ptArray);
	//设定颜色
	ref_ptr<Vec4Array> colorArray = new Vec4Array();
	colorArray->push_back(clr);
	geomPt->setColorArray(colorArray);
	geomPt->setColorBinding(Geometry::BIND_PER_VERTEX);

	//设定点样式
	double size = 4.5f;
	ref_ptr<StateSet> stateSet = makePtState(size);
	geomPt->setStateSet(stateSet);

	ref_ptr<DrawArrays> drawArray = new DrawArrays(PrimitiveSet::POINTS, 0, ptArray->size());
	//添加几何节点
	geomPt->addPrimitiveSet(drawArray);
	geode->addDrawable(geomPt);

	//设置类型
	geode->setName("point");
	return geode.release();
}

StateSet* CDrawer::makePtState(int size)
{
	//设置样式
	ref_ptr<StateSet> set = new StateSet();
	ref_ptr<Point> point = new Point();
	point->setSize(size);
	set->setAttribute(point);
	set->setMode(GL_DEPTH_TEST, StateAttribute::OFF);
	set->setMode(GL_LIGHTING, StateAttribute::OFF);
	return set.release();
}

void CDrawer::convertLatLongToXyz(double lat, double longtitude, osg::Vec3d &xyz, const osg::Vec3d &center, double radius)
{
	double x = radius * cos(lat / 180 * PI) * cos(longtitude / 180 * PI) + center.x();
	double y = radius * cos(lat / 180 * PI) * sin(longtitude / 180 * PI) + center.y();
	double z = radius * sin(lat / 180 * PI) + center.z();

	xyz = osg::Vec3d(x, y, z);
}


osg::Geode* CDrawer::drawPolygon(std::vector<osg::Vec3d> coord, osg::Vec4d color)
{
	osg::ref_ptr<osg::Geode> geode = new osg::Geode();
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry();
	osg::ref_ptr<osg::Vec4Array> plygonColor = new osg::Vec4Array();
	osg::ref_ptr<osg::Vec3Array> plygonArray = new osg::Vec3Array();

	for (int i = 0; i < coord.size(); i++)
	{
		plygonArray->push_back(coord[i]);
	}

	plygonColor->push_back(color);
	geom->setVertexArray(plygonArray);
	geom->setColorArray(plygonColor);
	geom->setColorBinding(osg::Geometry::BIND_OVERALL);

	osg::DrawElementsUInt* geomBase = new osg::DrawElementsUInt(osg::PrimitiveSet::POLYGON, 1);

	for (int i = 0; i < coord.size(); i++)
	{
		geomBase->push_back(i);
	}

	geom->addPrimitiveSet(geomBase);
	geode->addDrawable(geom);

	osg::ref_ptr<osg::BlendFunc> blendFunc = new osg::BlendFunc();
	blendFunc->setSource(osg::BlendFunc::SRC_ALPHA);
	blendFunc->setDestination(osg::BlendFunc::ONE_MINUS_CONSTANT_ALPHA);

	osg::ref_ptr<osg::StateSet> stateset = geode->getOrCreateStateSet();
	stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
	stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	stateset->setAttributeAndModes(blendFunc);
	geode->setStateSet(stateset);

	return geode.release();
}