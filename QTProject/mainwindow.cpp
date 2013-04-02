/************************************************************************/
/* �޸�1.CreateActions()�е�view3DSceneAction ��ע�͵��ˣ���Ϊ��ʱ����Ҫ����˵��������ɼ���ֱ����ת����
�޸�2. ��3D������ʾ��ص�	CreateCentralWidget();CreateDockWidget();CreateActions();
       CreateMenu();CreateToolbar();CreateConnections(); �����ŵ������Ӧ�¼��д�����˻���ֶ�̬�Ĳ˵�
*/
/************************************************************************/
#include <string>
#include <QFileDialog>
#include <QMessageBox>
#include <QIcon>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "3DFeatureExtract/FeatureExtract.h"
#include "3DSearch/search.h"
#include "NaiveBayesClassify/NB.h"
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <strstream>
#include <map>
using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
	//�ڹرյ�ʱ���Զ��ͷ��ڴ�
	//setAttribute(Qt::WA_DeleteOnClose);

	// �����趨
	ui->setupUi(this);
	scene=NULL;
	SourceImage = NULL;
	entireState = imageHandle;
	objectList = NULL;
	relationship = NULL;
	this->pSceneMatResult = new SceneMatRes[SCENEFORDISPLAY];

	////���ڱ��浥��ģ�ͼ������
	//selected3DModel = -1;
	//this->pObjectMatchResult = new ObjectMatRes[MODELSEARCHRESULTNUMBER];

	CreateCentralWidget();
	CreateDockWidget();
	CreateActions();
	CreateMenu();
	CreateToolbar();
	CreateConnections();
}

MainWindow::~MainWindow()
{
	if(SourceImage!=NULL)
		delete SourceImage;

	//sceneDisplayWidget->destroy();

	if(pSceneMatResult != NULL)
	{
		delete pSceneMatResult;
		pSceneMatResult = NULL;
	}

    delete ui;
}

void MainWindow::CreateConnections()
{
	//mainwindow������widget����ϵ��������createAction��ͷ�ģ�����toolbar��Menu��connection
	connect(this,SIGNAL(OpenImageFile(QImage *)),pictureDisplayWidget,SLOT(OpenImageFile(QImage *)));
	connect(this,SIGNAL(SendPicDisMesg(QSegPictureDisplay *)),pictureDisplayWidget,SLOT(GetMainwMesg(QSegPictureDisplay *)));
	//connect(this,SIGNAL(SetDisScene(Scene*)),sceneDisplayWidget,SLOT(SetDisScene(Scene*)));
	connect(this,SIGNAL(SaveSegObject(QString ,int)),pictureDisplayWidget,SLOT(SaveSegObject(QString ,int)));
	connect(this,SIGNAL(ClearDrawRect()),segPictureDisplayWidget,SLOT(ClearDrawRect()));
	emit SendPicDisMesg(segPictureDisplayWidget);
	//connect(this,SIGNAL(PickImageObject(int)),pictureDisplayWidget,SLOT(PickImageAction(int)));
	//connect(this,SIGNAL(SetChooseMode()),sceneDisplayWidget,SLOT(ChooseModelAction()));
}

void MainWindow::OpenSceneFile()
{
	if(scene!=NULL)
		delete scene;

	scene=new Scene;
    //QString fileName=QFileDialog::getOpenFileName(this,tr("Open Scene File"),".",tr("obj file(*.obj)"));
    //QString fileName="E:\\QtProjects\\QSceneEdit\\conference_room26\\conference_room26.obj";
    QString fileName="E:\\conference_room26.obj";
	bool flag=scene->readScene(fileName.toStdString().c_str());
    if(!flag)
	{
        QMessageBox::warning(this,tr("ReadScene"),tr("Open Scene File Error"),QMessageBox::Yes);
		return;
	}

	//CreateRelationItem();  //��ʱע�͵�
	emit SetDisScene(scene);
}

bool MainWindow::OpenSceneOfSearch(const char *threeDSceneFilePath)
{   //���·���Ƿ�Ϊ��
	if(threeDSceneFilePath == NULL)
	{
		QMessageBox::warning(this,tr("ReadScene"),tr("Scene File Path Error"),QMessageBox::Yes);
		return false;
	}

	if(scene!=NULL)
	{
		delete scene;
	}
	scene=new Scene;
	
	bool flag=scene->readScene(threeDSceneFilePath);
	if(!flag)
	{
		QMessageBox::warning(this,tr("ReadScene"),tr("Open Scene File Error"),QMessageBox::Yes);
	    return false;
	}

	//CreateRelationItem();

	// ������ʾ3D������signal
	emit SetDisScene(scene);
}


