#include "QSetRelationDialog.h"
#include <QPixmap>
#include<QMessageBox>
//#include <fstream>
//#include <string>

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
	leftList->setSelectionMode(QListView::MultiSelection);
	rigthList = new QListView;
	rigthList->setSelectionMode(QListView::MultiSelection);
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


/**************************************************************
ÿһ��relation���趨�����м������б���ĳ��relationship��ѡ����
Ϊ�����ģ���ѡobject����ѡrelationship
****************************************************************/
void QSetRelationDialog::SaveRelations(int index)
{
	if(clickState != bothClicked)
	{
		QMessageBox::warning(this,tr("Set Relationships"),tr("Please select the Object on both side!"),QMessageBox::Yes);
		return;
	}
	std::set<int>::iterator leftIt = leftChoiceSet.begin();
	std::set<int>::iterator rightIt = rightChoiceSet.begin();

	//����relationships
	switch(index)
	{
	//Group_Center,1,-1
	case 0:
		if (leftChoiceSet.size()!= 1)
		{
			QMessageBox::warning(this,tr("Set Relationships"),tr("Object  group center wrong!"),QMessageBox::Yes);
			return;
		}
		while(rightIt!= rightChoiceSet.end())
		{
			relationships[*leftIt][*rightIt] = 1;
			relationships[*rightIt][*leftIt] = -1;
			rightIt++;
		}
		//relationships[leftChoice][rightChoice] = 1;
		//relationships[rightChoice][leftChoice] = -1;
		break;
	//Group_Element
	case 1:
		if (rightChoiceSet.size()!= 1)
		{
			QMessageBox::warning(this,tr("Set Relationships"),tr("Object  group center wrong!"),QMessageBox::Yes);
			return;
		}
		while(leftIt!= leftChoiceSet.end())
		{
			relationships[*leftIt][*rightIt] = -1;
			relationships[*rightIt][*leftIt] = 1;
			leftIt++;
		}
		//relationships[leftChoice][rightChoice] = -1;
		//relationships[rightChoice][leftChoice] = 1;
		break;
	//Support, 5, -5
	case 2:
		if ( leftChoiceSet.size()!= 1) //ֻ����һ�Զ಻���Ƕ�Զ�
		{
			QMessageBox::warning(this,tr("Set Relationships"),tr("Support Object wrong!"),QMessageBox::Yes);
			return;
		}
		while(rightIt!= rightChoiceSet.end())
		{
			relationships[*leftIt][*rightIt] = 5;
			relationships[*rightIt][*leftIt] = -5;
			rightIt++;
		}
		//relationships[leftChoice][rightChoice] = 5;
		//relationships[rightChoice][leftChoice] = -5;
		break;
	//Be_Supported
	case 3:
		if (rightChoiceSet.size()!= 1)
		{
			QMessageBox::warning(this,tr("Set Relationships"),tr("Support Object wrong!"),QMessageBox::Yes);
			return;
		}
		while(leftIt!= leftChoiceSet.end())
		{
			relationships[*leftIt][*rightIt] = -5;
			relationships[*rightIt][*leftIt] = 5;
			leftIt++;
		}
		//relationships[leftChoice][rightChoice] = -5;
		//relationships[rightChoice][leftChoice] = 5;
		break;
	//Identical 6,6
	case 4:
		/*
		identical �������Щ���⣬ֻ����tag��ͬ��ʱ��ſ��Զ�Զ࣬�����tag��ͬ��������identical��ϵ������һ��������
		*/
		while(leftIt != leftChoiceSet.end())
		{
			while(rightIt != rightChoiceSet.end())
			{
				if (objects[*leftIt]->tag == objects[*rightIt]->tag)
				{
					relationships[*leftIt][*rightIt] = 6;
					relationships[*rightIt][*leftIt] = 6;
				}
				rightIt++;
			}
			leftIt++;
		}
		//relationships[leftChoice][rightChoice] = 6;
		//relationships[rightChoice][leftChoice] = 6;
		break;
	//Contact  7,7
	case 5:
		if (leftChoiceSet.size()!=1 || rightChoiceSet.size()!=1)
		{
			QMessageBox::warning(this,tr("Set Relationships"),tr("Contact Object wrong!"),QMessageBox::Yes);
			return;
		}
		relationships[*leftIt][*rightIt] = 7;
		relationships[*rightIt][*leftIt] = 7;
		break;
	//Enclosure 8,-8
	case 6:
		//�涨����һ����������������Թ�������һ��
		if ( leftChoiceSet.size()!= 1) //ֻ����һ�Զ಻���Ƕ�Զ�
		{
			QMessageBox::warning(this,tr("Set Relationships"),tr("Enclosure Object wrong!"),QMessageBox::Yes);
			return;
		}
		while(rightIt!= rightChoiceSet.end())
		{
			relationships[*leftIt][*rightIt] = 8;
			relationships[*rightIt][*leftIt] = -8;
			rightIt++;
		}
		//relationships[leftChoice][rightChoice] = 8;
		//relationships[rightChoice][leftChoice] = -8;
		break;
	//Be_Enclosured
	case 7:
		if (rightChoiceSet.size()!= 1)
		{
			QMessageBox::warning(this,tr("Set Relationships"),tr("Enclosure Object wrong!"),QMessageBox::Yes);
			return;
		}
		while(leftIt!= leftChoiceSet.end())
		{
			relationships[*leftIt][*rightIt] = -8;
			relationships[*rightIt][*leftIt] = 8;
			leftIt++;
		}
		//relationships[leftChoice][rightChoice] = -8;
		//relationships[rightChoice][leftChoice] = 8;
		break;
	default:
		break;
	}

	//����rightChoiceSet ��leftChoiceSet��ͬʱ��state�趨��nonSelected
	rightChoiceSet.clear();
	leftChoiceSet.clear();
	
	clickState = nonClicked;

	//��������listview�е�ѡ�б��
	rigthList->clearSelection();
	leftList->clearSelection();

}

