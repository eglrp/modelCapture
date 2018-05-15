#pragma once
#include <memory>
#include <string>



namespace capture
{
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
		virtual void autoCaptureImage(std::string sceneFileName, std::string outFileName, int screenCaptureWidth, int screenCaptureHeight,
			double eyeX, double eyeY, double eyeZ,  
			double centerX, double centerY, double centerZ,
			double upX, double upY, double upZ) = 0;


		virtual void preview(std::string sceneFileName, double radius) = 0;
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