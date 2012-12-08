#ifndef QSEGPICTUREDISPLAY_H
#define QSEGPICTUREDISPLAY_H

#include <QWidget>
#include <QPainter>
#include <QPoint>
#include <QRect>
#include <QMouseEvent>
#include "Global.h"
#include "ImageSeg/MeanShift.h"
#include "ImageSeg/SegObject.h"

namespace Ui {
	class QSegPictureDisplay;
}

class QSegPictureDisplay : public QWidget
{
	Q_OBJECT

//method
public:
	explicit QSegPictureDisplay(QWidget *parent = 0);
	~QSegPictureDisplay();
	void paintEvent(QPaintEvent *e);  //继承自基类的虚绘制函数
	void paintImage(QPoint point, int size);       //用于交互地补全pickup 的object
	void eraseImage(QPoint point, int size);      //用于交互地擦除gray image
	
	public slots:
		void PickupObject(QImage *grayImage);  //用于接受QPictureDisplay的pickup signal，显示pickup object
		void EraseImageAction();
		void PaintImageAction();

private:
	void mouseMoveEvent(QMouseEvent *event);
	void wheelEvent(QWheelEvent *event);
	void mousePressEvent(QMouseEvent *event);


//attribute
public:
	enum ImgHandleState{handleStart, painting , erasing};
	ImgHandleState sState;
	QRect ImageRect;  //Image绘画的区间
	//bool isErase;       // 擦除？
	//bool isDraw;        // 画图?
	bool isLButtonDown; // 鼠标左键是否按下
	QImage *grayImg;    //用于显示的gray Image
		
private:
	//Ui::QPictureDisplay *ui;
};

#endif // QPICTUREDISPLAY_H
