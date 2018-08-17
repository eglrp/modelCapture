#pragma once
#include <memory>
#include <string>
#include <osg/Node>


namespace capture
{
	class CSnapPara;
	/*
	** brief ��ͼ�ӿ�
	*/
	class ICapture
	{
	public:
		ICapture();
		virtual ~ICapture();

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
			double upX, double upY, double upZ) = 0;

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
			double upX, double upY, double upZ, double fovy) = 0;

		/*
		** brief Ԥ��
		** param para �������
		*/
		virtual void preview(std::shared_ptr<CSnapPara> para) = 0;

		/*
		** brief ����ˢ��
		*/
		virtual void refresh(std::shared_ptr<CSnapPara> para) = 0;

		/*
		** brief ����Ԥ��
		*/
		virtual void setPreview() = 0;
	};


	/*
	** brief ��ͼ�ӿڹ���
	*/
	class ICaptureFactory
	{
	public:
		/*
		** brief create �����ӿ�
		*/
		static std::shared_ptr<ICapture> create();
	};

}