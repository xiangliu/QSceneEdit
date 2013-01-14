#ifndef QSEARCHLISTDISPLAY_H
#define QSEARCHLISTDISPLAY_H

/************************************************************************/
/*this file is used to display the relationship of the object segmented from the image.                                                                      */
/************************************************************************/

#include <QPoint>
#include <QImage>
#include <QGridLayout>
#include <QLabel>
#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include "MathLib.h"

namespace Ui {
	class QSearchListDisplay;
}

class QSearchListDisplay : public QWidget
{
	Q_OBJECT

		//method
public:
	explicit QSearchListDisplay(QWidget *parent = 0);
	~QSearchListDisplay();
	bool downlaodSceneImage(int windWidth, int windHeight); // Ϊÿ����Ӧ��label���ض�Ӧ����Ƭ

protected:
	//void mouseDoubleClickEvent ( QMouseEvent *event);

	//attribute:
public:
	//������ʾ���м������ĳ�������Ƭ�ı���
	QLabel* imageLable1;
	QLabel* imageLable2;
	QLabel* imageLable3;
	QLabel* imageLable4;
	QLabel* imageLable5;
	QLabel* imageLable6;
	QLabel* imageLable7;
	QLabel* imageLable8;

	//���ڱ������н��������image
	QImage image1;
	QImage image2;
	QImage image3;
	QImage image4;
	QImage image5;
	QImage image6;
	QImage image7;
	QImage image8;

	//����׶α���
	enum SearchState{searchUnprepared,searchStart};
	SearchState searchState;

	//����widget�Ŀ����ر���
	QGridLayout* mainLayout ; //����װ��Щlabel��layout

	//3D������ص����ݱ���
	pSceneMatRes pSceneMatResult;  //���ڱ���������������

	//��¼ͨ���û�����˫��ѡ��ĳ������
	int selectedSecne;  
	QPoint buttonDown; // ��갴��ʱ����Ļ���λ��

	//��¼���ڵķ�Χ������ȷ���û�����ĳ������
	int imageWidth;
	int imageHeight;

	public slots:
		
private:
	 
	
};

#endif // QPICTUREDISPLAY_H
