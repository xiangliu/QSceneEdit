#ifndef QMODELLISTDIALOG_H
#define QMODELLISTDIALOG_H

/************************************************************************/
/* 用于展示3D场景中的模型推荐列表                                                                   */
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
#include <QGridLayout>

#include "MathLib.h"
#include "Global.h"
#include "ImageSeg/SegObject.h"
#include "ImageSeg/CvvImage.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/imgproc_c.h"

#define SMALL_MODEL_IMAGESIZE 50  //用于控制模型小图片的显示大小
#define SEARCHRESULTSIZE 18   //用于控制最终显示结果的个数

namespace Ui {
	class QModelListDialog;
}

class QModelListDialog : public QDialog
{
	Q_OBJECT

		//method
public:
	explicit QModelListDialog(QWidget *parent = 0);
	~QModelListDialog();
	void DownloadModelImage(int windWidth, int windHeight); //用于去载入检索得到的所有模型的Image

protected:
	//void mouseMoveEvent(QMouseEvent *event);
	//void wheelEvent(QWheelEvent *event);
	void mouseDoubleClickEvent(QMouseEvent *event); //用于记录用户挑选的是哪个模型

	//attribute:
public:
	//用于保存用户选择了哪边的tag
	//enum ClickState{nonClicked,leftListClicked ,rightListClicked,bothClicked};
	//ClickState clickState;

	//用于显示所有检索到的模型的照片的变量
	QLabel* imageLable1;
	QLabel* imageLable2;
	QLabel* imageLable3;
	QLabel* imageLable4;
	QLabel* imageLable5;
	QLabel* imageLable6;
	QLabel* imageLable7;
	QLabel* imageLable8;
	QLabel* imageLable9;
	QLabel* imageLable10;
	QLabel* imageLable11;
	QLabel* imageLable12;
	QLabel* imageLable13;
	QLabel* imageLable14;
	QLabel* imageLable15;
	QLabel* imageLable16;
	QLabel* imageLable17;
	QLabel* imageLable18;

	//用于保存所有将被载入的image
	QImage image1;
	QImage image2;
	QImage image3;
	QImage image4;
	QImage image5;
	QImage image6;
	QImage image7;
	QImage image8;
	QImage image9;
	QImage image10;
	QImage image11;
	QImage image12;
	QImage image13;
	QImage image14;
	QImage image15;
	QImage image16;
	QImage image17;
	QImage image18;

	QGridLayout* modelListMainLayout;

	//保存用户挑选的模型,-1表示未挑选任何模型
	int selectedModle;
	//pObjectMatRes pObjectMatchResult; //用于保存检索到的所有模型的filepath，此处仅仅为指针，内存在QSceneDisplay开辟，由mainwindow释放
	vector<string> objectFilepath; //用于保存推荐物体的路径
	int modelImageWidth;
	int modelImageHeight;  //用于表示最终显示结果中模型图片的大小

signals:
	void Inseart3DModel(int selectedModel); //这个signal由QSceneDisplay 来接受，用于接受用户双击选择的结果

public slots:
	void accept();  //在doubleclick中调用，用来完成Model的选择，然后关闭dialog

private:

};

#endif

