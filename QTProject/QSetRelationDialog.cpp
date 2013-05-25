#include "QSetRelationDialog.h"
#include <QPixmap>
#include<QMessageBox>
//#include <fstream>
//#include <string>

QSetRelationDialog::QSetRelationDialog(QWidget *parent) :
QDialog(parent)
{
	//在关闭的时候自动释放内存
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
	leftList->setModel(leftStandardModle); // 关联数据
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

//为两边tag list 准备数据
void QSetRelationDialog::createModelTag()
{
	//为tag list 准备数据
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
每一次relation的设定是以中间下拉列表中某个relationship的选择作
为结束的；先选object，再选relationship
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

	//设置relationships
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
		if ( leftChoiceSet.size()!= 1) //只能是一对多不能是多对多
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
		identical 的情况有些特殊，只有在tag相同的时候才可以多对多，如果有tag相同而不存在identical关系，则不能一次性设置
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
		//规定可以一个包含多个，不可以过个包含一个
		if ( leftChoiceSet.size()!= 1) //只能是一对多不能是多对多
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

	//清理rightChoiceSet 和leftChoiceSet，同时将state设定成nonSelected
	rightChoiceSet.clear();
	leftChoiceSet.clear();
	
	clickState = nonClicked;

	//清理左右listview中的选中标记
	rigthList->clearSelection();
	leftList->clearSelection();

}

//保存用户的左边list中object选择，当第一次选择时是选定，第二次选择同一个物体则是取消
void QSetRelationDialog::leftItemCliked(QModelIndex index)
{
	//每次只记录被点击的位置，同时更改状态,同时显示被挑选object的image
	leftChoice = index.row();

	//*************交互方式修改后新增代码******************
	if(leftChoiceSet.count(leftChoice)) //如果在此之前已经选定了某个模型，再次点击表示取消
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
	//*************交互方式修改后新增代码******************

	//显示照片
	if(smallTopImage != NULL)
	{
		delete smallTopImage;
	}

	smallTopImage = IplImage2QImage(objects[leftChoice]->grayimg.GetImage());
	QImage temp = smallTopImage->scaled(SMALLIMAGESIZE,SMALLIMAGESIZE);
	topImageLabel->setPixmap(QPixmap::fromImage(temp));
	topImageLabel->show();
}


//保存用户的右边list中object选择
void QSetRelationDialog::rightItemCliked(QModelIndex index)
{
	//每次只记录被点击的位置，同时更改状态,同时显示被挑选object的image
	rightChoice = index.row();

	if(rightChoiceSet.count(rightChoice)) //如果在此之前已经选定了某个模型，再次点击表示取消
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

	//显示照片
	if(smallButtonImage != NULL)
	{
		delete smallButtonImage;
	}

	smallButtonImage = IplImage2QImage(objects[rightChoice]->grayimg.GetImage());
	QImage temp = smallButtonImage->scaled(SMALLIMAGESIZE,SMALLIMAGESIZE);
	buttonImageLabel->setPixmap(QPixmap::fromImage(temp));
	buttonImageLabel->show();
}

//用来结束整个relationship的设置
void QSetRelationDialog::accept()
{
	//将对角线恢复成0
	for (int i = 0; i< objectCount ;i++)
	{
		for(int j = 0; j< objectCount ;j++)
		{
			if(i == j)
			relationships[i][j] = 0;
		}
	}

	/*
	//测试
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