//open the image
void MainWindow::MOpenImageFile()
{
    //
    if(SourceImage!=NULL)
        delete SourceImage;

    SourceImage=new QImage;
    QString fileName=QFileDialog::getOpenFileName(this,tr("Open Picture File"),".",tr("jpg file(*.jpg)"));
    bool flag = SourceImage->load(fileName);
    if(!flag)
	{
        QMessageBox::warning(this,tr("Load Picture"),tr("Load picture Error"),QMessageBox::Yes);
		return ;
	}

	//������ʾ���ָ�ͼƬ����Ϣ
	emit OpenImageFile(SourceImage);
}

////׼���ô洢���зָ�object���ڴ�
//void MainWindow::PrepareRelations()
//{
//	if(twdObjectCount != 0)
//	{
//		relationship = new int[twdObjectCount*twdObjectCount];
//		objectList = new CSegObject*[twdObjectCount];
//	}
//}


//ֱ��Ϊimage�ָ�������object�趨relationship
void MainWindow::SetRelations()
{
	//֤���������̵�������
	if(entireState == imageHandle)
	{
		//����׼���׶�
		setRelationDialog = new QSetRelationDialog;
		setRelationDialog->objectCount = pictureDisplayWidget->objects.size();
		int temp1 = setRelationDialog->objectCount;
		setRelationDialog->objects = new CSegObject*[temp1] ;
		for(int i = 0; i<temp1 ; i++)
		{
			setRelationDialog->objects[i] = pictureDisplayWidget->objects[i];
		}
		
		//���ɱ�������relationship���ڴ��ַ	
		setRelationDialog->relationships = new int*[temp1];
		for(int i = 0 ;i < temp1; i++)
		{
			setRelationDialog->relationships[i] = new int[temp1];
			memset(setRelationDialog->relationships[i],0,temp1*4);
		}

		//ת��״̬
		entireState = setRelationship;

		//Ϊ���ߵ�list��������
		setRelationDialog->createModelTag();

		setRelationDialog->show();
	}

}

//��������image tag���Ҷ���image�����޸���ϣ���click���б���
void  MainWindow::ClickImageSaveButton()
{
	QString imageTag;
	//first,check the tag input
	imageTag = tagEdit->text();
	if(imageTag.isEmpty())
	{
           //QMessageBox("Please input the object tag !");
		   return;
	}
	QString w = weightSpinBox->text();
	QChar *t = w.data();
	char temp1 = t[0].toAscii();
	int wg = temp1 - '0';

	//ת�Ƹ�QPictureDisplay.hȥ����
	emit SaveSegObject(imageTag ,wg);
	emit ClearDrawRect();

	//���QLineEdit �� QSpinBox �е�����
	tagEdit->clear();
	weightSpinBox->setValue(5);
}

void MainWindow::SaveSceneFile()
{

}

void MainWindow::CreateRelationItem()
{
	rootList.clear();
	map<string,vector<int>>::iterator it;
	QString strQ;
	bool flag=false;  // ָʾ��һ�������Ƿ�����Ҫ��
	QFont font;
	font.setBold(true);
	font.setItalic(true);

	for (it=scene->RelationMap.begin();it!=scene->RelationMap.end();it++)
	{
		QTreeWidgetItem *item=new QTreeWidgetItem;
		strQ = QString::fromLocal8Bit(it->first.c_str());
		item->setText(0,strQ);
		item->setFont(0,font);

		if (strQ.startsWith("group") || strQ.startsWith("support") || strQ.startsWith("enclousure"))
			flag=true;

		for(int i=0;i<it->second.size();i++)
		{
			QTreeWidgetItem *child=new QTreeWidgetItem(item);
			int mIndex=it->second[i];
			int occurs = -1;
			map<string, int>::iterator it = scene->ModelMap.find("Wall");
			if( it != scene->ModelMap.end() )
				occurs = it ->second;

			switch(mIndex)
			{
			case -1: 
				strQ=tr("Wall");
				mIndex=occurs;
				break;
			case -2: 
				strQ=tr("onWall");
				mIndex=occurs;
				break;
			case -3: 
				strQ=tr("hangWall");
				mIndex=occurs;
				break;
			default:
				strQ = QString::fromLocal8Bit(scene->sceneModels[mIndex]->name.c_str());
				break;
			}
			QVariant v(mIndex);
			child->setData(0,Qt::UserRole,v);
			child->setText(0,strQ);
			if(i==0 && flag)
			{
				child->setForeground(0,QBrush(QColor(Qt::red)));
				flag=false;
			}
			item->addChild(child);
		}
		rootList.append(item);
	}
	treeWidget->insertTopLevelItems(0,rootList);
	//treeWidget->expandAll(); //ȫ��չ��
}

