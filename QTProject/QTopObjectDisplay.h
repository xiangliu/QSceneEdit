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
	void paintEvent(QPaintEvent *e);  //�̳��Ի��������ƺ���

protected:


	//attribute:
public:
	QImage *topImage;   //������ʾ��Image

signals:
	//void PickupObject(QImage *grayImage); //���signal��QSegPictureDisplay �����ܣ�������ʾpickup��object

	public slots:
		//void GetSegObjectList(vector<CSegObject*> &objects); // ��mainwindow��ȡ��ص�����

private:

};

#endif

