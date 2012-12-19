#include <QFileDialog>
#include <QMessageBox>
#include <QIcon>
#include "mainwindow.h"
#include "ui_mainwindow.h"

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

	////照片显示部分变量初始化
	//pictureDisplayWidget = new QPictureDisplay;
	//segPictureDisplayWidget = new QSegPictureDisplay;

	//// 界面内容层次结构
	//gLayout=new QGridLayout;
    delete ui;
}

void MainWindow::CreateConnections()
{
	//mainwindow和其他widget的联系，区别与createAction里头的，那是toolbar和Menu的connection
	connect(this,SIGNAL(OpenImageFile(QImage *)),pictureDisplayWidget,SLOT(OpenImageFile(QImage *)));
	connect(this,SIGNAL(SendPicDisMesg(QSegPictureDisplay *)),pictureDisplayWidget,SLOT(GetMainwMesg(QSegPictureDisplay *)));
	connect(this,SIGNAL(SetDisScene(Scene*)),sceneDisplayWidget,SLOT(SetDisScene(Scene*)));
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
    QString fileName="E:\\QTProject\\conference_room26.obj";
	bool flag=scene->readScene(fileName.toStdString().c_str());
    if(!flag)
        QMessageBox::warning(this,tr("ReadScene"),tr("Open Scene File Error"),QMessageBox::Yes);

	CreateRelationItem();
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
	emit OpenImageFile(SourceImage);
    //emit SetDisScene(scene);
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

void MainWindow::SetRelations()
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
	setRelationDialog->relationships = new int[temp1*temp1];
	for(int i =0; i< temp1; i++)
	{
		for(int j = 0; j < temp1; j++)
		{
			setRelationDialog->relationships[i+temp1 + j] = 0;
		}
	}

	//为两边的list生成数据
	setRelationDialog->createModelTag();

	setRelationDialog->show();

}

//当输入晚image tag，且对于image进行修改完毕，则click进行保存
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
	
    saveSceneAction=new QAction(QIcon(":/image/save.png"),tr("保存场景"),this);
    connect(saveSceneAction,SIGNAL(triggered()),this,SLOT(SaveSceneFile()));

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
	//connect(createRelationAction,SIGNAL(triggered()),segPictureDisplayWidget,SLOT(EraseImageAction()));

	//for view the selected 3D scene 
	view3DSceneAction = new QAction(QIcon(":/image/display.png"),tr("查看3D场景"),this);
	//connect(createRelationAction,SIGNAL(triggered()),segPictureDisplayWidget,SLOT(EraseImageAction()));

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

}

void MainWindow::CreateMenu()
{
	fileMenu=ui->menuBar->addMenu(tr("文件"));
	fileMenu->addAction(openSceneAction);
	fileMenu->addAction(saveSceneAction);

	imageEditMenu = ui->menuBar->addMenu(tr("照片处理"));
	imageEditMenu->addAction(segImageAction);
	imageEditMenu->addAction(picImageAction);
	imageEditMenu->addAction(paintImageAction);
	imageEditMenu->addAction(eraseImageAction);

	searchMenu = ui->menuBar->addMenu(tr("场景检索"));
	searchMenu->addAction(createRelationAction);
	searchMenu->addAction(searchSceneAction);
	searchMenu->addAction(view3DSceneAction);

	modelEditMenu=ui->menuBar->addMenu(tr("模型编辑"));
	modelEditMenu->addAction(chooseModelAction);
	modelEditMenu->addAction(transModelAction);
	modelEditMenu->addAction(rotateModelAction);

	sceneEditMenu=ui->menuBar->addMenu(tr("场景编辑"));
	sceneEditMenu->addAction(transSceneAction);
	sceneEditMenu->addAction(rotateSceneAction);

}

void MainWindow::CreateToolbar()
{
	fileToolBar=addToolBar(tr("文件"));
	fileToolBar->addAction(openSceneAction);
	fileToolBar->addAction(saveSceneAction);

	segImageToolBar = addToolBar(tr("照片处理"));
	segImageToolBar->addAction(segImageAction);
	segImageToolBar->addAction(picImageAction);
	segImageToolBar->addAction(paintImageAction);
	segImageToolBar->addAction(eraseImageAction);

	searchToolBar = addToolBar(tr("场景检索"));
	searchToolBar->addAction(createRelationAction);
	searchToolBar->addAction(searchSceneAction);
	searchToolBar->addAction(view3DSceneAction);

	editModelToolBar=addToolBar(tr("模型编辑"));
	editModelToolBar->addAction(chooseModelAction);
	editModelToolBar->addAction(transModelAction);
	editModelToolBar->addAction(rotateModelAction);

	editSceneToolBar=addToolBar(tr("场景编辑"));
	editSceneToolBar->addAction(transSceneAction);
	editSceneToolBar->addAction(rotateSceneAction);
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
	sceneDisplayWidget=new QSceneDisplay;

	//照片显示部分变量初始化
	pictureDisplayWidget = new QPictureDisplay;
	segPictureDisplayWidget = new QSegPictureDisplay;
	//tagDisplayWidget = new QTagDisplay;
	/*
	// 界面内容层次结构
	gridLayout=new QGridLayout;
	gridLayout->addWidget(pictureDisplayWidget,0,0);
	gridLayout->addWidget(segPictureDisplayWidget,0,1);
	//hlayout->addWidget(sceneDisplayWidget);
	*/
	
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

	rightSplitter = new QSplitter(Qt::Vertical);
	rightSplitter->addWidget(tagWidget);
	rightSplitter->addWidget(segPictureDisplayWidget);
	//rightSplitter->addWidget(relationDisplayWidget);
	//rightSplitter->setStretchFactor(0,2);

	mainSplitter = new QSplitter(Qt::Horizontal);
	mainSplitter->addWidget(pictureDisplayWidget);
	mainSplitter->addWidget(rightSplitter);
	mainSplitter->setStretchFactor(0,2);
	mainSplitter->setStyleSheet("QSplitter::handle { background-color: gray }"); 
	mainSplitter->setHandleWidth(3);  
	mainLayout = new QGridLayout;
	mainLayout->addWidget(mainSplitter,0,0);

	ui->centralWidget->setLayout(mainLayout);

	//为设置relationship准备变量，推迟到trigger事件的时候再去创立
	//setRelationDialog = new QSetRelationDialog;

	// 界面主层次结构
	//ui->centralWidget->setLayout(gridLayout);

}


