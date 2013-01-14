#include "QSearchListDisplay.h"
#include <QMessageBox>
#include <string>

QSearchListDisplay::QSearchListDisplay(QWidget *parent) :
QWidget(parent)
{
	//parent->hide();
	this->pSceneMatResult = NULL;
	this->selectedSecne = 0;
	this->searchState = searchUnprepared;

	imageLable1 = new QLabel;
	imageLable2 = new QLabel;
	imageLable3 = new QLabel;
	imageLable4 = new QLabel;
	imageLable5 = new QLabel;
	imageLable6 = new QLabel;
	imageLable7 = new QLabel;
	imageLable8 = new QLabel;

	mainLayout = new QGridLayout;
	mainLayout->addWidget(imageLable1,0,0);
	mainLayout->addWidget(imageLable2,0,1);
	mainLayout->addWidget(imageLable3,0,2);
	mainLayout->addWidget(imageLable4,0,3);
	mainLayout->addWidget(imageLable5,1,0);
	mainLayout->addWidget(imageLable6,1,1);
	mainLayout->addWidget(imageLable7,1,2);
	mainLayout->addWidget(imageLable8,1,3);

	this->setLayout(mainLayout);
	//this->show();
}

QSearchListDisplay::~QSearchListDisplay()
{
}

bool QSearchListDisplay::downlaodSceneImage(int windWidth, int windHeight)
{
	//如果没有检索到结果场景则直接返回
	if(pSceneMatResult == NULL)
	{
		QMessageBox::warning(this,tr("Scenes Search"),tr("There is no search result!"),QMessageBox::Yes);
		return false;
	}

	//将所有的image都设定成固定大小
	this->imageWidth = (int)(windWidth /4);
	this->imageHeight = (int)(windHeight/2);

	string t1 = string(pSceneMatResult[0].name) + ".jpg";
	string t2 = string(pSceneMatResult[1].name) + ".jpg";
	string t3 = string(pSceneMatResult[2].name) + ".jpg";
	string t4 = string(pSceneMatResult[3].name) + ".jpg";
	string t5 = string(pSceneMatResult[4].name) + ".jpg";
	string t6 = string(pSceneMatResult[5].name) + ".jpg";
	string t7 = string(pSceneMatResult[6].name) + ".jpg";
	string t8 = string(pSceneMatResult[7].name) + ".jpg";


	//为8个label download image(未检查image load 是否成功)
	image1.load(t1.c_str());
	image2.load(t2.c_str());
	image3.load(t3.c_str());
	image4.load(t4.c_str());
	image5.load(t5.c_str());
	image6.load(t6.c_str());
	image7.load(t7.c_str());
	image8.load(t8.c_str());

	QImage temp1 = image1.scaled(imageWidth,imageHeight);
	QImage temp2 = image2.scaled(imageWidth,imageHeight);
	QImage temp3 = image3.scaled(imageWidth,imageHeight);
	QImage temp4 = image4.scaled(imageWidth,imageHeight);
	QImage temp5 = image5.scaled(imageWidth,imageHeight);
	QImage temp6 = image6.scaled(imageWidth,imageHeight);
	QImage temp7 = image7.scaled(imageWidth,imageHeight);
	QImage temp8 = image8.scaled(imageWidth,imageHeight);

	//将image加载到label之中
	imageLable1->setPixmap(QPixmap::fromImage(temp1));
	imageLable1->show();
	imageLable2->setPixmap(QPixmap::fromImage(temp2));
	imageLable2->show();
	imageLable3->setPixmap(QPixmap::fromImage(temp3));
	imageLable3->show();
	imageLable4->setPixmap(QPixmap::fromImage(temp4));
	imageLable4->show();
	imageLable5->setPixmap(QPixmap::fromImage(temp5));
	imageLable5->show();
	imageLable6->setPixmap(QPixmap::fromImage(temp6));
	imageLable6->show();
	imageLable7->setPixmap(QPixmap::fromImage(temp7));
	imageLable7->show();
	imageLable8->setPixmap(QPixmap::fromImage(temp8));
	imageLable8->show();

	this->searchState = searchStart;
}


////取消这里的事件响应，放到mainwindow里进行处理，因为如果在这里进行处理还需要将处理结果提交给mainwindow，再次处理
//void QSearchListDisplay::mouseDoubleClickEvent(QMouseEvent *event)
//{
//     //这里设置的场景序号是从0开始到7
//	if(this->searchState == searchStart)
//	{
//		setMouseTracking(true);
//		buttonDown = event->pos();
//		//int x = 
//		int temp = buttonDown.rx()/imageWidth;
//		int a = buttonDown.rx() >imageWidth*temp ? temp:temp-1;
//		int b = buttonDown.ry()>imageHeight ? 1:0;
//		this->selectedSecne = b*4+ a;
//	}	
//}