void MainWindow::CreateActions()
{

    // openSceneAction=new QAction(QIcon(":/image/open.png"),tr("�򿪳���"),this);
    //connect(openSceneAction,SIGNAL(triggered()),this,SLOT(OpenSceneFile()));

    //***********added by liuxiang,for opening picture *************
    openSceneAction=new QAction(QIcon(":/image/open.png"),tr("����Ƭ"),this);
	connect(openSceneAction,SIGNAL(triggered()),this,SLOT(MOpenImageFile()));
	
 /*   saveSceneAction=new QAction(QIcon(":/image/save.png"),tr("���泡��"),this);
    connect(saveSceneAction,SIGNAL(triggered()),this,SLOT(SaveSceneFile()));*/

	//for segment image
	segImageAction = new QAction(QIcon(":/image/segment.png"),tr("�ָ���Ƭ"),this);
	connect(segImageAction,SIGNAL(triggered()),pictureDisplayWidget,SLOT(SegImageAction()));
	
	//pick up the segmented object
	picImageAction = new QAction(QIcon(":/image/pickup.png"),tr("ѡ������"),this);
	connect(picImageAction,SIGNAL(triggered()),pictureDisplayWidget,SLOT(PickImageAction()));

	paintImageAction=new QAction(QIcon(":/image/brush.png"),tr("��ȫ����"),this);
	connect(paintImageAction,SIGNAL(triggered()),segPictureDisplayWidget,SLOT(PaintImageAction()));

	eraseImageAction=new QAction(QIcon(":/image/eraser.png"),tr("��������"),this);
	connect(eraseImageAction,SIGNAL(triggered()),segPictureDisplayWidget,SLOT(EraseImageAction()));

	//for create relationship for objects from image
	createRelationAction = new QAction(QIcon(":/image/relationship.png"),tr("relationships"),this);
	//connect(createRelationAction,SIGNAL(triggered()),pictureDisplayWidget,SLOT(SaveSegObjectList(&twdObjectCount,))); //��һ��ֻ�ܻ�ȡ���ж��ٸ�object
	//connect(createRelationAction,SIGNAL(triggered()),this,SLOT(PrepareRelations()));   //׼���ñ���object���ڴ�ͱ���relationship���ڴ�
	//connect(createRelationAction,SIGNAL(triggered()),setRelationDialog,SLOT(GetSegObjectList(&pictureDisplayWidget->objects))); //����ֱ�Ӹ�ֵ������Ҫ����
	connect(createRelationAction,SIGNAL(triggered()),this,SLOT(SetRelations()));  //��������dialog���¼�

	//for search for 3D scene list 
	searchSceneAction = new QAction(QIcon(":/image/search.png"),tr("����3D����"),this);
	connect(searchSceneAction,SIGNAL(triggered()),this,SLOT(Search3DScenes()));

	//for view the selected 3D scene 
	//view3DSceneAction = new QAction(QIcon(":/image/display.png"),tr("�鿴3D����"),this);
	//connect(createRelationAction,SIGNAL(triggered()),segPictureDisplayWidget,SLOT(EraseImageAction()));

	/*chooseModelAction=new QAction(QIcon(":/image/choose.png"),tr("ѡ��ģ��"),this);
	connect(chooseModelAction,SIGNAL(triggered()),sceneDisplayWidget,SLOT(ChooseModelAction()));
	

	transModelAction=new QAction(QIcon(":/image/obj_trans.png"),tr("ƽ������"),this);
	connect(transModelAction,SIGNAL(triggered()),sceneDisplayWidget,SLOT(TransModelAction()));
	
	rotateModelAction=new QAction(QIcon(":/image/obj_rotate.png"),tr("��ת����"),this);
	connect(rotateModelAction,SIGNAL(triggered()),sceneDisplayWidget,SLOT(RotateModelAction()));

	transSceneAction=new QAction(QIcon(":/image/trans.png"),tr("ƽ�Ƴ���"),this);
	connect(transSceneAction,SIGNAL(triggered()),sceneDisplayWidget,SLOT(ChooseModelAction()));

	rotateSceneAction=new QAction(QIcon(":/image/rotate.png"),tr("��ת����"),this);
	connect(rotateSceneAction,SIGNAL(triggered()),sceneDisplayWidget,SLOT(ChooseModelAction()));*/

}

