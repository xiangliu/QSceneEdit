#ifndef QSETRELATIONDIALOG_H
#define QSETRELATIONDIALOG_H

/************************************************************************/
/* ������������object֮���relationship                                                                     */
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

#define SMALLIMAGESIZE 256  //���ڿ���СͼƬ����ʾ��С

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
	//���ڱ����û�ѡ�����ıߵ�tag
	enum ClickState{nonClicked,leftListClicked ,rightListClicked,bothClicked};
	ClickState clickState;
	int leftChoice;  //���ڼ�¼�����ѡ��object��tag
	int rightChoice; //���ڼ�¼�ұ���ѡ��object��tag

	//������ʾ���ݶ�Ӧ�ؼ�
	int objectCount;             //��������
	CSegObject** objects ;     //���б��ָ������ָ��

	//���ڱ������е�relationship
	//relationship�ı�������������ϰ벿��Ϊ��һ���趨�����²���Ϊ�ڶ����趨
	//�����һ�εõ���tag����Ϊi, j; relation = k;
	//��ôrelationship[i*count+j]=k
	//ÿһ��relationship���趨������QComboBox�ĸı���Ϊ�¼�����
	//int *relationships;
	int **relationships; //ֱ�Ӿ����ó�[i][j] = k;

	//���ڿؼ���Ӧ����
	QImage *smallTopImage;
	QImage *smallButtonImage;
	QLabel *topImageLabel;
	QLabel *buttonImageLabel;

	QLabel *leftLable;
	QLabel *rightLable;
	QListView *leftList;      //�������object��ǩ
	QStandardItemModel *leftStandardModle;
	QStandardItemModel *rightStandardModle;
	//QStringList *leftListModel;  //���view���������
	QListView *rigthList;     //�Ҳ�����object��ǩ
	//QStringList *rightListModel;  //�Ҳ�view���������
	QComboBox *relationListBox;  //���ڰ�������relationship������
	QStringList *relationList;  //���ڰ�������relationship�����������
	QPushButton *finishButton;  //���ڵ�������趨������button
	QVBoxLayout *leftLayout ;  //��������layout
	QVBoxLayout *centerLayout ;  //��������layout
	QVBoxLayout *rightLayout ;  //��������layout
	QWidget *rightWidget;
	QWidget *centerWidget;
	QWidget *leftWidget;
	QSplitter *mainSplitter;    //������������ķָ���
	QHBoxLayout *mainLayout ;  //���������layout



signals:
	//void PickupObject(QImage *grayImage); //���signal��QSegPictureDisplay �����ܣ�������ʾpickup��object

	public slots:
	//void GetSegObjectList(vector<CSegObject*> &objects); // ��mainwindow��ȡ��ص�����
	void leftItemCliked(QModelIndex index); //����ߵ�list��ѡ���ʱ����Ҫ�����¼�����Ӧ����ʾobject��Ӧ��image
	void rightItemCliked(QModelIndex index);
	void SaveRelations(int index);  //��������QComboBox�¼����趨����object��relationship
	void accept();  //�����������relationship������
private:
	
};

#endif

