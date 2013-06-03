#ifndef QMODELLISTDIALOG_H
#define QMODELLISTDIALOG_H

/************************************************************************/
/* ����չʾ3D�����е�ģ���Ƽ��б�                                                                   */
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

#define SMALL_MODEL_IMAGESIZE 50  //���ڿ���ģ��СͼƬ����ʾ��С
#define SEARCHRESULTSIZE 18   //���ڿ���������ʾ����ĸ���

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
	void DownloadModelImage(int windWidth, int windHeight); //����ȥ��������õ�������ģ�͵�Image

protected:
	//void mouseMoveEvent(QMouseEvent *event);
	//void wheelEvent(QWheelEvent *event);
	void mouseDoubleClickEvent(QMouseEvent *event); //���ڼ�¼�û���ѡ�����ĸ�ģ��

	//attribute:
public:
	//���ڱ����û�ѡ�����ıߵ�tag
	//enum ClickState{nonClicked,leftListClicked ,rightListClicked,bothClicked};
	//ClickState clickState;

	//������ʾ���м�������ģ�͵���Ƭ�ı���
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

	//���ڱ������н��������image
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

	//�����û���ѡ��ģ��,-1��ʾδ��ѡ�κ�ģ��
	int selectedModle;
	//pObjectMatRes pObjectMatchResult; //���ڱ��������������ģ�͵�filepath���˴�����Ϊָ�룬�ڴ���QSceneDisplay���٣���mainwindow�ͷ�
	vector<string> objectFilepath; //���ڱ����Ƽ������·��
	int modelImageWidth;
	int modelImageHeight;  //���ڱ�ʾ������ʾ�����ģ��ͼƬ�Ĵ�С

signals:
	void Inseart3DModel(int selectedModel); //���signal��QSceneDisplay �����ܣ����ڽ����û�˫��ѡ��Ľ��

public slots:
	void accept();  //��doubleclick�е��ã��������Model��ѡ��Ȼ��ر�dialog

private:

};

#endif