void MainWindow::CreateMenu()
{
	fileMenu=ui->menuBar->addMenu(tr("�ļ�"));
	fileMenu->addAction(openSceneAction);
	//fileMenu->addAction(saveSceneAction);

	imageEditMenu = ui->menuBar->addMenu(tr("��Ƭ����"));
	imageEditMenu->addAction(segImageAction);
	imageEditMenu->addAction(picImageAction);
	imageEditMenu->addAction(paintImageAction);
	imageEditMenu->addAction(eraseImageAction);

	searchMenu = ui->menuBar->addMenu(tr("��������"));
	searchMenu->addAction(createRelationAction);
	searchMenu->addAction(searchSceneAction);
	//searchMenu->addAction(view3DSceneAction);

	//modelEditMenu=ui->menuBar->addMenu(tr("ģ�ͱ༭"));
	//modelEditMenu->addAction(chooseModelAction);
	//modelEditMenu->addAction(transModelAction);
	//modelEditMenu->addAction(rotateModelAction);

	//sceneEditMenu=ui->menuBar->addMenu(tr("�����༭"));
	//sceneEditMenu->addAction(transSceneAction);
	//sceneEditMenu->addAction(rotateSceneAction);

}

void MainWindow::CreateToolbar()
{
	fileToolBar=addToolBar(tr("�ļ�"));
	fileToolBar->addAction(openSceneAction);
	//fileToolBar->addAction(saveSceneAction);

	segImageToolBar = addToolBar(tr("��Ƭ����"));
	segImageToolBar->addAction(segImageAction);
	segImageToolBar->addAction(picImageAction);
	segImageToolBar->addAction(paintImageAction);
	segImageToolBar->addAction(eraseImageAction);

	searchToolBar = addToolBar(tr("��������"));
	searchToolBar->addAction(createRelationAction);
	searchToolBar->addAction(searchSceneAction);
	//searchToolBar->addAction(view3DSceneAction);

	//editModelToolBar=addToolBar(tr("ģ�ͱ༭"));
	//editModelToolBar->addAction(chooseModelAction);
	//editModelToolBar->addAction(transModelAction);
	//editModelToolBar->addAction(rotateModelAction);

	//editSceneToolBar=addToolBar(tr("�����༭"));
	//editSceneToolBar->addAction(transSceneAction);
	//editSceneToolBar->addAction(rotateSceneAction);
}

void MainWindow::CreateDockWidget()
{
     //tree widget of the 3D scene
	if(entireState == threeDProcess)
	{
		// ���νṹ
		treeWidget=new QTreeWidget;
		treeWidget->setColumnCount(1);
		treeWidget->setHeaderLabel(tr("Scene Relationship"));

		// ͣ����
		rdocWidget=new QDockWidget;
		rdocWidget->setObjectName(tr("Scene Relationship"));
		rdocWidget->setWindowTitle(tr("Scene Relationship"));
		rdocWidget->setWidget(treeWidget);

		//�˴�����Ҫ����ǰ����ӵ�image handle ��toolbox
		addDockWidget(Qt::RightDockWidgetArea,rdocWidget);
	} 
	else if(entireState == imageHandle) //right toolbox of image handle
	{

	}
}

