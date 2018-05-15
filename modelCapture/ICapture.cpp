#include "stdafx.h"
#include "ICapture.h"
#include "Capture.h"

using namespace std;
using namespace capture;

ICapture::ICapture()
{
}


ICapture::~ICapture()
{
}

shared_ptr<ICapture> ICaptureFactory::create()
{
	shared_ptr<ICapture> temp(new CCapture());
	return temp;
}