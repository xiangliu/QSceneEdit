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

	// �Զ�������
public:
	QLineEdit *tagEdit ; // ����������Ƭ�ָ���object tag
	QLabel *tagLabel ;   // ����������Ƭ�ָ���object tag
	QSpinBox *weightSpinBox; //�������������Ȩ��
	QLabel *weightLabel ;    // �������������Ȩ�� tag
	QHBoxLayout *tagLayout ;
	QPushButton *saveButton;
	//QWidget *tagWidget;

	// �Զ��巽��
public:

public slots:
	void ClickedSaveButton();

signals:
	void 
	// �¼�����
protected:
	
private:
};

#endif // QTAGDISPLAY_H
