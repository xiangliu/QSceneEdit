#include "QModelListDialog.h"

QModelListDialog::QModelListDialog(QWidget *parent) :
QDialog(parent)
{
	//pObjectMatchResult = NULL;
	selectedModle = -1;

	imageLable1 = new QLabel;
	imageLable2 = new QLabel;
	imageLable3 = new QLabel;
	imageLable4 = new QLabel;
	imageLable5 = new QLabel;
	imageLable6 = new QLabel;
	imageLable7 = new QLabel;
	imageLable8 = new QLabel;
	imageLable9 = new QLabel;
	imageLable10 = new QLabel;
	imageLable11 = new QLabel;
	imageLable12 = new QLabel;
	imageLable13 = new QLabel;
	imageLable14 = new QLabel;
	imageLable15 = new QLabel;
	imageLable16 = new QLabel;
	imageLable17 = new QLabel;
	imageLable18 = new QLabel;

	modelListMainLayout = new QGridLayout;
	modelListMainLayout->addWidget(imageLable1,0,0);
	modelListMainLayout->addWidget(imageLable2,0,1);
	modelListMainLayout->addWidget(imageLable3,0,2);
	modelListMainLayout->addWidget(imageLable4,0,3);
	modelListMainLayout->addWidget(imageLable5,0,4);
	modelListMainLayout->addWidget(imageLable6,0,5);
	modelListMainLayout->addWidget(imageLable7,1,0);
	modelListMainLayout->addWidget(imageLable8,1,1);
	modelListMainLayout->addWidget(imageLable9,1,2);
	modelListMainLayout->addWidget(imageLable10,1,3);
	modelListMainLayout->addWidget(imageLable11,1,4);
	modelListMainLayout->addWidget(imageLable12,1,5);
	modelListMainLayout->addWidget(imageLable13,2,0);
	modelListMainLayout->addWidget(imageLable14,2,1);
	modelListMainLayout->addWidget(imageLable15,2,2);
	modelListMainLayout->addWidget(imageLable16,2,3);
	modelListMainLayout->addWidget(imageLable17,2,4);
	modelListMainLayout->addWidget(imageLable18,2,5);

	this->setLayout(modelListMainLayout);
}


QModelListDialog::~QModelListDialog()
{

}

