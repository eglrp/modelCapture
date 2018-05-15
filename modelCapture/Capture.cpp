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

	//Ê¹ÓÃÀëÆÁäÖÈ¾
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