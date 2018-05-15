#pragma once
#include "ICapture.h"
#include <osg/Vec3d>
#include <osg/Node>

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
		*/
		virtual void preview(std::string sceneFileName, double radius);

	protected:
		osg::Node* drawBaseShpere(const osg::Vec3d &center, double radius);

	};
}

