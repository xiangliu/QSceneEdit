#include "QSegPictureDisplay.h"


QSegPictureDisplay::QSegPictureDisplay(QWidget *parent) :
QWidget(parent)
{
	sState =  handleStart;
	isLButtonDown = false; 

	grayImg = NULL;
}

QSegPictureDisplay::~QSegPictureDisplay()
{

}

void QSegPictureDisplay::paintEvent(QPaintEvent *e)
{
	if(grayImg != NULL)
	{
	   //没有处理 x<0 和 y<0的情况
		int x = width()/2 - grayImg->width()/2;
        int y = height()/2 - grayImg->height()/2;
		QPainter painter(this);
		painter.drawImage(QPoint(x,y),*grayImg);
	}
}

void QSegPictureDisplay::PickupObject(QImage *grayImage)
{
	if(grayImage!=NULL)
	{
		sState = handleStart ;
		grayImg = grayImage;
		int x = width()/2 - grayImg->width()/2;
		int y = height()/2 - grayImg->height()/2;
		ImageRect.setCoords(x,y,x+grayImg->width(),y+grayImg->height());

		this->update();
	}
}

void QSegPictureDisplay::EraseImageAction()
{
	sState = erasing;
}

void QSegPictureDisplay::PaintImageAction()
{
    sState = painting;
}

void QSegPictureDisplay::eraseImage(QPoint point, int size)
{
	int destX=(std::min)((int)(point.x()+size - ImageRect.x()),(int)grayImg->width());
	int destY=(std::min)((int)(point.y()+size-ImageRect.y()),(int)grayImg->height());
	uchar *imageData = grayImg->bits();
	int temp1 = grayImg->bitPlaneCount ()/8;
	int startX = point.x() - ImageRect.x();
	int startY = point.y() - ImageRect.y();
	for (int i= startY; i<destY ;i++)
		for (int j= startX; j<destX ;j++)
		{
			(imageData+i*grayImg->bytesPerLine())[temp1*j+2]= 255;
			(imageData+i*grayImg->bytesPerLine())[temp1*j+1]= 255;
			(imageData+i*grayImg->bytesPerLine())[temp1*j+0]= 255;
		}

	
}

void QSegPictureDisplay::paintImage(QPoint point, int size)
{
	int destX=(std::min)((int)(point.x()+size - ImageRect.x()),(int)grayImg->width());
	int destY=(std::min)((int)(point.y()+size-ImageRect.y()),(int)grayImg->height());
	uchar *imageData = grayImg->bits();
	int temp1 = grayImg->bitPlaneCount ()/8;
	int startX = point.x() - ImageRect.x();
	int startY = point.y() - ImageRect.y();
	for (int i= startY; i<destY ;i++)
		for (int j= startX; j<destX ;j++)
		{
			(imageData+i*grayImg->bytesPerLine())[temp1*j+2]=0;
			(imageData+i*grayImg->bytesPerLine())[temp1*j+1]=0;
			(imageData+i*grayImg->bytesPerLine())[temp1*j+0]=0;
		}
}
void QSegPictureDisplay::mouseMoveEvent(QMouseEvent *event)
{
	QPoint point=event->pos();
	if(grayImg!=NULL && ImageRect.contains(point) && ((event->buttons() & Qt::LeftButton)))
	{
		if ( sState == painting )
		{
			paintImage(point,10);
		}

		if( sState == erasing )
		{
			eraseImage(point,10);
		}
		
		this->update();
	}
}

void QSegPictureDisplay::mousePressEvent(QMouseEvent *event)
{
	//if(grayImg==NULL)
	//	return;
	//setMouseTracking(true);
	//btnDown=event->pos();
}

void QSegPictureDisplay::wheelEvent(QWheelEvent *event)
{

}