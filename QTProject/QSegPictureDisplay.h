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
	void paintEvent(QPaintEvent *e);  //�̳��Ի��������ƺ���
	void paintImage(QPoint point, int size);       //���ڽ����ز�ȫpickup ��object
	void eraseImage(QPoint point, int size);      //���ڽ����ز���gray image
	
	public slots:
		void PickupObject(QImage *grayImage);  //���ڽ���QPictureDisplay��pickup signal����ʾpickup object
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
	QRect ImageRect;  //Image�滭������
	//bool isErase;       // ������
	//bool isDraw;        // ��ͼ?
	bool isLButtonDown; // �������Ƿ���
	QImage *grayImg;    //������ʾ��gray Image
		
private:
	//Ui::QPictureDisplay *ui;
};

#endif // QPICTUREDISPLAY_H
