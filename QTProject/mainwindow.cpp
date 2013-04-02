/************************************************************************/
/* 修改1.CreateActions()中的view3DSceneAction 被注释掉了，因为暂时不需要这个菜单，而是由检索直接跳转进入
修改2. 跟3D场景显示相关的	CreateCentralWidget();CreateDockWidget();CreateActions();
       CreateMenu();CreateToolbar();CreateConnections(); 都最后放到鼠标响应事件中处理，因此会出现动态的菜单
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
	//在关闭的时候自动释放内存
	//setAttribute(Qt::WA_DeleteOnClose);

	// 属性设定
	ui->setupUi(this);
	scene=NULL;
	SourceImage = NULL;
	entireState = imageHandle;
	objectList = NULL;
	relationship = NULL;
	this->pSceneMatResult = new SceneMatRes[SCENEFORDISPLAY];

	////用于保存单个模型检索结果
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
	//mainwindow和其他widget的联系，区别与createAction里头的，那是toolbar和Menu的connection
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

	//CreateRelationItem();  //暂时注释掉
	emit SetDisScene(scene);
}

bool MainWindow::OpenSceneOfSearch(const char *threeDSceneFilePath)
{   //检查路径是否为空
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

	// 发送显示3D场景的signal
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

	//发送显示被分割图片的消息
	emit OpenImageFile(SourceImage);
}

////准备好存储所有分割object的内存
//void MainWindow::PrepareRelations()
//{
//	if(twdObjectCount != 0)
//	{
//		relationship = new int[twdObjectCount*twdObjectCount];
//		objectList = new CSegObject*[twdObjectCount];
//	}
//}


//直接为image分割后的所有object设定relationship
void MainWindow::SetRelations()
{
	//证明整个过程的有序性
	if(entireState == imageHandle)
	{
		//数据准备阶段
		setRelationDialog = new QSetRelationDialog;
		setRelationDialog->objectCount = pictureDisplayWidget->objects.size();
		int temp1 = setRelationDialog->objectCount;
		setRelationDialog->objects = new CSegObject*[temp1] ;
		for(int i = 0; i<temp1 ; i++)
		{
			setRelationDialog->objects[i] = pictureDisplayWidget->objects[i];
		}
		
		//生成保存所有relationship的内存地址	
		setRelationDialog->relationships = new int*[temp1];
		for(int i = 0 ;i < temp1; i++)
		{
			setRelationDialog->relationships[i] = new int[temp1];
			memset(setRelationDialog->relationships[i],0,temp1*4);
		}

		//转换状态
		entireState = setRelationship;

		//为两边的list生成数据
		setRelationDialog->createModelTag();

		setRelationDialog->show();
	}

}

//当输入完image tag，且对于image进行修改完毕，则click进行保存
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

	//转移给QPictureDisplay.h去处理
	emit SaveSegObject(imageTag ,wg);
	emit ClearDrawRect();

	//清空QLineEdit 和 QSpinBox 中的内容
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
	bool flag=false;  // 指示第一个物体是否是重要的
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
	//treeWidget->expandAll(); //全部展开
}

void MainWindow::CreateActions()
{

    // openSceneAction=new QAction(QIcon(":/image/open.png"),tr("打开场景"),this);
    //connect(openSceneAction,SIGNAL(triggered()),this,SLOT(OpenSceneFile()));

    //***********added by liuxiang,for opening picture *************
    openSceneAction=new QAction(QIcon(":/image/open.png"),tr("打开照片"),this);
	connect(openSceneAction,SIGNAL(triggered()),this,SLOT(MOpenImageFile()));
	
 /*   saveSceneAction=new QAction(QIcon(":/image/save.png"),tr("保存场景"),this);
    connect(saveSceneAction,SIGNAL(triggered()),this,SLOT(SaveSceneFile()));*/

	//for segment image
	segImageAction = new QAction(QIcon(":/image/segment.png"),tr("分割照片"),this);
	connect(segImageAction,SIGNAL(triggered()),pictureDisplayWidget,SLOT(SegImageAction()));
	
	//pick up the segmented object
	picImageAction = new QAction(QIcon(":/image/pickup.png"),tr("选择物体"),this);
	connect(picImageAction,SIGNAL(triggered()),pictureDisplayWidget,SLOT(PickImageAction()));

	paintImageAction=new QAction(QIcon(":/image/brush.png"),tr("补全物体"),this);
	connect(paintImageAction,SIGNAL(triggered()),segPictureDisplayWidget,SLOT(PaintImageAction()));

	eraseImageAction=new QAction(QIcon(":/image/eraser.png"),tr("擦除物体"),this);
	connect(eraseImageAction,SIGNAL(triggered()),segPictureDisplayWidget,SLOT(EraseImageAction()));

	//for create relationship for objects from image
	createRelationAction = new QAction(QIcon(":/image/relationship.png"),tr("relationships"),this);
	//connect(createRelationAction,SIGNAL(triggered()),pictureDisplayWidget,SLOT(SaveSegObjectList(&twdObjectCount,))); //这一步只能获取到有多少个object
	//connect(createRelationAction,SIGNAL(triggered()),this,SLOT(PrepareRelations()));   //准备好保存object的内存和保存relationship的内存
	//connect(createRelationAction,SIGNAL(triggered()),setRelationDialog,SLOT(GetSegObjectList(&pictureDisplayWidget->objects))); //可以直接赋值，不需要传递
	connect(createRelationAction,SIGNAL(triggered()),this,SLOT(SetRelations()));  //触发生成dialog的事件

	//for search for 3D scene list 
	searchSceneAction = new QAction(QIcon(":/image/search.png"),tr("搜索3D场景"),this);
	connect(searchSceneAction,SIGNAL(triggered()),this,SLOT(Search3DScenes()));

	//for view the selected 3D scene 
	//view3DSceneAction = new QAction(QIcon(":/image/display.png"),tr("查看3D场景"),this);
	//connect(createRelationAction,SIGNAL(triggered()),segPictureDisplayWidget,SLOT(EraseImageAction()));

	/*chooseModelAction=new QAction(QIcon(":/image/choose.png"),tr("选择模型"),this);
	connect(chooseModelAction,SIGNAL(triggered()),sceneDisplayWidget,SLOT(ChooseModelAction()));
	

	transModelAction=new QAction(QIcon(":/image/obj_trans.png"),tr("平移物体"),this);
	connect(transModelAction,SIGNAL(triggered()),sceneDisplayWidget,SLOT(TransModelAction()));
	
	rotateModelAction=new QAction(QIcon(":/image/obj_rotate.png"),tr("旋转物体"),this);
	connect(rotateModelAction,SIGNAL(triggered()),sceneDisplayWidget,SLOT(RotateModelAction()));

	transSceneAction=new QAction(QIcon(":/image/trans.png"),tr("平移场景"),this);
	connect(transSceneAction,SIGNAL(triggered()),sceneDisplayWidget,SLOT(ChooseModelAction()));

	rotateSceneAction=new QAction(QIcon(":/image/rotate.png"),tr("旋转场景"),this);
	connect(rotateSceneAction,SIGNAL(triggered()),sceneDisplayWidget,SLOT(ChooseModelAction()));*/

}