//用于载入检索结果列表
void QModelListDialog::DownloadModelImage(int windWidth, int windHeight)
{
	if(objectFilepath.size() > 0)
	{
		//将所有的image都设定成固定大小
		this->modelImageWidth = (int)(windWidth/6);
		this->modelImageHeight = (int)(windHeight/3);

		//string t1 = string(pObjectMatchResult[0].name) + ".jpg";
		//string t2 = string(pObjectMatchResult[1].name) + ".jpg";
		//string t3 = string(pObjectMatchResult[2].name) + ".jpg";
		//string t4 = string(pObjectMatchResult[3].name) + ".jpg";
		//string t5 = string(pObjectMatchResult[4].name) + ".jpg";
		//string t6 = string(pObjectMatchResult[5].name) + ".jpg";
		//string t7 = string(pObjectMatchResult[6].name) + ".jpg";
		//string t8 = string(pObjectMatchResult[7].name) + ".jpg";
		//string t9 = string(pObjectMatchResult[8].name) + ".jpg";
		//string t10 = string(pObjectMatchResult[9].name) + ".jpg";
		//string t11 = string(pObjectMatchResult[10].name) + ".jpg";
		//string t12 = string(pObjectMatchResult[11].name) + ".jpg";
		//string t13 = string(pObjectMatchResult[12].name) + ".jpg";
		//string t14 = string(pObjectMatchResult[13].name) + ".jpg";
		//string t15 = string(pObjectMatchResult[14].name) + ".jpg";
		//string t16 = string(pObjectMatchResult[15].name) + ".jpg";
		//string t17 = string(pObjectMatchResult[16].name) + ".jpg";
		//string t18 = string(pObjectMatchResult[17].name) + ".jpg";

		string t1 = objectFilepath[0] + ".jpg";
		string t2 = objectFilepath[1] + ".jpg";
		string t3 = objectFilepath[2] + ".jpg";
		string t4 = objectFilepath[3] + ".jpg";
		string t5 = objectFilepath[4] + ".jpg";
		string t6 = objectFilepath[5] + ".jpg";
		string t7 = objectFilepath[6] + ".jpg";
		string t8 = objectFilepath[7] + ".jpg";
		string t9 = objectFilepath[8] + ".jpg";
		string t10 = objectFilepath[9] + ".jpg";
		string t11 = objectFilepath[10] + ".jpg";
		string t12 = objectFilepath[11] + ".jpg";
		string t13 = objectFilepath[12] + ".jpg";
		string t14 = objectFilepath[13] + ".jpg";
		string t15 = objectFilepath[14] + ".jpg";
		string t16 = objectFilepath[15] + ".jpg";
		string t17 = objectFilepath[16] + ".jpg";
		string t18 = objectFilepath[17] + ".jpg";

		//为8个label download image(未检查image load 是否成功)
		int i ;
		if( !image1.load(t1.c_str()))
		{
            i = 0;
		}
		image2.load(t2.c_str());
		image3.load(t3.c_str());
		image4.load(t4.c_str());
		image5.load(t5.c_str());
		image6.load(t6.c_str());
		image7.load(t7.c_str());
		image8.load(t8.c_str());
		image9.load(t9.c_str());
		image10.load(t10.c_str());
		image11.load(t11.c_str());
		image12.load(t12.c_str());
		image13.load(t13.c_str());
		image14.load(t14.c_str());
		image15.load(t15.c_str());
		image16.load(t16.c_str());
		image17.load(t17.c_str());
		image18.load(t18.c_str());

		QImage temp1 = image1.scaled(modelImageWidth,modelImageHeight);
		QImage temp2 = image2.scaled(modelImageWidth,modelImageHeight);
		QImage temp3 = image3.scaled(modelImageWidth,modelImageHeight);
		QImage temp4 = image4.scaled(modelImageWidth,modelImageHeight);
		QImage temp5 = image5.scaled(modelImageWidth,modelImageHeight);
		QImage temp6 = image6.scaled(modelImageWidth,modelImageHeight);
		QImage temp7 = image7.scaled(modelImageWidth,modelImageHeight);
		QImage temp8 = image8.scaled(modelImageWidth,modelImageHeight);
		QImage temp9 = image9.scaled(modelImageWidth,modelImageHeight);
		QImage temp10 = image10.scaled(modelImageWidth,modelImageHeight);
		QImage temp11 = image11.scaled(modelImageWidth,modelImageHeight);
		QImage temp12 = image12.scaled(modelImageWidth,modelImageHeight);
		QImage temp13 = image13.scaled(modelImageWidth,modelImageHeight);
		QImage temp14 = image14.scaled(modelImageWidth,modelImageHeight);
		QImage temp15 = image15.scaled(modelImageWidth,modelImageHeight);
		QImage temp16 = image16.scaled(modelImageWidth,modelImageHeight);
		QImage temp17 = image17.scaled(modelImageWidth,modelImageHeight);
		QImage temp18 = image18.scaled(modelImageWidth,modelImageHeight);

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
		imageLable9->setPixmap(QPixmap::fromImage(temp9));
		imageLable9->show();
		imageLable10->setPixmap(QPixmap::fromImage(temp10));
		imageLable10->show();
		imageLable11->setPixmap(QPixmap::fromImage(temp11));
		imageLable11->show();
		imageLable12->setPixmap(QPixmap::fromImage(temp12));
		imageLable12->show();
		imageLable13->setPixmap(QPixmap::fromImage(temp13));
		imageLable13->show();
		imageLable14->setPixmap(QPixmap::fromImage(temp14));
		imageLable14->show();
		imageLable15->setPixmap(QPixmap::fromImage(temp15));
		imageLable15->show();
		imageLable16->setPixmap(QPixmap::fromImage(temp16));
		imageLable16->show();
		imageLable17->setPixmap(QPixmap::fromImage(temp17));
		imageLable17->show();
		imageLable18->setPixmap(QPixmap::fromImage(temp18));
		imageLable18->show();
	
	}
}

//用于记录用户挑选的是哪个模型
void QModelListDialog::mouseDoubleClickEvent(QMouseEvent *event) 
{
	setMouseTracking(true);
	QPoint buttonDown = event->pos();

	int temp = buttonDown.rx()/(this->modelImageWidth);
	int a = buttonDown.rx() > (this->modelImageWidth)*temp ? temp:temp-1;
	 temp = buttonDown.ry()/(this->modelImageHeight);
	int b = buttonDown.ry() > (this->modelImageHeight)*temp ? temp:temp-1;
	this->selectedModle = b*6+ a;

	//在记录用户选择之后，调用accept函数，发射关于选择的signal，同时关闭dialog
	//Inseart3DModel(selectedModle);
	this->accept();
}

//被自己的双击函数调用
void QModelListDialog::accept()
{
	//往QSceneDisplay发送信号，再关闭对话框
	emit Inseart3DModel(selectedModle);
	QDialog::accept();
}


