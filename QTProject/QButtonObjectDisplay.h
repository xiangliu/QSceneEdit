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
	void paintEvent(QPaintEvent *e);  //�̳��Ի��������ƺ���

protected:


	//attribute:
public:
	QImage *buttonImage;   //������ʾ��Image

signals:
	//void PickupObject(QImage *grayImage); //���signal��QSegPictureDisplay �����ܣ�������ʾpickup��object

	public slots:
		//void GetSegObjectList(vector<CSegObject*> &objects); // ��mainwindow��ȡ��ص�����
		
private:

};

#endif

