#include "stdafx.h"
#include "Capture.h"
#include <OpenThreads/ScopedLock>
#include <osg/ArgumentParser>
#include <osg/CoordinateSystemNode>
#include <osg/Matrix>
#include <osg/NodeVisitor>

#include <osgUtil/IntersectionVisitor>
#include <osgUtil/GLObjectsVisitor>

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

#include <osgGA/DriveManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/TerrainManipulator>
#include <osgGA/TrackballManipulator>

#include <osgTerrain/Terrain>
#include <osgTerrain/GeometryTechnique>

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/Renderer>

#include <iostream>
#include <sstream>
#include <osg/Camera>

#include <osg/BlendFunc>
#include <osg/ShapeDrawable>

#include <osg/Point>

using namespace std;
using namespace osg;
using namespace osgGA;
using namespace osgDB;
using namespace capture;

/** Helper class*/
template<class T>
class FindTopMostNodeOfTypeVisitor : public NodeVisitor
{
public:
	FindTopMostNodeOfTypeVisitor() :
		NodeVisitor(NodeVisitor::TRAVERSE_ALL_CHILDREN),
		_foundNode(0)
	{}

	void apply(Node& node)
	{
		T* result = dynamic_cast<T*>(&node);
		if (result)
			_foundNode = result;
		else
			traverse(node);
	}

	T* _foundNode;
};

/** Convenience function*/
template<class T>
T* findTopMostNodeOfType(Node* node)
{
	if (!node) return 0;

	FindTopMostNodeOfTypeVisitor<T> fnotv;
	node->accept(fnotv);

	return fnotv._foundNode;
}

/** Capture the frame buffer and write image to disk*/
class WindowCaptureCallback : public Camera::DrawCallback
{
public:
	WindowCaptureCallback(GLenum readBuffer, const std::string& name) :
		_readBuffer(readBuffer),
		_fileName(name)
	{
		_image = new Image;
	}

	virtual void operator () (RenderInfo& renderInfo) const
	{
#if !defined(OSG_GLES1_AVAILABLE) && !defined(OSG_GLES2_AVAILABLE)
		glReadBuffer(_readBuffer);
#else
		notify(NOTICE) << "Error: GLES unable to do glReadBuffer" << std::endl;
#endif

		OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);
		GraphicsContext* gc = renderInfo.getState()->getGraphicsContext();
		if (gc->getTraits())
		{
			GLenum pixelFormat;

			if (gc->getTraits()->alpha)
				pixelFormat = GL_RGBA;
			else
				pixelFormat = GL_RGB;

#if defined(OSG_GLES1_AVAILABLE) || defined(OSG_GLES2_AVAILABLE)
			if (pixelFormat == GL_RGB)
			{
				GLint value = 0;
#ifndef GL_IMPLEMENTATION_COLOR_READ_FORMAT
#define GL_IMPLEMENTATION_COLOR_READ_FORMAT 0x8B9B
#endif
				glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_FORMAT, &value);
				if (value != GL_RGB ||
					value != GL_UNSIGNED_BYTE)
				{
					pixelFormat = GL_RGBA;//always supported
				}
			}
#endif
			int width = gc->getTraits()->width;
			int height = gc->getTraits()->height;

			std::cout << "Capture: size=" << width << "x" << height << ", format=" << (pixelFormat == GL_RGBA ? "GL_RGBA" : "GL_RGB") << std::endl;

			_image->readPixels(0, 0, width, height, pixelFormat, GL_UNSIGNED_BYTE);
		}

		if (!_fileName.empty())
		{
			std::cout << "Writing to: " << _fileName << std::endl;
			writeImageFile(*_image, _fileName);
		}
	}

protected:
	GLenum                      _readBuffer;
	std::string                 _fileName;
	ref_ptr<Image>    _image;
	mutable OpenThreads::Mutex  _mutex;
};


/** Do Culling only while loading PagedLODs*/
class CustomRenderer : public osgViewer::Renderer
{
public:
	CustomRenderer(Camera* camera)
		: osgViewer::Renderer(camera),
		_cullOnly(true)
	{
	}

	/** Set flag to omit drawing in renderingTraversals */
	void setCullOnly(bool on) { _cullOnly = on; }

