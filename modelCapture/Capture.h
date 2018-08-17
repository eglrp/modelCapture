#pragma once
#include "ICapture.h"
#include <osg/Vec3d>
#include <osg/Node>
#include <mutex>
#include <osg/Group>

namespace capture
{
	class CSnapPara;
	/*
	** brief ������Ⱦ��׽��
	*/
	class CCapture : public ICapture
	{
	public:
		CCapture();
		virtual ~CCapture();

		/*
		** brief �Զ���׽
		** param sceneFileName ��׽�ĳ����ļ� ȫ·��
		** param outFileName ����Ľ�ͼ�ļ� ȫ·��
		** param screenCaptureWidth screenCaptureHeight ��ͼ���
		** param eye center up �ӵ㡢���ġ��������
		*/
		virtual void autoCaptureImage(osg::ref_ptr<osg::Node> loadedModel, std::string outFileName, int screenCaptureWidth, int screenCaptureHeight,
			double eyeX, double eyeY, double eyeZ,
			double centerX, double centerY, double centerZ,
			double upX, double upY, double upZ);

		/*
		** brief �Զ���׽
		** param sceneFileName ��׽�ĳ����ļ� ȫ·��
		** param outFileName ����Ľ�ͼ�ļ� ȫ·��
		** param screenCaptureWidth screenCaptureHeight ��ͼ���
		** param eye center up �ӵ㡢���ġ��������
		*/
		virtual void autoCaptureImage(osg::ref_ptr<osg::Node> loadedModel, std::string outFileName, int screenCaptureWidth, int screenCaptureHeight,
			double eyeX, double eyeY, double eyeZ,
			double centerX, double centerY, double centerZ,
			double upX, double upY, double upZ, double fovy);

		/*
		** brief Ԥ��
		** param sceneFileName ��׽�ĳ����ļ� ȫ·��
		** param para �������
		*/
		virtual void preview(std::shared_ptr<CSnapPara> para);

		/*
		** brief ����ִ��Ԥ���ĺ���
		** param para �������
		*/
		virtual void previewImplement(std::shared_ptr<CSnapPara> para);

		/*
		** brief ����ˢ��
		*/
		virtual void refresh(std::shared_ptr<CSnapPara> para);

		virtual void setPreview();

	protected:
		void drawGraphic(std::shared_ptr<CSnapPara> para, osg::ref_ptr<osg::Group> root);
		/*
		** brief �������λ��
		** param para �������
		*/
		osg::Node* drawCameraPosition(std::shared_ptr<CSnapPara> para);

		/*
		** brief ���ͼ��
		** param root ���ڵ�
		*/
		void clearGraphic(osg::ref_ptr<osg::Group> root);

	protected:
		/*
		** brief ���ڵ�
		*/
		osg::ref_ptr<osg::Group> mRoot;

		/*
		** brief Ԥ��
		*/
		bool bPreview = false;
	};
}