void MainWindow::CreateCentralWidget()
{
	// �����趨&&������ʼ��
	//stackedWidget = new QStackedWidget(this);
	/*
	widgetStack = new QStackedWidget;
	mainSplitter = new QSplitter(Qt::Horizontal);
	rightSplitter = new QSplitter(Qt::Vertical);

   // picturePartWidget = new QWidget;
	//sceneDisplayWidget=new QSceneDisplay(ui->centralWidget);
	//sceneDisplayWidget->hide();
	//��Ƭ��ʾ���ֱ�����ʼ��
	pictureDisplayWidget = new QPictureDisplay;
	segPictureDisplayWidget = new QSegPictureDisplay;
	//pictureDisplayWidget = new QPictureDisplay(picturePartWidget);  //�˴�������ΪpicturePartWidget���Ӵ���
	//segPictureDisplayWidget = new QSegPictureDisplay(picturePartWidget);

	//********************* ȫ���ǵ�һ��ҳ�棨��Ƭ�ָ��ȡ�����棩�Ĳ������*****************************
	tagLayout = new QHBoxLayout;
	tagLabel = new QLabel(tr("Tag of Object:"));
	tagEdit = new QLineEdit;
	tagEdit->setMaxLength(10);
	weightSpinBox = new QSpinBox;
	weightSpinBox->setMaximum(10);
	weightSpinBox->setMinimum(1);
	weightSpinBox->setValue(5);
	weightLabel = new QLabel(tr("Object weight:"));
	saveButton = new QPushButton(tr("Save"));
	connect(saveButton,SIGNAL(clicked()),this,SLOT(ClickImageSaveButton()));

	tagLayout->addWidget(tagLabel);
	tagLayout->addWidget(tagEdit);
	tagLayout->addWidget(weightLabel);
	tagLayout->addWidget(weightSpinBox);
	tagLayout->addWidget(saveButton);
	tagWidget = new QWidget;
	tagWidget->setLayout(tagLayout);

	//rightSplitter = new QSplitter(Qt::Vertical);
	rightSplitter->addWidget(tagWidget);
	rightSplitter->addWidget(segPictureDisplayWidget);
	//rightSplitter->addWidget(relationDisplayWidget);
	//rightSplitter->setStretchFactor(0,2);

	//mainSplitter = new QSplitter(Qt::Horizontal);
	mainSplitter->addWidget(pictureDisplayWidget);
	mainSplitter->addWidget(rightSplitter);
	mainSplitter->setStretchFactor(0,2);
	mainSplitter->setStyleSheet("QSplitter::handle { background-color: gray }"); 
	mainSplitter->setHandleWidth(3);  

	//widgetStack = new QStackedWidget;
	widgetStack->addWidget(mainSplitter);
	widgetStack->setCurrentIndex(0);
	mainLayout = new QGridLayout;
	mainLayout->addWidget(widgetStack,0,0);


	ui->centralWidget->setLayout(mainLayout);

	//Ϊ����relationship׼���������Ƴٵ�trigger�¼���ʱ����ȥ����
	//setRelationDialog = new QSetRelationDialog;

	// ��������νṹ
	//ui->centralWidget->setLayout(gridLayout);
	*/

	//2013.3.4 ���¸�д����ṹ
	entireProjectLayout = new QGridLayout; //ֱ����ui��centralWidget���Ƶ�
	centralStackedWidget = new QStackedWidget(ui->centralWidget);
	imageSegmentationWidget = new QWidget;
	searchListDisplayWidget = new QSearchListDisplay;
	sceneDisplayWidget = new QSceneDisplay;

	centralStackedWidget->addWidget(imageSegmentationWidget);
	centralStackedWidget->addWidget(searchListDisplayWidget);
	centralStackedWidget->addWidget(sceneDisplayWidget);
	
	entireProjectLayout->addWidget(centralStackedWidget);
	ui->centralWidget->setLayout(entireProjectLayout);

	imageMainLayout = new QGridLayout;
	imageMainSplitter = new QSplitter(Qt::Horizontal);
	imageRightSplitter = new QSplitter(Qt::Vertical);
	pictureDisplayWidget = new QPictureDisplay;
	segPictureDisplayWidget = new QSegPictureDisplay;

	//********************* ȫ���ǵ�һ��ҳ�棨��Ƭ�ָ��ȡ�����棩�Ĳ������*****************************
	tagLayout = new QHBoxLayout;
	tagLabel = new QLabel(tr("Tag of Object:"));
	tagEdit = new QLineEdit;
	tagEdit->setMaxLength(10);
	weightSpinBox = new QSpinBox;
	weightSpinBox->setMaximum(10);
	weightSpinBox->setMinimum(1);
	weightSpinBox->setValue(5);
	weightLabel = new QLabel(tr("Object weight:"));
	saveButton = new QPushButton(tr("Save"));
	connect(saveButton,SIGNAL(clicked()),this,SLOT(ClickImageSaveButton()));

	tagLayout->addWidget(tagLabel);
	tagLayout->addWidget(tagEdit);
	tagLayout->addWidget(weightLabel);
	tagLayout->addWidget(weightSpinBox);
	tagLayout->addWidget(saveButton);
	tagWidget = new QWidget;
	tagWidget->setLayout(tagLayout);

	//rightSplitter = new QSplitter(Qt::Vertical);
	imageRightSplitter->addWidget(tagWidget);
	imageRightSplitter->addWidget(segPictureDisplayWidget);
	//rightSplitter->addWidget(relationDisplayWidget);
	//rightSplitter->setStretchFactor(0,2);

	//mainSplitter = new QSplitter(Qt::Horizontal);
	imageMainSplitter->addWidget(pictureDisplayWidget);
	imageMainSplitter->addWidget(imageRightSplitter);
	imageMainSplitter->setStretchFactor(0,2);
	imageMainSplitter->setStyleSheet("QSplitter::handle { background-color: gray }"); 
	imageMainSplitter->setHandleWidth(3);  

	imageMainLayout->addWidget(imageMainSplitter);
	imageSegmentationWidget->setLayout(imageMainLayout);

	centralStackedWidget->setCurrentIndex(0);

}

