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
	bool downlaodSceneImage(int windWidth, int windHeight); // 为每个对应的label加载对应的照片

protected:
	//void mouseDoubleClickEvent ( QMouseEvent *event);

	//attribute:
public:
	//用于显示所有检索到的场景的照片的变量
	QLabel* imageLable1;
	QLabel* imageLable2;
	QLabel* imageLable3;
	QLabel* imageLable4;
	QLabel* imageLable5;
	QLabel* imageLable6;
	QLabel* imageLable7;
	QLabel* imageLable8;

	//用于保存所有将被载入的image
	QImage image1;
	QImage image2;
	QImage image3;
	QImage image4;
	QImage image5;
	QImage image6;
	QImage image7;
	QImage image8;

	//处理阶段变量
	enum SearchState{searchUnprepared,searchStart};
	SearchState searchState;

	//整个widget的框架相关变量
	QGridLayout* mainLayout ; //用于装这些label的layout

	//3D场景相关的数据变量
	pSceneMatRes pSceneMatResult;  //用于保存检索结果的数据

	//记录通过用户交互双击选择的场景序号
	int selectedSecne;  
	QPoint buttonDown; // 鼠标按下时，屏幕点的位置

	//记录窗口的范围，用来确定用户点击的场景序号
	int imageWidth;
	int imageHeight;

	public slots:
		
private:
	 
	
};

#endif // QPICTUREDISPLAY_H
