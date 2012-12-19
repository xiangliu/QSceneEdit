#ifndef QBUTTONOBJECTDISPLAY_H
#define QBUTTONOBJECTDISPLAY_H

namespace Ui {
	class QButtonObjectDisplay;
}

class QButtonObjectDisplay : public QWidget
{
	Q_OBJECT

		//method
public:
	explicit QButtonObjectDisplay(QWidget *parent = 0);
	~QButtonObjectDisplay();
	void paintEvent(QPaintEvent *e);  //继承自基类的虚绘制函数

protected:


	//attribute:
public:
	QImage *buttonImage;   //用于显示的Image

signals:
	//void PickupObject(QImage *grayImage); //这个signal由QSegPictureDisplay 来接受，用来显示pickup的object

	public slots:
		//void GetSegObjectList(vector<CSegObject*> &objects); // 从mainwindow获取相关的数据
		
private:

};

#endif

