#include "stdafx.h"
#include "PickHandler.h"
#include <osgViewer/View>
#include "Capture.h"
#include "EmptyCameraHandler.h"
#include "IDrawer.h"
#include "qcomm.h"
#include "SnapPara.h"

using namespace std;
using namespace osg;
using namespace capture;

const double err = 5;

CPickHandler::CPickHandler(CEmptyCameraHandler* emptyCamera, shared_ptr<CSnapPara> para)
	: mEmptyCamera(emptyCamera),
	mPara(para)
{

}


CPickHandler::~CPickHandler()
{
}


bool CPickHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
	if (!view) return false;

	switch (ea.getEventType())
	{
		case(osgGA::GUIEventAdapter::PUSH) :
		{
			auto vecCameraPos = mPara->calAllCameraPoint();

			for (auto pos : vecCameraPos)
			{
				Matrix VPW = computeMVPW(view);
				Vec3d windows = pos * VPW;
				
				double windowX = ea.getX();
				double windowY = ea.getY();
				
				if ((windowX - err < windows.x()) && (windows.x() < windowX + err) 
					&& (windowY - err < windows.y()) && (windows.y() < windowY + err))
				{
					ref_ptr<Group> root = dynamic_cast<Group*> (view->getSceneData());

					if (!root)
					{
						break;
					}

					ref_ptr<Node> sceneNode = root->getChild(0);

					if (sceneNode)
					{
						//如果在范围内, 切换视图
						Vec3d up = mPara->mUp;
						switchSnapView(pos, mPara->mCenter, up);
					}
				}

			}

			break;
		}
		case(osgGA::GUIEventAdapter::MOVE) :
		{
			auto vecCameraPos = mPara->calAllCameraPoint();

			//在范围内高亮
			for (auto pos : vecCameraPos)
			{
				Matrix VPW = computeMVPW(view);

				Vec3d windows = pos * VPW;

				double windowX = ea.getX();
				double windowY = ea.getY();

				if ((windowX - err < windows.x()) && (windows.x() < windowX + err)
					&& (windowY - err < windows.y()) && (windows.y() < windowY + err))
				{
					ref_ptr<Group> root = dynamic_cast<Group*> (view->getSceneData());

					if (!root)
					{
						break;
					}

					ref_ptr<Group> highLightGroup = root->getChild(4)->asGroup();
					clearGroup(highLightGroup);

					//如果在范围内, 切换视图
					shared_ptr<IDrawer> pDrawer = IDrawerFactory::create();
					ref_ptr<Node> basePt = pDrawer->drawBasePoint(pos, Vec4d(0, 1, 1, 1));

					highLightGroup->addChild(basePt);

					break;
				}

			}

			break;

		}
		case(osgGA::GUIEventAdapter::RELEASE) :
		{

			break;
		}
		default:
			break;
	}
	return false;
}

void CPickHandler::switchSnapView(Vec3d eye, Vec3d center, Vec3d up)
{
	mEmptyCamera->setTransformation(eye, center, up);
}

void CPickHandler::clearGroup(ref_ptr<Group> group)
{
	int numChild = group->getNumChildren();
	group->removeChildren(0, numChild);
}

const osg::Matrix CPickHandler::computeMVPW(osgViewer::View* view) const
{
	osg::Matrix matrix(view->getCamera()->getViewMatrix() * view->getCamera()->getProjectionMatrix());

	if (view->getCamera()->getViewport())
		matrix.postMult(view->getCamera()->getViewport()->computeWindowMatrix());
	else
		OSG_WARN << "osg::Matrix SceneView::computeMVPW() - error no viewport attached to SceneView, coords will be computed incorrectly." << std::endl;

	return matrix;
}