void MainWindow::CreateMenu()
{
	fileMenu=ui->menuBar->addMenu(tr("文件"));
	fileMenu->addAction(openSceneAction);
	//fileMenu->addAction(saveSceneAction);

	imageEditMenu = ui->menuBar->addMenu(tr("照片处理"));
	imageEditMenu->addAction(segImageAction);
	imageEditMenu->addAction(picImageAction);
	imageEditMenu->addAction(paintImageAction);
	imageEditMenu->addAction(eraseImageAction);

	searchMenu = ui->menuBar->addMenu(tr("场景检索"));
	searchMenu->addAction(createRelationAction);
	searchMenu->addAction(searchSceneAction);
	//searchMenu->addAction(view3DSceneAction);

	//modelEditMenu=ui->menuBar->addMenu(tr("模型编辑"));
	//modelEditMenu->addAction(chooseModelAction);
	//modelEditMenu->addAction(transModelAction);
	//modelEditMenu->addAction(rotateModelAction);

	//sceneEditMenu=ui->menuBar->addMenu(tr("场景编辑"));
	//sceneEditMenu->addAction(transSceneAction);
	//sceneEditMenu->addAction(rotateSceneAction);

}

void MainWindow::CreateToolbar()
{
	fileToolBar=addToolBar(tr("文件"));
	fileToolBar->addAction(openSceneAction);
	//fileToolBar->addAction(saveSceneAction);

	segImageToolBar = addToolBar(tr("照片处理"));
	segImageToolBar->addAction(segImageAction);
	segImageToolBar->addAction(picImageAction);
	segImageToolBar->addAction(paintImageAction);
	segImageToolBar->addAction(eraseImageAction);

	searchToolBar = addToolBar(tr("场景检索"));
	searchToolBar->addAction(createRelationAction);
	searchToolBar->addAction(searchSceneAction);
	//searchToolBar->addAction(view3DSceneAction);

	//editModelToolBar=addToolBar(tr("模型编辑"));
	//editModelToolBar->addAction(chooseModelAction);
	//editModelToolBar->addAction(transModelAction);
	//editModelToolBar->addAction(rotateModelAction);

	//editSceneToolBar=addToolBar(tr("场景编辑"));
	//editSceneToolBar->addAction(transSceneAction);
	//editSceneToolBar->addAction(rotateSceneAction);
}