void MainWindow::Search3DScenes()
{
	QString errMessage;  //�������������Ϣ
	char err[100];
	char* filename = "src_Image";

	//֤����һ���Ѿ������relationship������
	if(entireState == setRelationship)
	{
		//��relationship�е����ݱ������
		this->relationship = setRelationDialog->relationships;
		setRelationDialog->relationships = NULL;
		this->objectList = setRelationDialog->objects;
		setRelationDialog->objects = NULL;
		this->twdObjectCount = setRelationDialog->objectCount;

		//Ϊ�ָ���ÿ����Ƭ��ȡ������
		for (int i = 0; i< twdObjectCount ; i++)
		{
			if(!FeatureExtractForPictureScene(err, *objectList[i],i,filename))
			{
				QMessageBox::warning(this,tr("Feature Extract"),tr("Feature Extract failed:"),QMessageBox::Yes);
				return;
				//MessageBox("Feature Extract of the picture failed��%s",err);
			}
		}
		QMessageBox::information(this,tr("Feature Extract"),tr("Feature Extract finished!"),QMessageBox::Yes);

		//������ת�Ƶ�twdScene֮��
		this->twdScene.modelNum = this->twdObjectCount;
		strcpy(twdScene.pictureFilename,"src_Image");

		for(int i = 0 ; i< twdObjectCount ; i++)
		{
			//��һ������tag
			strcpy(twdScene.tag1[i],pictureDisplayWidget->objects[i]->tag.toStdString().c_str());
			//�ڶ�������importance
			twdScene.importance[i] = (float)pictureDisplayWidget->objectImportance[i];
			////����������relationship---ת���������ˣ���Ϊ�Ǵ�intת����char
			//for (int j = 0; j < twdObjectCount ; j++)
			//{
			//	twdScene.relationship[i][j] = relationship[i][j];
			//}
		}

		//****************�Գ������з���****************
		//1.�ö���������𳡾��ı�ǩ����
		map<string,int> allLabelAndPosition;
		ifstream in;
		int tempInt;
		int position = 1;
		string inLine;
		string label;
		in.open("MLData\\AllLabelAndCount.txt");
		if(!in)
		{
			//message box
		}
		while(! in.eof())
		{
			getline(in,inLine);
			istringstream line(inLine);
			line>>label>>tempInt;
			allLabelAndPosition.insert(make_pair(label,position));
			position++;
			line.clear();
		}
		in.close();

		//2.�ý���������ת����sparse_feat
		sparse_feat testData;
		map<int,int> sceneObjectData;

		for(int i=0; i< this->twdObjectCount; i++)
		{
			string tempLabel(objectList[i]->tag.toLocal8Bit());
			
			//˳�������ݴ浽sceneDisplayWidget֮�У�ģ���Ƽ���ʱ����Ҫʹ��
			if(sceneDisplayWidget->sourceSceneLabels.count(tempLabel))
			{
				sceneDisplayWidget->sourceSceneLabels[tempLabel]++;
			}
			else
			{
				sceneDisplayWidget->sourceSceneLabels.insert(make_pair(tempLabel,1));
			}

			if(allLabelAndPosition.count(tempLabel))//�ñ�֤ԭ�п��������label
			{
				if(sceneObjectData.count(allLabelAndPosition[tempLabel]))
				{
					sceneObjectData[allLabelAndPosition[tempLabel]]++;
				}
				else
				{
					sceneObjectData.insert(make_pair(allLabelAndPosition[tempLabel],1));
				}
			}
			
		}

		map<int,int>::iterator sceneDataIt = sceneObjectData.begin();
		while(sceneDataIt != sceneObjectData.end())
		{
			testData.id_vec.push_back(sceneDataIt->first);
			testData.value_vec.push_back(sceneDataIt->second);
			sceneDataIt++;
		}

		//3.��ͨ���㷨�������������
	
		string modelSavePath = "F:\\NaiveBayesData\\Mytrain.model";
		int event_model = 1;

		NB nb;
		nb.load_model(modelSavePath.c_str());
		int sceneClass = nb.classify_OneScene(testData,event_model);

		//****************����3D����********************
		int resultSum = 1;
		////����
		//int resultSum = Search3DSceneFromBuffer(twdScene,this->relationship,err,pSceneMatResult);
		//int resultSum = Search3DSceneFromBufferWithClassify(sceneClass,twdScene,this->relationship,err,pSceneMatResult);
		//if(!resultSum)
		//{
		//	//QMessageBox::warning(this,tr("Scenes Search"),tr("Search failed!"),QMessageBox::Yes);
		//	//return;
		//}
		//else
		//{
		//	//QMessageBox::information(this,tr("Scenes Search"),tr("Search finished!"),QMessageBox::Yes);
		//}

		//�������崦��״̬�ĸı�
		this->entireState = search3DScene;

		//char tempFilePath[200];
		FILE *fpt;
		fpt = fopen("SearchResult\\NewSceneDisplay.txt", "rb");
		if(fpt == NULL)
		{
			//sprintf(err,"Can't open file : q8_table! \n");
		}
		//���������������
		string temp;
		string temp1;
		int index;
		for(int i = 0; i< resultSum ; i++)
		//for(int i = 0; i< 15 ; i++)
		{
			fgets(pSceneMatResult[i].name, 200, fpt);
			temp.assign(pSceneMatResult[i].name);
			//temp = string(pSceneMatResult[i].name);
			temp1 = temp.substr(0,temp.find_last_of('/'));
			index = temp1.find_last_of('/');
			temp = temp1.substr(index,temp1.size());
			temp = temp1+temp;
			strcpy(pSceneMatResult[i].name,temp.c_str());
		}
		fclose(fpt);

		//Ȼ��Ҫ����ҳ�����ת������3DSceneListҳ��ȥ
		searchListDisplayWidget->resize(ui->centralWidget->width(),ui->centralWidget->height());

		//���ݼ�������
		searchListDisplayWidget->pSceneMatResult = this->pSceneMatResult;
		//��searchListDisplayWidgetȥload image
		searchListDisplayWidget->downlaodSceneImage(ui->centralWidget->width(),ui->centralWidget->height());	
		int tt = centralStackedWidget->indexOf(searchListDisplayWidget);
		centralStackedWidget->setCurrentIndex(tt);
        
	}
}

