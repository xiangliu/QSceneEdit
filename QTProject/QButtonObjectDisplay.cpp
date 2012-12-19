#include "QButtonObjectDisplay.h"

QButtonObjectDisplay::QButtonObjectDisplay(QWidget *parent) :
QWidget(parent)
{
	buttonImage = NULL;
	//this->setFixedSize();
}

QButtonObjectDisplay::~QButtonObjectDisplay()
{

}

void QButtonObjectDisplay::paintEvent(QPaintEvent *e)
{
	//int x = width()/2 - SrcImage->width()/2;
	//int y = height()/2 - SrcImage->height()/2;
	//QPainter painter(this);
	//IplImage *temp1 = upimg.GetImage();
	//QImage *t2 = IplImage2QImage(temp1);
	//painter.drawImage(QPoint(x,y),*t2);
}