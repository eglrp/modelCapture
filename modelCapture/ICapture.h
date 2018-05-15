#pragma once
#include <memory>
#include <string>



namespace capture
{
	/*
	** brief 截图接口
	*/
	class ICapture
	{
	public:
		ICapture();
		virtual ~ICapture();

		/*
		** brief 自动捕捉
		** param sceneFileName 捕捉的场景文件 全路径
		** param outFileName 输出的截图文件 全路径
		** param screenCaptureWidth screenCaptureHeight 截图宽高
		** param eye center up 视点、中心、相机方向
		*/
		virtual void autoCaptureImage(std::string sceneFileName, std::string outFileName, int screenCaptureWidth, int screenCaptureHeight,
			double eyeX, double eyeY, double eyeZ,  
			double centerX, double centerY, double centerZ,
			double upX, double upY, double upZ) = 0;


		virtual void preview(std::string sceneFileName, double radius) = 0;
	};


	/*
	** brief 截图接口工厂
	*/
	class ICaptureFactory
	{
	public:
		/*
		** brief create 生产接口
		*/
		static std::shared_ptr<ICapture> create();
	};

}