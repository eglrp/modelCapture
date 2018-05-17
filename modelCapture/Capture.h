#pragma once
#include "ICapture.h"
#include <osg/Vec3d>
#include <osg/Node>
#include <mutex>

namespace capture
{
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
		virtual void autoCaptureImage(std::string sceneFileName, std::string outFileName, int screenCaptureWidth, int screenCaptureHeight,
			double eyeX, double eyeY, double eyeZ,
			double centerX, double centerY, double centerZ,
			double upX, double upY, double upZ);

		/*
		** brief Ԥ��
		** param sceneFileName ��׽�ĳ����ļ� ȫ·��
		** param radius �뾶����
		** param interval ���
		*/
		virtual void preview(std::string sceneFileName, double radius, int interval);

		/*
		** brief ����ִ��Ԥ���ĺ���
		** param 
		*/
		virtual void previewImplement(std::string sceneFileName, double radius, int interval);

	protected:
		/*
		** brief �������������ϵ������
		** param center ��������
		** param radius �뾶����
		*/
		std::vector<osg::Vec3d> calAllCameraPoint(int interval, double radius, osg::Vec3d center);

		/*
		** brief ���ƻ�������
		** param center ��������
		** param radius �뾶����
		*/
		osg::Node* drawBaseShpere(const osg::Vec3d &center, double radius);

		/*
		** brief �������λ��
		** param interval ���
		** param radius �뾶
		** param center ������
		*/
		osg::Node* drawCameraPosition(int interval, double radius, osg::Vec3d center);

		/*
		** brief �������λ��
		** param interval ���
		*/
		osg::Geode* drawBasePoint(osg::Vec3d pt, osg::Vec4d clr);

		/*
		** brief ��������ʽ
		** param size ���С
		*/
		osg::StateSet* makePtState(int size);

	protected:
		std::vector<osg::Vec3d> mVecCameraPos;


	};
}