void MainWindow::CreateDockWidget()
{
     //tree widget of the 3D scene
	if(entireState == threeDProcess)
	{
		// 树形结构
		treeWidget=new QTreeWidget;
		treeWidget->setColumnCount(1);
		treeWidget->setHeaderLabel(tr("Scene Relationship"));

		// 停靠栏
		rdocWidget=new QDockWidget;
		rdocWidget->setObjectName(tr("Scene Relationship"));
		rdocWidget->setWindowTitle(tr("Scene Relationship"));
		rdocWidget->setWidget(treeWidget);

		//此处可能要隐藏前面添加的image handle 的toolbox
		addDockWidget(Qt::RightDockWidgetArea,rdocWidget);
	} 
	else if(entireState == imageHandle) //right toolbox of image handle
	{

	}
}

void MainWindow::CreateCentralWidget()
{
	// 界面设定&&部件初始化
	//stackedWidget = new QStackedWidget(this);
	/*
	widgetStack = new QStackedWidget;
	mainSplitter = new QSplitter(Qt::Horizontal);
	rightSplitter = new QSplitter(Qt::Vertical);

   // picturePartWidget = new QWidget;
	//sceneDisplayWidget=new QSceneDisplay(ui->centralWidget);
	//sceneDisplayWidget->hide();
	//照片显示部分变量初始化
	pictureDisplayWidget = new QPictureDisplay;
	segPictureDisplayWidget = new QSegPictureDisplay;
	//pictureDisplayWidget = new QPictureDisplay(picturePartWidget);  //此处设置其为picturePartWidget的子窗口
	//segPictureDisplayWidget = new QSegPictureDisplay(picturePartWidget);

	//********************* 全部是第一个页面（照片分割、提取、保存）的布局相关*****************************
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

	//为设置relationship准备变量，推迟到trigger事件的时候再去创立
	//setRelationDialog = new QSetRelationDialog;

	// 界面主层次结构
	//ui->centralWidget->setLayout(gridLayout);
	*/

	//2013.3.4 重新改写界面结构
	entireProjectLayout = new QGridLayout; //直接由ui的centralWidget控制的
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

	//********************* 全部是第一个页面（照片分割、提取、保存）的布局相关*****************************
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
	QString errMessage;  //用来保存出错信息
	char err[100];
	char* filename = "src_Image";

	//证明上一步已经完成了relationship的设置
	if(entireState == setRelationship)
	{
		//将relationship中的数据保存过来
		this->relationship = setRelationDialog->relationships;
		setRelationDialog->relationships = NULL;
		this->objectList = setRelationDialog->objects;
		setRelationDialog->objects = NULL;
		this->twdObjectCount = setRelationDialog->objectCount;

		//为分割后的每张照片提取描述子
		for (int i = 0; i< twdObjectCount ; i++)
		{
			if(!FeatureExtractForPictureScene(err, *objectList[i],i,filename))
			{
				QMessageBox::warning(this,tr("Feature Extract"),tr("Feature Extract failed:"),QMessageBox::Yes);
				return;
				//MessageBox("Feature Extract of the picture failed：%s",err);
			}
		}
		QMessageBox::information(this,tr("Feature Extract"),tr("Feature Extract finished!"),QMessageBox::Yes);

		//将数据转移到twdScene之中
		this->twdScene.modelNum = this->twdObjectCount;
		strcpy(twdScene.pictureFilename,"src_Image");

		for(int i = 0 ; i< twdObjectCount ; i++)
		{
			//第一步拷贝tag
			strcpy(twdScene.tag1[i],pictureDisplayWidget->objects[i]->tag.toStdString().c_str());
			//第二步拷贝importance
			twdScene.importance[i] = (float)pictureDisplayWidget->objectImportance[i];
			////第三步拷贝relationship---转换出问题了，因为是从int转成了char
			//for (int j = 0; j < twdObjectCount ; j++)
			//{
			//	twdScene.relationship[i][j] = relationship[i][j];
			//}
		}

		//****************对场景进行分类****************
		//1.得读入所有类别场景的标签集合
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

		//2.得将数据类型转换成sparse_feat
		sparse_feat testData;
		map<int,int> sceneObjectData;

		for(int i=0; i< this->twdObjectCount; i++)
		{
			string tempLabel(objectList[i]->tag.toLocal8Bit());
			
			//顺道将数据存到sceneDisplayWidget之中，模型推荐的时候需要使用
			if(sceneDisplayWidget->sourceSceneLabels.count(tempLabel))
			{
				sceneDisplayWidget->sourceSceneLabels[tempLabel]++;
			}
			else
			{
				sceneDisplayWidget->sourceSceneLabels.insert(make_pair(tempLabel,1));
			}

			if(allLabelAndPosition.count(tempLabel))//得保证原有库里有这个label
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

		//3.得通过算法计算分类结果返回
	
		string modelSavePath = "F:\\NaiveBayesData\\Mytrain.model";
		int event_model = 1;

		NB nb;
		nb.load_model(modelSavePath.c_str());
		int sceneClass = nb.classify_OneScene(testData,event_model);

		//****************检索3D场景********************
		int resultSum = 1;
		////检索
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

		//进行整体处理状态的改变
		this->entireState = search3DScene;

		//char tempFilePath[200];
		FILE *fpt;
		fpt = fopen("SearchResult\\NewSceneDisplay.txt", "rb");
		if(fpt == NULL)
		{
			//sprintf(err,"Can't open file : q8_table! \n");
		}
		//处理检索到的数据
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

		//然后要进行页面的跳转，调到3DSceneList页面去
		searchListDisplayWidget->resize(ui->centralWidget->width(),ui->centralWidget->height());

		//传递检索数据
		searchListDisplayWidget->pSceneMatResult = this->pSceneMatResult;
		//让searchListDisplayWidget去load image
		searchListDisplayWidget->downlaodSceneImage(ui->centralWidget->width(),ui->centralWidget->height());	
		int tt = centralStackedWidget->indexOf(searchListDisplayWidget);
		centralStackedWidget->setCurrentIndex(tt);
        
	}
}

/******本函数目前为止只用来处理从3D场景列表到浏览单个场景的跳转******/
void MainWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
	//这里设置的场景序号是从0开始到7
	if(this->entireState == search3DScene)
	{
		setMouseTracking(true);
		QPoint buttonDown = event->pos();
		//int x = 
		int temp = buttonDown.rx()/(searchListDisplayWidget->imageWidth);
		int a = buttonDown.rx() > (searchListDisplayWidget->imageWidth)*temp ? temp:temp-1;
		int b = buttonDown.ry() > (searchListDisplayWidget->imageHeight) ? 1:0;
		this->selcted3DScene = b*4+ a;

		//阶段跳转
		this->entireState = threeDProcess;

	    //*********准备3D场景显示和编辑相关的工作**************

		//create action
		saveSceneAction=new QAction(QIcon(":/image/save.png"),tr("保存场景"),this);
		connect(saveSceneAction,SIGNAL(triggered()),this,SLOT(SaveSceneFile()));

		chooseModelAction=new QAction(QIcon(":/image/choose.png"),tr("选择模型"),this);
		connect(chooseModelAction,SIGNAL(triggered()),sceneDisplayWidget,SLOT(ChooseModelAction()));

		transModelAction=new QAction(QIcon(":/image/obj_trans.png"),tr("平移物体"),this);
		connect(transModelAction,SIGNAL(triggered()),sceneDisplayWidget,SLOT(TransModelAction()));

		rotateModelAction=new QAction(QIcon(":/image/obj_rotate.png"),tr("旋转物体"),this);
		connect(rotateModelAction,SIGNAL(triggered()),sceneDisplayWidget,SLOT(RotateModelAction()));

		transSceneAction=new QAction(QIcon(":/image/trans.png"),tr("平移场景"),this);
		connect(transSceneAction,SIGNAL(triggered()),sceneDisplayWidget,SLOT(ChooseModelAction()));

		rotateSceneAction=new QAction(QIcon(":/image/rotate.png"),tr("旋转场景"),this);
		connect(rotateSceneAction,SIGNAL(triggered()),sceneDisplayWidget,SLOT(ChooseModelAction()));

		pickupCubeAction = new QAction(QIcon(":/image/contextSearch.png"),tr("添加物体检索框"),this);
		connect(pickupCubeAction,SIGNAL(triggered()),sceneDisplayWidget,SLOT(pickupCubeAction()));

		searchInseartObjectAction = new QAction(QIcon(":/image/SearchInseatObject.png"),tr("查找物体"),this);
		connect(searchInseartObjectAction,SIGNAL(triggered()),sceneDisplayWidget,SLOT(searchInseartObject()));

		//add menu
		fileMenu->addAction(saveSceneAction);  //这个是在文件菜单那一栏，比较特殊一点

		modelEditMenu=ui->menuBar->addMenu(tr("模型编辑"));
		modelEditMenu->addAction(chooseModelAction);
		modelEditMenu->addAction(transModelAction);
		modelEditMenu->addAction(rotateModelAction);

		sceneEditMenu=ui->menuBar->addMenu(tr("场景编辑"));
		sceneEditMenu->addAction(transSceneAction);
		sceneEditMenu->addAction(rotateSceneAction);

		//create toolbar
		fileToolBar->addAction(saveSceneAction);

		editModelToolBar=addToolBar(tr("模型编辑"));
		editModelToolBar->addAction(pickupCubeAction);
		editModelToolBar->addAction(searchInseartObjectAction);
		editModelToolBar->addAction(chooseModelAction);
		editModelToolBar->addAction(transModelAction);
		editModelToolBar->addAction(rotateModelAction);

		editSceneToolBar=addToolBar(tr("场景编辑"));
		editSceneToolBar->addAction(transSceneAction);
		editSceneToolBar->addAction(rotateSceneAction);

		//create connection
		connect(this,SIGNAL(SetDisScene(Scene*)),sceneDisplayWidget,SLOT(SetDisScene(Scene*)));
		connect(this,SIGNAL(SetChooseMode()),sceneDisplayWidget,SLOT(ChooseModelAction()));
		//connect(this,SIGNAL(SetChooseMode()),sceneDisplayWidget,SLOT(ChooseModelAction()));
		
		//1.获取场景,如果失败则用message提示
		string temp1 = string(this->pSceneMatResult[this->selcted3DScene].name)+".obj";
		//if(OpenSceneOfSearch(this->pSceneMatResult[this->selcted3DScene].name) )
		if(OpenSceneOfSearch( temp1.c_str()) )
		{
			QMessageBox::warning(this,tr("Scenes Loading"),tr("Scene Laod failed!"),QMessageBox::Yes);
			return;
		}
		OpenSceneOfSearch( temp1.c_str());

		//准备显示场景的widget
		sceneDisplayWidget->resize(ui->centralWidget->width(),ui->centralWidget->height());
		int tt = centralStackedWidget->indexOf(sceneDisplayWidget);
		centralStackedWidget->setCurrentIndex(tt);

	}	
}

//该方法被转移到QSceneDisplay中去了
//用来响应菜单的单个模型检索功能
//该函数必须根据插入的检索框位置，通过检索算法查找到相应的模型列表，随后将模型列表传递给QModelListDialog，让其显示
//void MainWindow::searchInseartObject()  
//{
//	//if(this->)
//}

//该函数同样转移到QSceneDisplay中去了
//本函数响应QModelListDialog发射的信号，download被挑选的物体，随后再插入场景，让场景重绘
//void MainWindow::Inseart3DModel(int selectedModel)
//{
//
//}




