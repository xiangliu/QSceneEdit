#ifndef QRELATIONDISPLAY_H
#define QRELATIONDISPLAY_H

/*
 this file is used to dispay the relationship of the object 
 segmented from the image.
*/

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>

namespace Ui {
	class QRelationDisplay;
}

class QRelationDisplay : public QWidget
{
	Q_OBJECT

		//method
public:
	explicit QRelationDisplay(QWidget *parent = 0);
	~QRelationDisplay();
	

protected:


	//attribute:
public:


	public slots:
		
private:
	//Ui::QPictureDisplay *ui;
};

#endif // QPICTUREDISPLAY_H
