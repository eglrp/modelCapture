#include "stdafx.h"
#include "IDrawer.h"
#include "Drawer.h"
#include <memory>

using namespace std;
using namespace capture;

IDrawer::IDrawer()
{
}


IDrawer::~IDrawer()
{
}


shared_ptr<IDrawer> IDrawerFactory::create()
{
	shared_ptr<IDrawer> temp(new CDrawer);
	return temp;
}