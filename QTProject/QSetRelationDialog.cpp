#include "QSetRelationDialog.h"
#include <QPixmap>
#include<QMessageBox>

QSetRelationDialog::QSetRelationDialog(QWidget *parent) :
QDialog(parent)
{
	//�ڹرյ�ʱ���Զ��ͷ��ڴ�
	//setAttribute(Qt::WA_DeleteOnClose);
	objects = NULL;
	clickState = nonClicked;
	smallTopImage = new QImage(SMALLIMAGESIZE,SMALLIMAGESIZE,QImage::Format_RGB666);
	smallButtonImage =new QImage(SMALLIMAGESIZE,SMALLIMAGESIZE,QImage::Format_RGB666);

	leftList = new QListView;
	rigthList = new QListView;
	leftStandardModle = new QStandardItemModel;
	rightStandardModle = new QStandardItemModel;
	//leftListModel = new QStringList;
	//rightListModel = new QStringList;
	leftList->setModel(leftStandardModle); // ��������
	rigthList->setModel(rightStandardModle);
	leftLable = new QLabel(tr("Objects:"));
	rightLable = new QLabel(tr("Objects:"));

	leftLayout = new QVBoxLayout;
	leftLayout->addWidget(leftLable);
	leftLayout->addWidget(leftList);
	rightLayout = new QVBoxLayout;
	rightLayout->addWidget(rightLable);
	rightLayout->addWidget(rigthList);

	relationList = new QStringList;
	relationList->append(tr("Group_Center"));
	relationList->append(tr("Group_Element"));
	relationList->append(tr("Support"));
	relationList->append(tr("Be_Supported"));
	relationList->append(tr("Identical"));
	relationList->append(tr("Contact"));
	relationList->append(tr("Enclosure"));
	relationList->append(tr("Be_Enclosured"));
	relationListBox = new QComboBox;
	relationListBox->addItems(*relationList);
	connect(relationListBox,SIGNAL(activated(int)),this,SLOT(SaveRelations(int)));

	topImageLabel = new QLabel;
	buttonImageLabel = new QLabel;

	finishButton = new QPushButton(tr("Finish"));
	connect(finishButton,SIGNAL(clicked()),this,SLOT(accept()));
	
	centerLayout = new QVBoxLayout;
	centerLayout->addWidget(topImageLabel);
	centerLayout->addWidget(relationListBox);
	centerLayout->addWidget(buttonImageLabel);
	centerLayout->addWidget(finishButton);

	leftWidget = new QWidget;
	centerWidget = new QWidget;
	rightWidget = new QWidget;

	leftWidget->setLayout(leftLayout);
	centerWidget->setLayout(centerLayout);
	rightWidget->setLayout(rightLayout);

	mainSplitter =  new QSplitter(Qt::Horizontal);
	mainSplitter->addWidget(leftWidget);
	mainSplitter->addWidget(centerWidget);
	mainSplitter->addWidget(rightWidget);

    mainLayout = new QHBoxLayout;
	mainLayout->addWidget(mainSplitter);
	this->setLayout(mainLayout);

	topImageLabel->setPixmap(QPixmap::fromImage(*smallTopImage));
	topImageLabel->show();
	buttonImageLabel->setPixmap(QPixmap::fromImage(*smallButtonImage));
	buttonImageLabel->show();
}


QSetRelationDialog::~QSetRelationDialog()
{
	if(objects != NULL)
	{
		//delete [] objects;
	}

}

//Ϊ����tag list ׼������
void QSetRelationDialog::createModelTag()
{
	//Ϊtag list ׼������
	for(int i = 0; i< objectCount ;i++)
	{
		QStandardItem *Item1 = new QStandardItem(objects[i]->tag);
		QStandardItem *Item2 = new QStandardItem(objects[i]->tag);
		leftStandardModle->appendRow(Item1);
		rightStandardModle->appendRow(Item2);
	}

	connect(leftList,SIGNAL(clicked(QModelIndex)),this,SLOT(leftItemCliked(QModelIndex)));
	connect(rigthList,SIGNAL(clicked(QModelIndex)),this,SLOT(rightItemCliked(QModelIndex)));
}

