#pragma once
#include <opencv/cv.h>
#include <vector>
#include <osg/Vec3d>

void cvFitPlane(const CvMat* points, double* plane);

void CVPlane(std::vector<osg::Vec3d>& data, double& a, double& b, double& c, double& d);

void CVLine(std::vector<osg::Vec3d>& data, float& nx, float& ny, float& x, float& y);