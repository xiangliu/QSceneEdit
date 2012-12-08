//#include "stdafx.h"
#include "SegObject.h"


CSegObject::CSegObject(void)
{
}


CSegObject::~CSegObject(void)
{
	grayimg.Destroy();
	objimg.Destroy();
}

void CSegObject::SetGrayImg(CvvImage& img)
{
	grayimg.Destroy();
	grayimg.Create(img.Width(),img.Height(),24);
	cvCopy(img.GetImage(),grayimg.GetImage(),0);
}

void CSegObject::SetObjImg(CvvImage& img)
{
	objimg.Destroy();
	objimg.Create(img.Width(),img.Height(),24);
	cvCopy(img.GetImage(),objimg.GetImage(),0);
}