void QSetRelationDialog::SaveRelations(int index)
{
	if(clickState != bothClicked)
	{
		QMessageBox::warning(this,tr("Set Relationships"),tr("Please select the Object on both side!"),QMessageBox::Yes);
		return;
	}

	//����relationships
	switch(index)
	{
	//Group_Center,1,-1
	case 0:
		relationships[leftChoice][rightChoice] = 1;
		relationships[rightChoice][leftChoice] = -1;
		break;
	//Group_Element
	case 1:
		relationships[leftChoice][rightChoice] = -1;
		relationships[rightChoice][leftChoice] = 1;
		break;
	//Support, 5, -5
	case 2:
		relationships[leftChoice][rightChoice] = 5;
		relationships[rightChoice][leftChoice] = -5;
		break;
	//Be_Supported
	case 3:
		relationships[leftChoice][rightChoice] = -5;
		relationships[rightChoice][leftChoice] = 5;
		break;
	//Identical 6,6
	case 4:
		relationships[leftChoice][rightChoice] = 6;
		relationships[rightChoice][leftChoice] = 6;
		break;
	//Contact  7,7
	case 5:
		relationships[leftChoice][rightChoice] = 7;
		relationships[rightChoice][leftChoice] = 7;
		break;
	//Enclosure 8,-8
	case 6:
		relationships[leftChoice][rightChoice] = 8;
		relationships[rightChoice][leftChoice] = -8;
		break;
	//Be_Enclosured
	case 7:
		relationships[leftChoice][rightChoice] = -8;
		relationships[rightChoice][leftChoice] = 8;
		break;
	default:
		break;
	}

}

//�����û������list��objectѡ��
void QSetRelationDialog::leftItemCliked(QModelIndex index)
{
	//ÿ��ֻ��¼�������λ�ã�ͬʱ����״̬,ͬʱ��ʾ����ѡobject��image
	leftChoice = index.row();
	if(clickState == rightListClicked || clickState == bothClicked)
	{
		clickState = bothClicked;
	}
	else
	{
		clickState = leftListClicked;
	}

	//��ʾ��Ƭ
	if(smallTopImage != NULL)
	{
		delete smallTopImage;
	}

	smallTopImage = IplImage2QImage(objects[leftChoice]->grayimg.GetImage());
	QImage temp = smallTopImage->scaled(SMALLIMAGESIZE,SMALLIMAGESIZE);
	topImageLabel->setPixmap(QPixmap::fromImage(temp));
	topImageLabel->show();
}


//�����û����ұ�list��objectѡ��
void QSetRelationDialog::rightItemCliked(QModelIndex index)
{
	//ÿ��ֻ��¼�������λ�ã�ͬʱ����״̬,ͬʱ��ʾ����ѡobject��image
	rightChoice = index.row();
	if(clickState == leftListClicked || clickState == bothClicked)
	{
		clickState = bothClicked;
	}
	else
	{
		clickState = rightListClicked;
	}

	//��ʾ��Ƭ
	if(smallButtonImage != NULL)
	{
		delete smallButtonImage;
	}

	smallButtonImage = IplImage2QImage(objects[rightChoice]->grayimg.GetImage());
	QImage temp = smallButtonImage->scaled(SMALLIMAGESIZE,SMALLIMAGESIZE);
	buttonImageLabel->setPixmap(QPixmap::fromImage(temp));
	buttonImageLabel->show();
}

//������������relationship������
void QSetRelationDialog::accept()
{
	QMessageBox::information(this,tr("Set Relationships"),tr("Relationships are created!"),QMessageBox::Yes);
	QDialog::accept();
}
