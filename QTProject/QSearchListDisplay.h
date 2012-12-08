#ifndef QSEARCHLISTDISPLAY_H
#define QSEARCHLISTDISPLAY_H

/*
 this file is used to dispay the relationship of the object 
 segmented from the image.
*/

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>

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
	

protected:


	//attribute:
public:


	public slots:
		
private:
	//Ui::QPictureDisplay *ui;
};

#endif // QPICTUREDISPLAY_H
