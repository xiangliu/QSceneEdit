#include <QFileDialog>
#include <QMessageBox>
#include <QIcon>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
	// �����趨
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

	////��Ƭ��ʾ���ֱ�����ʼ��
	//pictureDisplayWidget = new QPictureDisplay;
	//segPictureDisplayWidget = new QSegPictureDisplay;

	//// �������ݲ�νṹ
	//gLayout=new QGridLayout;
    delete ui;
}

void MainWindow::CreateConnections()
{
	//mainwindow������widget����ϵ��������createAction��ͷ�ģ�����toolbar��Menu��connection
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
	
    saveSceneAction=new QAction(QIcon(":/image/save.png"),tr("���泡��"),this);
    connect(saveSceneAction,SIGNAL(triggered()),this,SLOT(SaveSceneFile()));

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

}

void MainWindow::CreateMenu()
{
	fileMenu=ui->menuBar->addMenu(tr("�ļ�"));
	fileMenu->addAction(openSceneAction);
	fileMenu->addAction(saveSceneAction);

	imageEditMenu = ui->menuBar->addMenu(tr("��Ƭ����"));
	imageEditMenu->addAction(segImageAction);
	imageEditMenu->addAction(picImageAction);
	imageEditMenu->addAction(paintImageAction);
	imageEditMenu->addAction(eraseImageAction);

	modelEditMenu=ui->menuBar->addMenu(tr("ģ�ͱ༭"));
	modelEditMenu->addAction(chooseModelAction);
	modelEditMenu->addAction(transModelAction);
	modelEditMenu->addAction(rotateModelAction);

	sceneEditMenu=ui->menuBar->addMenu(tr("�����༭"));
	sceneEditMenu->addAction(transSceneAction);
	sceneEditMenu->addAction(rotateSceneAction);

}

void MainWindow::CreateToolbar()
{
	fileToolBar=addToolBar(tr("�ļ�"));
	fileToolBar->addAction(openSceneAction);
	fileToolBar->addAction(saveSceneAction);

	segImageToolBar = addToolBar(tr("��Ƭ����"));
	segImageToolBar->addAction(segImageAction);
	segImageToolBar->addAction(picImageAction);
	segImageToolBar->addAction(paintImageAction);
	segImageToolBar->addAction(eraseImageAction);

	editModelToolBar=addToolBar(tr("ģ�ͱ༭"));
	editModelToolBar->addAction(chooseModelAction);
	editModelToolBar->addAction(transModelAction);
	editModelToolBar->addAction(rotateModelAction);

	editSceneToolBar=addToolBar(tr("�����༭"));
	editSceneToolBar->addAction(transSceneAction);
	editSceneToolBar->addAction(rotateSceneAction);
}

void MainWindow::CreateDockWidget()
{

     //tree widget of the 3D scene
	if(hState == threeDProcess)
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
	else if(hState == imageProcess) //right toolbox of image handle
	{

	}
}

void MainWindow::CreateCentralWidget()
{
	// �����趨&&������ʼ��
	sceneDisplayWidget=new QSceneDisplay;

	//��Ƭ��ʾ���ֱ�����ʼ��
	pictureDisplayWidget = new QPictureDisplay;
	segPictureDisplayWidget = new QSegPictureDisplay;

	// �������ݲ�νṹ
	gridLayout=new QGridLayout;
	gridLayout->addWidget(pictureDisplayWidget,0,0);
	gridLayout->addWidget(segPictureDisplayWidget,0,1);
	//hlayout->addWidget(sceneDisplayWidget);
	
	// ��������νṹ
	ui->centralWidget->setLayout(gridLayout);

}


