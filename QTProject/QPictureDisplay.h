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

	void GenImage();    //��ComputeSegImg ���ã��ڽ���Ƭ�ָ�֮���������ɷָ�Ľ�����������ָ�Ŀ����ɫ�ʵ�
	void GenGrayImage(); // �������ɺ�pickup object���ڵĺڰ�ͼ��
	void UpdateSelected(int x,int y);  // ����������ĳ�ε��֮����Щ����ѡ��
	void UpdateRes();     //�������²�����ͼƬupimg
	void GenObj();      //����ԭʼͼƬ�б�ѡ�е�ĳ������
	void ResetSelectedImage();   //��ѡ��ĳ�����岢����֮�󣬽�����һ��pickup֮ǰ��Ҫ����ǰһ�ε�object
	void paintEvent(QPaintEvent *e);  //�̳��Ի��������ƺ���
	void GetSurroudRect(QPoint &A, QPoint &B); //��ȡ��ά�İ�Χ��

protected:
	void mouseMoveEvent(QMouseEvent *event);
	void wheelEvent(QWheelEvent *event);
	void mousePressEvent(QMouseEvent *event);

//attribute:
public:
	//����objectRePicked����ڶ���pickup object
	enum handleImageState {emputy , loaded ,segmented , objcetPicked,objectRePicked};
	handleImageState hState ;
	//int HandleImage ;    // 0 represents not laoded yet;1--loaded;2--segmented; 3--picked object
	QImage *SrcImage;    // represent the loaded image;
	IplImage *sImage;    // represent the loaded image;
	int ImageHeight ;    // represent the loaded image's height;
	int ImageLength;     // represent the loaded image's length;

	// �ָ��������
    //int isSegmented;    // is the image been segmented, 0 represents not segmented yet
	int **labels;       // ��Ӧͼ��ÿ�����ص�label
	int regionCount;    // region�ĸ���,������ȷ��region����������256�����������GenImage()�����ִ���
	//int* colors;        // ��region��Ӧ��color --�޸ĳɾֲ�������
	int* Iselected;     // ��Щregion��ѡ����

	CvvImage orgimg;  // ԭʼ����ͼƬ
	CvvImage segimg;  // �ָ��ͼƬ
	CvvImage resimg;  // ���ϵ�ͼ��
	CvvImage upimg;   // ����¼�����µ�ͼƬ,�ʼ��segimgһ��
	CvvImage obj;     //ԭʼͼƬ�б�ѡ�е�ĳ������
	CvvImage grayimg; // pickup�������ɶ�Ӧ�ĻҶ�ͼ
	vector<CSegObject*> objects; // �ָ��������
	vector<int> objectImportance;  //�ָ���������Ҫ�̶�,�Ͳ�ȥ�޸�CSegObject�Ľṹ�ˣ����ⶨ��
	
	//������ر���
	QPoint buttonDown; // ��갴��ʱ����Ļ���λ��
	//bool ClearPickupObject;  //���µĵ������ʱ���Ƿ����ԭ����ʵ��object
	QRect ImageRect;  //��ʾ����image���ڵ�����

	//widget֮���connection
	QSegPictureDisplay *segPictureDisplayWidget;

signals:
	void PickupObject(QImage *grayImage); //���signal��QSegPictureDisplay �����ܣ�������ʾpickup��object

public slots:
	void SegImageAction();        //used to segment the Image
	void PickImageAction();         //used to pickup the segmented object
	void OpenImageFile(QImage *SourceImage);  //��Ӧ��mainwindow��������Ƭ��action
	void GetMainwMesg(QSegPictureDisplay *segPicDispWidget); //��mainwindow���QSegPictureDisplay��ָ��
	void SaveSegObject(QString tag, int weight); //���ڱ���ָ�õ�object
	//void SaveSegObjectList(int &objectNum); //������mainwindow�������б����object
private:
    //Ui::QPictureDisplay *ui;
};

#endif // QPICTUREDISPLAY_H
