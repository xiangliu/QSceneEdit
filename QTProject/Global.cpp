#include "Global.h"

IplImage *QImage2IplImage(QImage *src)
{
	int width = src->width();
	int height = src->height();
	CvSize Size;
	Size.height = height;
	Size.width = width;
	IplImage *res = cvCreateImage(Size, IPL_DEPTH_8U, 3);
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			QRgb rgb = src->pixel(x, y);
			cvSet2D(res, y, x, CV_RGB(qRed(rgb), qGreen(rgb), qBlue(rgb)));
		}
	}
	return res;
}

//change IplImage to QImage，此处未开辟内存
QImage *IplImage2QImage(IplImage *src)
{
	QImage *res;
	cvCvtColor(src,src,CV_BGR2RGB);
	uchar *imgData=(uchar *)src->imageData;
	//Constructs an image with the given width, height and format, that uses an existing memory buffer
	res=new QImage(imgData,src->width,src->height,QImage::Format_RGB888);
	return res;
}