	virtual void operator () (GraphicsContext* /*context*/)
	{
		if (_graphicsThreadDoesCull)
		{
			if (_cullOnly)
				cull();
			else
				cull_draw();
		}
	}

	virtual void cull()
	{
		osgUtil::SceneView* sceneView = _sceneView[0].get();
		if (!sceneView || _done) return;

		updateSceneView(sceneView);

		osgViewer::View* view = dynamic_cast<osgViewer::View*>(_camera->getView());
		if (view) sceneView->setFusionDistance(view->getFusionDistanceMode(), view->getFusionDistanceValue());

		sceneView->inheritCullSettings(*(sceneView->getCamera()));
		sceneView->cull();
	}

	bool _cullOnly;
};




CCapture::CCapture()
{
}


CCapture::~CCapture()
{
}


void CCapture::autoCaptureImage(std::string sceneFileName, std::string outFileName, int screenCaptureWidth, int screenCaptureHeight,
	double eyeX, double eyeY, double eyeZ,
	double centerX, double centerY, double centerZ,
	double upX, double upY, double upZ)
{
	Vec3d eye(eyeX, eyeY, eyeZ);
	Vec3d center(centerX, centerY, centerZ);
	Vec3d up(upX, upY, upZ);
	
	// Construct the viewer and register options arguments.
	osgViewer::Viewer viewer;

	// Get user specified number of DatabaseThreads
	int dbThreads = 2;

	// Rendering mode is passive by default
	bool activeMode = false;

	//ʹ��������Ⱦ
	bool use_pbuffer = true;

	DisplaySettings::instance()->setNumOfDatabaseThreadsHint(dbThreads);

	if (use_pbuffer) {
		DisplaySettings* ds = DisplaySettings::instance().get();
		ref_ptr<GraphicsContext::Traits> traits = new GraphicsContext::Traits(ds);

		if (viewer.getCamera()->getGraphicsContext() && viewer.getCamera()->getGraphicsContext()->getTraits()) {
			//use viewer settings for window size
			ref_ptr<const GraphicsContext::Traits> src_traits = viewer.getCamera()->getGraphicsContext()->getTraits();
			traits->screenNum = src_traits->screenNum;
			traits->displayNum = src_traits->displayNum;
			traits->hostName = src_traits->hostName;
			traits->width = src_traits->width;
			traits->height = src_traits->height;
			traits->red = src_traits->red;
			traits->green = src_traits->green;
			traits->blue = src_traits->blue;
			traits->alpha = src_traits->alpha;
			traits->depth = src_traits->depth;
			traits->pbuffer = true;
		}
		else {
			//viewer would use fullscreen size (unknown here) pbuffer will use 4096 x4096 (or best avaiable)
			traits->width = screenCaptureWidth;
			traits->height = screenCaptureHeight;
			traits->pbuffer = true;
		}
		ref_ptr<GraphicsContext> pbuffer = GraphicsContext::createGraphicsContext(traits.get());
		if (pbuffer.valid())
		{
			notify(NOTICE) << "Pixel buffer has been created successfully." << std::endl;
			ref_ptr<Camera> camera = new Camera(*viewer.getCamera());
			camera->setGraphicsContext(pbuffer.get());
			camera->setViewport(new Viewport(0, 0, traits->width, traits->height));
			GLenum buffer = pbuffer->getTraits()->doubleBuffer ? GL_BACK : GL_FRONT;
			camera->setDrawBuffer(buffer);
			camera->setReadBuffer(buffer);
			viewer.setCamera(camera.get());
		}
		else
		{
			notify(NOTICE) << "Pixel buffer has not been created successfully." << std::endl;
		}
	}



	// load the data
	ref_ptr<Node> loadedModel = readNodeFile(sceneFileName);
	if (!loadedModel)
	{
		std::cout << ": No data loaded" << std::endl;
		return;
	}

	loadedModel->getOrCreateStateSet()->setMode(GL_LIGHTING, 0x2);

	// Compute eye point in world coordiantes
	viewer.getCamera()->setViewMatrixAsLookAt(eye, center, up);


	// Optimize DatabasePager for auto-capture
	ref_ptr<DatabasePager> pager = viewer.getDatabasePager();
	pager->setDoPreCompile(false);

	// Install custom renderer
	ref_ptr<CustomRenderer> customRenderer = new CustomRenderer(viewer.getCamera());
	viewer.getCamera()->setRenderer(customRenderer.get());

	// Override threading model
	viewer.setThreadingModel(osgViewer::Viewer::SingleThreaded);

	// Set the final SceneData to show
	viewer.setSceneData(loadedModel.get());

	// Realize GUI
	viewer.realize();

	//--- Load PageLOD tiles ---

	// Initiate the first PagedLOD request
	viewer.frame();

	Timer_t beforeLoadTick = Timer::instance()->tick();

	// Keep updating and culling until full level of detail is reached
	while (!viewer.done() && pager->getRequestsInProgress())
	{
		//        std::cout <<pager->getRequestsInProgress()<<" ";
		viewer.updateTraversal();
		viewer.renderingTraversals();
	}
	//    std::cout<<std::endl;

	Timer_t afterLoadTick = Timer::instance()->tick();
	std::cout << "Load and Compile time = " << Timer::instance()->delta_s(beforeLoadTick, afterLoadTick) << " seconds" << std::endl;

	// Do cull and draw to render the scene correctly
	customRenderer->setCullOnly(false);


	//--- Capture the image!!! ---
	if (!activeMode)
	{
		// Add the WindowCaptureCallback now that we have full resolution
		GLenum buffer = viewer.getCamera()->getGraphicsContext()->getTraits()->doubleBuffer ? GL_BACK : GL_FRONT;
		viewer.getCamera()->setFinalDrawCallback(new WindowCaptureCallback(buffer, outFileName));

		Timer_t beforeRenderTick = Timer::instance()->tick();

		// Do rendering with capture callback
		viewer.renderingTraversals();

		Timer_t afterRenderTick = Timer::instance()->tick();
		std::cout << "Rendering time = " << Timer::instance()->delta_s(beforeRenderTick, afterRenderTick) << " seconds" << std::endl;

		return;
	}
	else
	{
		viewer.run();
		return;
	}
}