/******������ĿǰΪֹֻ���������3D�����б����������������ת******/
void MainWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
	//�������õĳ�������Ǵ�0��ʼ��7
	if(this->entireState == search3DScene)
	{
		setMouseTracking(true);
		QPoint buttonDown = event->pos();
		//int x = 
		int temp = buttonDown.rx()/(searchListDisplayWidget->imageWidth);
		int a = buttonDown.rx() > (searchListDisplayWidget->imageWidth)*temp ? temp:temp-1;
		int b = buttonDown.ry() > (searchListDisplayWidget->imageHeight) ? 1:0;
		this->selcted3DScene = b*4+ a;

		//�׶���ת
		this->entireState = threeDProcess;

	    //*********׼��3D������ʾ�ͱ༭��صĹ���**************

		//create action
		saveSceneAction=new QAction(QIcon(":/image/save.png"),tr("���泡��"),this);
		connect(saveSceneAction,SIGNAL(triggered()),this,SLOT(SaveSceneFile()));

		chooseModelAction=new QAction(QIcon(":/image/choose.png"),tr("ѡ��ģ��"),this);
		connect(chooseModelAction,SIGNAL(triggered()),sceneDisplayWidget,SLOT(ChooseModelAction()));

		transModelAction=new QAction(QIcon(":/image/obj_trans.png"),tr("ƽ������"),this);
		connect(transModelAction,SIGNAL(triggered()),sceneDisplayWidget,SLOT(TransModelAction()));

		rotateModelAction=new QAction(QIcon(":/image/obj_rotate.png"),tr("��ת����"),this);
		connect(rotateModelAction,SIGNAL(triggered()),sceneDisplayWidget,SLOT(RotateModelAction()));

		transSceneAction=new QAction(QIcon(":/image/trans.png"),tr("ƽ�Ƴ���"),this);
		connect(transSceneAction,SIGNAL(triggered()),sceneDisplayWidget,SLOT(ChooseModelAction()));

		rotateSceneAction=new QAction(QIcon(":/image/rotate.png"),tr("��ת����"),this);
		connect(rotateSceneAction,SIGNAL(triggered()),sceneDisplayWidget,SLOT(ChooseModelAction()));

		pickupCubeAction = new QAction(QIcon(":/image/contextSearch.png"),tr("������������"),this);
		connect(pickupCubeAction,SIGNAL(triggered()),sceneDisplayWidget,SLOT(pickupCubeAction()));

		searchInseartObjectAction = new QAction(QIcon(":/image/SearchInseatObject.png"),tr("��������"),this);
		connect(searchInseartObjectAction,SIGNAL(triggered()),sceneDisplayWidget,SLOT(searchInseartObject()));

		//add menu
		fileMenu->addAction(saveSceneAction);  //��������ļ��˵���һ�����Ƚ�����һ��

		modelEditMenu=ui->menuBar->addMenu(tr("ģ�ͱ༭"));
		modelEditMenu->addAction(chooseModelAction);
		modelEditMenu->addAction(transModelAction);
		modelEditMenu->addAction(rotateModelAction);

		sceneEditMenu=ui->menuBar->addMenu(tr("�����༭"));
		sceneEditMenu->addAction(transSceneAction);
		sceneEditMenu->addAction(rotateSceneAction);

		//create toolbar
		fileToolBar->addAction(saveSceneAction);

		editModelToolBar=addToolBar(tr("ģ�ͱ༭"));
		editModelToolBar->addAction(pickupCubeAction);
		editModelToolBar->addAction(searchInseartObjectAction);
		editModelToolBar->addAction(chooseModelAction);
		editModelToolBar->addAction(transModelAction);
		editModelToolBar->addAction(rotateModelAction);

		editSceneToolBar=addToolBar(tr("�����༭"));
		editSceneToolBar->addAction(transSceneAction);
		editSceneToolBar->addAction(rotateSceneAction);

		//create connection
		connect(this,SIGNAL(SetDisScene(Scene*)),sceneDisplayWidget,SLOT(SetDisScene(Scene*)));
		connect(this,SIGNAL(SetChooseMode()),sceneDisplayWidget,SLOT(ChooseModelAction()));
		//connect(this,SIGNAL(SetChooseMode()),sceneDisplayWidget,SLOT(ChooseModelAction()));
		
		//1.��ȡ����,���ʧ������message��ʾ
		string temp1 = string(this->pSceneMatResult[this->selcted3DScene].name)+".obj";
		//if(OpenSceneOfSearch(this->pSceneMatResult[this->selcted3DScene].name) )
		if(OpenSceneOfSearch( temp1.c_str()) )
		{
			QMessageBox::warning(this,tr("Scenes Loading"),tr("Scene Laod failed!"),QMessageBox::Yes);
			return;
		}
		OpenSceneOfSearch( temp1.c_str());

		//׼����ʾ������widget
		sceneDisplayWidget->resize(ui->centralWidget->width(),ui->centralWidget->height());
		int tt = centralStackedWidget->indexOf(sceneDisplayWidget);
		centralStackedWidget->setCurrentIndex(tt);

	}	
}

//�÷�����ת�Ƶ�QSceneDisplay��ȥ��
//������Ӧ�˵��ĵ���ģ�ͼ�������
//�ú���������ݲ���ļ�����λ�ã�ͨ�������㷨���ҵ���Ӧ��ģ���б����ģ���б��ݸ�QModelListDialog��������ʾ
//void MainWindow::searchInseartObject()  
//{
//	//if(this->)
//}

//�ú���ͬ��ת�Ƶ�QSceneDisplay��ȥ��
//��������ӦQModelListDialog������źţ�download����ѡ�����壬����ٲ��볡�����ó����ػ�
//void MainWindow::Inseart3DModel(int selectedModel)
//{
//
//}




