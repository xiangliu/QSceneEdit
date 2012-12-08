#include <QFileDialog>
#include <QMessageBox>
#include <QIcon>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
	// 属性设定
	ui->setupUi(this);
	scene=NULL;
	SourceImage = NULL;
	hState = imageProcess;

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
	if(hState == threeDProcess)
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
	else if(hState == imageProcess) //right toolbox of image handle
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

	// 界面内容层次结构
	gridLayout=new QGridLayout;
	gridLayout->addWidget(pictureDisplayWidget,0,0);
	gridLayout->addWidget(segPictureDisplayWidget,0,1);
	//hlayout->addWidget(sceneDisplayWidget);
	
	// 界面主层次结构
	ui->centralWidget->setLayout(gridLayout);

}


