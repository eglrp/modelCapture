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

#include <osgGA/StateSetManipulator>

#include <thread>

#include "osgViewer/CompositeViewer"
#include "EmptyCameraHandler.h"
#include "PickHandler.h"
#include "IDrawer.h"
#include "SnapPara.h"

using namespace std;
using namespace osg;
using namespace osgGA;
using namespace osgDB;
using namespace capture;

std::mutex mut;

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


void CCapture::autoCaptureImage(osg::ref_ptr<osg::Node> loadedModel, std::string outFileName, int screenCaptureWidth, int screenCaptureHeight,
	double eyeX, double eyeY, double eyeZ,
	double centerX, double centerY, double centerZ,
	double upX, double upY, double upZ)
{
	// load the data
	if (!loadedModel)
	{
		std::cout << ": No data loaded" << std::endl;
		return;
	}

	Vec3d eye(eyeX, eyeY, eyeZ);
	Vec3d center(centerX, centerY, centerZ);
	Vec3d up(upX, upY, upZ);
	
	// Construct the viewer and register options arguments.
	osgViewer::Viewer viewer;

	// Get user specified number of DatabaseThreads
	int dbThreads = 2;

	// Rendering mode is passive by default
	bool activeMode = false;

	//使用离屏渲染
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



void CCapture::preview(shared_ptr<CSnapPara> para)
{
	std::thread t(&CCapture::previewImplement, this, para);
	t.detach();
}

void CCapture::previewImplement(shared_ptr<CSnapPara> para)
{
	double radius = para->mRadius;
	double intervalX = para->mIntervalX;
	double intervalY = para->mIntervalY;
	Vec3d center = para->mCenter;
	Vec3d up = para->mUp;
	osg::ref_ptr<osg::Node> model = para->mSceneNode;
	osg::ref_ptr<osg::Node> snapNode = model;

	mRoot = new osg::Group;
	osg::ref_ptr<osg::Group> sceneGroup = new osg::Group;
	osg::ref_ptr<osg::Group> sphereGroup = new osg::Group;
	osg::ref_ptr<osg::Group> cameraPointGroup = new osg::Group;
	osg::ref_ptr<osg::Group> highLightPointGroup = new osg::Group;

	mRoot->addChild(sceneGroup);
	mRoot->addChild(sphereGroup);
	mRoot->addChild(cameraPointGroup);
	mRoot->addChild(highLightPointGroup);

	//加载场景
	sceneGroup->addChild(model);

	//创建gc
	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
	traits->x = 100;
	traits->y = 100;
	traits->width = 1000;
	traits->height = 800;
	traits->windowDecoration = true;
	traits->doubleBuffer = true;
	traits->sharedContext = 0;

	osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext(traits.get());
	if (gc.valid())
	{
		osg::notify(osg::INFO) << "  GraphicsWindow has been created successfully." << std::endl;

		// need to ensure that the window is cleared make sure that the complete window is set the correct colour
		// rather than just the parts of the window that are under the camera's viewports
		gc->setClearColor(osg::Vec4f(0.2f, 0.2f, 0.6f, 1.0f));
		gc->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	else
	{
		osg::notify(osg::NOTICE) << "  GraphicsWindow has not been created successfully." << std::endl;
	}

	//创建复合viewer
	osg::ref_ptr<osgViewer::CompositeViewer> viewer = new osgViewer::CompositeViewer();
	osg::ref_ptr<CEmptyCameraHandler> emptyCamera = new CEmptyCameraHandler();
	
	//创建主view
	{
		osg::ref_ptr<osgViewer::View> view1 = new osgViewer::View;
		view1->setName("main view");
		view1->setSceneData(mRoot);
		view1->getCamera()->setGraphicsContext(gc.get());
		view1->setCameraManipulator(new osgGA::TrackballManipulator);
		view1->addEventHandler(new osgViewer::WindowSizeHandler);
		view1->getCamera()->setViewport(new osg::Viewport(0, 0, traits->width, traits->height));
		//禁用裁剪小细节
		osg::CullStack::CullingMode cullingMode = view1->getCamera()->getCullingMode();
		cullingMode &= ~(osg::CullStack::SMALL_FEATURE_CULLING);
		view1->getCamera()->setCullingMode(cullingMode);
		viewer->addView(view1);

		osg::ref_ptr<CPickHandler> pickHandler = new CPickHandler(emptyCamera, para);
		view1->addEventHandler(pickHandler);
	}
	
	{
		//创建截图展示view
		osg::ref_ptr<osgViewer::View> view2 = new osgViewer::View;
		view2->setName("sub view");
		view2->setSceneData(snapNode);
		view2->getCamera()->setViewport(new osg::Viewport(0, 0, 300, 300));
		view2->getCamera()->setClearColor(osg::Vec4d(0.2, 0.2, 0.2, 1));
		view2->getCamera()->setGraphicsContext(gc.get());

		view2->setCameraManipulator(emptyCamera);

		auto vecCameraPos = para->calAllCameraPoint();

		if (vecCameraPos.size() > 0)
		{
			auto pt = vecCameraPos[vecCameraPos.size() - 1];
			emptyCamera->setTransformation(pt, center, up);

		
		}

		viewer->addView(view2);
	}

	//绘制场景展示
	drawGraphic(para, mRoot);

	while (!viewer->done())
	{
		lock_guard<mutex> gurad(mut);
		viewer->frame();
	}

	bPreview = false;
	
}

void CCapture::refresh(shared_ptr<CSnapPara> para)
{
	if (!bPreview)
	{
		return;
	}

	lock_guard<mutex> gurad(mut);

	clearGraphic(mRoot);
	drawGraphic(para, mRoot);
}

void CCapture::setPreview()
{
	bPreview = true;
}

void CCapture::drawGraphic(std::shared_ptr<CSnapPara> para, osg::ref_ptr<Group> root)
{
	osg::ref_ptr<osg::Group> sphereGroup = root->getChild(1)->asGroup();
	osg::ref_ptr<osg::Group> cameraPointGroup = root->getChild(2)->asGroup();


	shared_ptr<IDrawer> iDrawer = IDrawerFactory::create();

	//绘制球体
	Vec3d center = para->mCenter;
	double radius = para->mRadius;
	double latMin = para->mMinLatitude;
	double latMax = para->mMaxLatitude;
	double longMin = para->mMinLongitude;
	double longMax = para->mMaxLongitude;

	ref_ptr<Node> sphere = iDrawer->drawBaseShpere(center, radius);
	sphereGroup->addChild(sphere);

	//绘制相机位置
	ref_ptr<Node> cameraPos = drawCameraPosition(para);
	cameraPointGroup->addChild(cameraPos);

	//绘制半椎体
	ref_ptr<Node> fan = iDrawer->drawFan(center, radius, latMin, latMax, longMin, longMax);
	sphereGroup->addChild(fan);
}


void CCapture::clearGraphic(osg::ref_ptr<Group> root)
{
	osg::ref_ptr<osg::Group> sphereGroup = root->getChild(1)->asGroup();
	osg::ref_ptr<osg::Group> cameraPointGroup = root->getChild(2)->asGroup();

	int numSphereChild = sphereGroup->getNumChildren();
	sphereGroup->removeChildren(0, numSphereChild);

	int numCameraChild = cameraPointGroup->getNumChildren();
	cameraPointGroup->removeChildren(0, numCameraChild);
}

Node* CCapture::drawCameraPosition(shared_ptr<CSnapPara> para)
{
	ref_ptr<Group> grp = new Group;
	shared_ptr<IDrawer> iDrawer = IDrawerFactory::create();

	auto vecEye = para->calAllCameraPoint();

	for (auto eye : vecEye)
	{
		ref_ptr<Geode> pt = iDrawer->drawBasePoint(eye, Vec4d(1, 0, 0, 1));
		grp->addChild(pt);
	}

	return grp.release();
}