//�����û������list��objectѡ�񣬵���һ��ѡ��ʱ��ѡ�����ڶ���ѡ��ͬһ����������ȡ��
void QSetRelationDialog::leftItemCliked(QModelIndex index)
{
	//ÿ��ֻ��¼�������λ�ã�ͬʱ����״̬,ͬʱ��ʾ����ѡobject��image
	leftChoice = index.row();

	//*************������ʽ�޸ĺ���������******************
	if(leftChoiceSet.count(leftChoice)) //����ڴ�֮ǰ�Ѿ�ѡ����ĳ��ģ�ͣ��ٴε����ʾȡ��
	{
		leftChoiceSet.erase(leftChoice);
	}
	else
	{
		leftChoiceSet.insert(leftChoice);
	}

	if(clickState == rightListClicked || clickState == bothClicked)
	{
		if(leftChoiceSet.size())
		{
			clickState = bothClicked;
		}
		else
		{
			clickState == rightListClicked;
		}
		
	}
	else
	{
		if (leftChoiceSet.size())
		{
			clickState = leftListClicked;
		}
		else
		{
			clickState = nonClicked;
		}
		
	}
	//*************������ʽ�޸ĺ���������******************

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

	if(rightChoiceSet.count(rightChoice)) //����ڴ�֮ǰ�Ѿ�ѡ����ĳ��ģ�ͣ��ٴε����ʾȡ��
	{
		rightChoiceSet.erase(rightChoice);
	}
	else
	{
		rightChoiceSet.insert(rightChoice);
	}

	if(clickState == leftListClicked || clickState == bothClicked)
	{
		if (rightChoiceSet.size())
		{
			clickState = bothClicked;
		}
		else
		{
			clickState == leftListClicked;
		}
		
	}
	else
	{
		if (rightChoiceSet.size())
		{
			clickState = rightListClicked;
		}
		else
		{
			clickState == nonClicked;
		}
		
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
	//���Խ��߻ָ���0
	for (int i = 0; i< objectCount ;i++)
	{
		for(int j = 0; j< objectCount ;j++)
		{
			if(i == j)
			relationships[i][j] = 0;
		}
	}

	/*
	//����
	std::ofstream out;
	out.open("SearchResult\\RelationshipTest.txt");
	for (int i = 0; i< objectCount ;i++)
	{
		for(int j = 0; j< objectCount ;j++)
		{
			out<<relationships[i][j]<<" ";
		}
		out<<std::endl;
	}
	
	for (int i = 0; i< objectCount ;i++)
	{
		out<<std::string(objects[i]->tag.toLocal8Bit())<<" ";
	}

	out.close();
	*/

	QMessageBox::information(this,tr("Set Relationships"),tr("Relationships are created!"),QMessageBox::Yes);
	QDialog::accept();
}
