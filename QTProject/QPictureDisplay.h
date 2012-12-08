#ifndef QPICTUREDISPLAY_H
#define QPICTUREDISPLAY_H

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QRect>
#include "Global.h"
#include "ImageSeg/MeanShift.h"
#include "ImageSeg/SegObject.h"
#include "QSegPictureDisplay.h"

namespace Ui {
class QPictureDisplay;
}

class QPictureDisplay : public QWidget
{
    Q_OBJECT
  
//method
public:
    explicit QPictureDisplay(QWidget *parent = 0);
    ~QPictureDisplay();

	//IplImage *QImage2IplImage(QImage *src) ;
	//QImage *IplImage2QImage(IplImage *src) ;

	void GenImage();    //被ComputeSegImg 调用，在将照片分割之后，用来生成分割的结果，包括将分割的块添加色彩等
	void GenGrayImage(); // 用来生成和pickup object对于的黑白图像
	void UpdateSelected(int x,int y);  // 用来更新在某次点击之后哪些区域被选中
	void UpdateRes();     //点击后更新产生的图片upimg
	void GenObj();      //生成原始图片中被选中的某个物体
	void ResetSelectedImage();   //在选中某个物体并保存之后，进行下一次pickup之前需要清理前一次的object
	void paintEvent(QPaintEvent *e);  //继承自基类的虚绘制函数

protected:
	void mouseMoveEvent(QMouseEvent *event);
	void wheelEvent(QWheelEvent *event);
	void mousePressEvent(QMouseEvent *event);

//attribute:
public:

	enum handleImageState {emputy , loaded ,segmented , objcetPicked};
	handleImageState hState ;
	//int HandleImage ;    // 0 represents not laoded yet;1--loaded;2--segmented; 3--picked object
	QImage *SrcImage;    // represent the loaded image;
	IplImage *sImage;    // represent the loaded image;
	int ImageHeight ;    // represent the loaded image's height;
	int ImageLength;     // represent the loaded image's length;

	// 分割相关属性
    //int isSegmented;    // is the image been segmented, 0 represents not segmented yet
	int **labels;       // 对应图像每个像素的label
	int regionCount;    // region的个数
	int* colors;        // 各region对应的color
	int* Iselected;     // 哪些region被选中了

	CvvImage orgimg;  // 原始读入图片
	CvvImage segimg;  // 分割后图片
	CvvImage resimg;  // 罩上的图层
	CvvImage upimg;   // 点击事件后更新的图片
	CvvImage obj;     //原始图片中被选中的某个物体
	CvvImage grayimg; // 物体生成对应的灰度图
	vector<CSegObject*> objects; // 分割出的物体

	//交互相关变量
	QPoint buttonDown; // 鼠标按下时，屏幕点的位置
	bool ClearPickupObject;  //当新的点击产生时，是否清楚原来现实的object
	QRect ImageRect;  //表示载入image所在的区域

	//widget之间的connection
	QSegPictureDisplay *segPictureDisplayWidget;

signals:
	void PickupObject(QImage *grayImage); //这个signal由QSegPictureDisplay 来接受，用来显示pickup的object

public slots:
	void SegImageAction();        //used to segment the Image
	void PickImageAction();         //used to pickup the segmented object
	void OpenImageFile(QImage *SourceImage);  //响应在mainwindow中载入照片的action
	void GetMainwMesg(QSegPictureDisplay *segPicDispWidget); //从mainwindow获得QSegPictureDisplay的指针
private:
    //Ui::QPictureDisplay *ui;
};

#endif // QPICTUREDISPLAY_H
