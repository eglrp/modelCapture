#include "stdafx.h"
#include "Drawer.h"
#include <osg/Shape>
#include <osg/Geode>
#include <osg/BlendFunc>
#include <osg/ShapeDrawable>
#include <osg/Geometry>
#include <osg/Point>
#include <osg/LineWidth>
#include <osg/LineStipple>

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

	osg::ref_ptr<osg::Group> group = new osg::Group;

	std::vector<osg::Vec3d> vecTop;
	std::vector<osg::Vec3d> vecLeft;
	std::vector<osg::Vec3d> vecRight;
	std::vector<osg::Vec3d> vecBottom;

	for (double lat = latMin; lat < latMax; lat++)
	{
		osg::Vec3d xyz;
		convertLatLongToXyz(lat, longMax, xyz, center, radius);
		vecTop.push_back(xyz);
	}

	for (double lat = latMin; lat < latMax; lat++)
	{
		osg::Vec3d xyz;
		convertLatLongToXyz(lat, longMin, xyz, center, radius);
		vecBottom.push_back(xyz);
	}

	for (double lon = longMin; lon < longMax; lon++)
	{
		osg::Vec3d xyz;
		convertLatLongToXyz(latMin, lon, xyz, center, radius);
		vecLeft.push_back(xyz);
	}

	for (double lon = longMin; lon < longMax; lon++)
	{
		osg::Vec3d xyz;
		convertLatLongToXyz(latMax, lon, xyz, center, radius);
		vecRight.push_back(xyz);
	}

	osg::Geode* topArc = drawBaseCurLine(vecTop);
	osg::Geode* bottomArc = drawBaseCurLine(vecBottom);
	osg::Geode* leftArc = drawBaseCurLine(vecLeft);
	osg::Geode* rightArc = drawBaseCurLine(vecRight);
	group->addChild(topArc);
	group->addChild(bottomArc);
	group->addChild(leftArc);
	group->addChild(rightArc);

	return group.release();
}

osg::Geode* CDrawer::drawBaseCurLine(std::vector<osg::Vec3d> coord)
{
	//������
	Geode* geoCurLine = new Geode();
	ref_ptr<Geometry> geomCurLine = new Geometry();
	ref_ptr<Vec4Array> curLineColor = new Vec4Array();
	ref_ptr<Vec3Array> curLineArray = new Vec3Array();

	for (int i = 0; i < coord.size(); i++)
	{
		curLineArray->push_back(coord[i]);
	}

	curLineColor->push_back(Vec4d(1, 0, 1, 1));
	geomCurLine->setVertexArray(curLineArray);
	geomCurLine->setColorArray(curLineColor);
	geomCurLine->setColorBinding(Geometry::BIND_OVERALL);
	geomCurLine->addPrimitiveSet(new DrawArrays(PrimitiveSet::LINE_STRIP, 0, curLineArray->size()));
	geoCurLine->addDrawable(geomCurLine);

	//��������ʽ
	geoCurLine->setStateSet(makeLineState());

	//�����߿�
	ref_ptr <LineWidth> LineSize = new LineWidth;
	LineSize->setWidth(4.5f);
	geomCurLine->getOrCreateStateSet()->setAttributeAndModes(LineSize.get(), StateAttribute::ON);
	return geoCurLine;
}

Geode* CDrawer::drawBasePoint(Vec3d pt, Vec4d clr)
{
	//���Ƶ�
	ref_ptr<Geode> geode = new Geode();
	ref_ptr<Geometry> geomPt = new Geometry();
	//�趨��
	ref_ptr<Vec3Array> ptArray = new Vec3Array();
	ptArray->push_back(pt);
	geomPt->setVertexArray(ptArray);
	//�趨��ɫ
	ref_ptr<Vec4Array> colorArray = new Vec4Array();
	colorArray->push_back(clr);
	geomPt->setColorArray(colorArray);
	geomPt->setColorBinding(Geometry::BIND_PER_VERTEX);

	//�趨����ʽ
	double size = 4.5f;
	ref_ptr<StateSet> stateSet = makePtState(size);
	geomPt->setStateSet(stateSet);

	ref_ptr<DrawArrays> drawArray = new DrawArrays(PrimitiveSet::POINTS, 0, ptArray->size());
	//��Ӽ��νڵ�
	geomPt->addPrimitiveSet(drawArray);
	geode->addDrawable(geomPt);

	//��������
	geode->setName("point");
	return geode.release();
}

StateSet* CDrawer::makePtState(int size)
{
	//������ʽ
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
	longtitude -= 90;
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

StateSet* CDrawer::makeLineState()
{
	//������ʽ
	StateSet* stateset = new StateSet();
	ref_ptr<LineStipple> linestipple = new LineStipple;
	linestipple->setFactor(1);
	stateset->setAttributeAndModes(linestipple, StateAttribute::OVERRIDE | StateAttribute::ON);
	stateset->setMode(GL_DEPTH_TEST, StateAttribute::OFF);
	stateset->setMode(GL_LIGHTING, StateAttribute::OFF);
	return stateset;
}