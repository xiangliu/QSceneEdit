#ifndef QSETRELATIONDIALOG_H
#define QSETRELATIONDIALOG_H

/************************************************************************/
/* 用于设置所有object之间的relationship                                                                     */
/************************************************************************/
#include <QImage>
#include <QLabel>
#include <QDialog>
#include <QWidget>
#include <QPainter>
#include <QStringList>
#include <QMouseEvent>
#include <QListView>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QPushButton>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QModelIndex>
#include "Global.h"
#include "ImageSeg/SegObject.h"
#include "ImageSeg/CvvImage.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/imgproc_c.h"

#define SMALLIMAGESIZE 256  //用于控制小图片的显示大小

namespace Ui {
	class QSetRelationDialog;
}

class QSetRelationDialog : public QDialog
{
	Q_OBJECT

		//method
public:
	explicit QSetRelationDialog(QWidget *parent = 0);
	~QSetRelationDialog();
	void createModelTag();

protected:
	//void mouseMoveEvent(QMouseEvent *event);
	//void wheelEvent(QWheelEvent *event);
	//void mousePressEvent(QMouseEvent *event);

	//attribute:
public:
	//用于保存用户选择了哪边的tag
	enum ClickState{nonClicked,leftListClicked ,rightListClicked,bothClicked};
	ClickState clickState;
	int leftChoice;  //用于记录左边挑选的object的tag
	int rightChoice; //用于记录右边挑选的object的tag

	//窗口显示内容对应控件
	int objectCount;             //总物体数
	CSegObject** objects ;     //所有被分割的物体指针

	//用于保存所有的relationship
	//relationship的编号序列是以右上半部分为第一次设定，左下部分为第二次设定
	//比如第一次得到的tag序列为i, j; relation = k;
	//那么relationship[i*count+j]=k
	//每一次relationship的设定都是以QComboBox的改变作为事件驱动
	//int *relationships;
	int **relationships; //直接就设置成[i][j] = k;

	//窗口控件对应变量
	QImage *smallTopImage;
	QImage *smallButtonImage;
	QLabel *topImageLabel;
	QLabel *buttonImageLabel;

	QLabel *leftLable;
	QLabel *rightLable;
	QListView *leftList;      //左侧所有object标签
	QStandardItemModel *leftStandardModle;
	QStandardItemModel *rightStandardModle;
	//QStringList *leftListModel;  //左侧view保存的数据
	QListView *rigthList;     //右侧所有object标签
	//QStringList *rightListModel;  //右侧view保存的数据
	QComboBox *relationListBox;  //用于包含所有relationship的种类
	QStringList *relationList;  //用于包含所有relationship的种类的数据
	QPushButton *finishButton;  //用于点击所有设定结束的button
	QVBoxLayout *leftLayout ;  //用于左侧的layout
	QVBoxLayout *centerLayout ;  //用于左侧的layout
	QVBoxLayout *rightLayout ;  //用于左侧的layout
	QWidget *rightWidget;
	QWidget *centerWidget;
	QWidget *leftWidget;
	QSplitter *mainSplitter;    //用于整个界面的分割器
	QHBoxLayout *mainLayout ;  //用于整体的layout



signals:
	//void PickupObject(QImage *grayImage); //这个signal由QSegPictureDisplay 来接受，用来显示pickup的object

	public slots:
	//void GetSegObjectList(vector<CSegObject*> &objects); // 从mainwindow获取相关的数据
	void leftItemCliked(QModelIndex index); //当左边的list被选择的时候需要触发事件来响应以显示object对应的image
	void rightItemCliked(QModelIndex index);
	void SaveRelations(int index);  //用来接收QComboBox事件，设定两个object的relationship
	void accept();  //用来完成整个relationship的设置
private:
	
};

#endif

