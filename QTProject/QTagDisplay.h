#ifndef QTAGDISPLAY_H
#define QTAGDISPLAY_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QSpinBox>
#include <QLabel>


namespace Ui {
	class QTagDisplay;
}

class QTagDisplay : public QWidget
{
	Q_OBJECT

public:
	QTagDisplay(QWidget *parent = 0);
	~QTagDisplay();

	// 自定义属性
public:
	QLineEdit *tagEdit ; // 用于输入照片分割后的object tag
	QLabel *tagLabel ;   // 用于输入照片分割后的object tag
	QSpinBox *weightSpinBox; //用于输入物体的权重
	QLabel *weightLabel ;    // 用于输入物体的权重 tag
	QHBoxLayout *tagLayout ;
	QPushButton *saveButton;
	//QWidget *tagWidget;

	// 自定义方法
public:

public slots:
	void ClickedSaveButton();

signals:
	void 
	// 事件处理
protected:
	
private:
};

#endif // QTAGDISPLAY_H