void CCapture::preview(std::string sceneFileName, double radius, int interval)
{
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(sceneFileName);

	osg::ref_ptr<osg::Group> group = new osg::Group;
	group->addChild(node);

	//��������
	osg::BoundingSphere bs = node->getBound();
	Vec3d center = bs.center();
	ref_ptr<Node> sphere = drawBaseShpere(center, radius);
	group->addChild(sphere);

	ref_ptr<Node> cameraPos = drawCameraPosition(interval, radius, center);
	group->addChild(cameraPos);
	
	osgViewer::Viewer viewer;

	//���òü�Сϸ��
	osg::CullStack::CullingMode cullingMode = viewer.getCamera()->getCullingMode();
	cullingMode &= ~(osg::CullStack::SMALL_FEATURE_CULLING);
	viewer.getCamera()->setCullingMode(cullingMode);

	viewer.setSceneData(group);

	viewer.realize();

	viewer.run();
}



Node* CCapture::drawBaseShpere(const osg::Vec3d &center, double radius)
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

Node* CCapture::drawCameraPosition(int interval, double radius, Vec3d center)
{
	ref_ptr<Group> grp = new Group;

	for (int latitude = -180; latitude <= 180; latitude = latitude + interval)
	{
		for (int longtitude = -180; longtitude <= 180; longtitude = longtitude + interval)
		{
			double t = latitude;
			double p = longtitude;

			double x = radius * sin(t / 180 * PI) * cos(p / 180 * PI) + center.x();
			double y = radius * sin(t / 180 * PI) * sin(p / 180 * PI) + center.y();
			double z = radius * cos(t / 180 * PI) + center.z();
			ref_ptr<Geode> pt = drawBasePoint(Vec3d(x, y, z));
			grp->addChild(pt);
		}
	}

	return grp.release();
}

Geode* CCapture::drawBasePoint(Vec3d pt)
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
	colorArray->push_back(Vec4d(1, 0, 0, 1));
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

StateSet* CCapture::makePtState(int size)
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