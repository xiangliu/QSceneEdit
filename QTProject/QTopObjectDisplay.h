#ifndef QTOPOBJECTDISPLAY_H
#define QTOPOBJECTDISPLAY_H

namespace Ui {
	class QTopObjectDisplay;
}

class QTopObjectDisplay : public QWidget
{
	Q_OBJECT

		//method
public:
	explicit QTopObjectDisplay(QWidget *parent = 0);
	~QTopObjectDisplay();
	void paintEvent(QPaintEvent *e);  //继承自基类的虚绘制函数

protected:


	//attribute:
public:
	QImage *topImage;   //用于显示的Image

signals:
	//void PickupObject(QImage *grayImage); //这个signal由QSegPictureDisplay 来接受，用来显示pickup的object

	public slots:
		//void GetSegObjectList(vector<CSegObject*> &objects); // 从mainwindow获取相关的数据

private:

};

#endif

