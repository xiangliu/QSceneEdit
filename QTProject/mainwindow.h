#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QStackedWidget>
#include <QMainWindow>
#include <QHBoxLayout>
#include <QTreeWidget>
#include <QDockWidget>
#include <QList>
#include <QTreeWidgetItem>
#include <QFont>
#include <QBrush>
#include <QColor>
#include <QVariant>
#include <QAction>
#include <QMenu>
#include <QToolBar>
#include <QImage>
#include <QLabel>
#include <QSplitter>

#include "QSceneDisplay.h"
#include "Scene.h"
#include "QPictureDisplay.h"
#include "QSegPictureDisplay.h"
#include "QTagDisplay.h"
#include "MathLib.h"
#include "QSetRelationDialog.h"
#include "QSearchListDisplay.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    // 自定义属性
public:

	//标示程序所在的状态，包括照片处理（imageHandle）、照片物体关系生成（setRelationship）
	//检索3D场景（search3DScene）、显示3D场景（display3DScene）、编辑3D场景（edit3DScene）
	enum entireProcessState { imageHandle, setRelationship ,search3DScene , display3DScene,threeDProcess,edit3DScene};
	entireProcessState entireState ;

	//定义relationship的种类： 0--默认无关系，1--水平支持（support），2--包围（enclosured），3--竖直接触（vContact），4--group关系
	//5--水平被支持， 6--被包围


	// 整个工程界面相关
	
	//整个场景的顶层界面
	QGridLayout* entireProjectLayout; //直接由ui的centralWidget控制的
	QWidget* imageSegmentationWidget;
	QSearchListDisplay *searchListDisplayWidget;
	QSceneDisplay* sceneDisplayWidget; 
	QStackedWidget* centralStackedWidget;

	//更3D场景list相关的layout
	QGridLayout* sceneListLayout;

	//与3D场景显示相关变量
	//QSearchListDisplay *searchListDisplayWidget;  //用于显示3D检索结果列表的widget
	QGridLayout *threeDSceneListLayout;    //用于显示3D检索结果列表的layout

	//2D场景现实相关变量
	QPictureDisplay *pictureDisplayWidget; //2D场景显示widget
	QSegPictureDisplay *segPictureDisplayWidget; //分割的2D物体显示widget
	QSetRelationDialog *setRelationDialog;       //设置照片物体relationship的dialog
	
	//照片分割：用来对整个屏幕区域进行布局的
	//QGridLayout *mainLayout;
	QGridLayout *imageMainLayout;
	QSplitter *imageMainSplitter;
	QSplitter *imageRightSplitter;

	//照片分割：用于mainwindow右边布局使用的控件
	QLineEdit *tagEdit ; // 用于输入照片分割后的object tag
	QLabel *tagLabel ;   // 用于输入照片分割后的object tag
	QSpinBox *weightSpinBox; //用于输入物体的权重
	QLabel *weightLabel ;    // 用于输入物体的权重 tag
	QHBoxLayout *tagLayout ;
	QPushButton *saveButton;
	QWidget *tagWidget;

	//QHBoxLayout *mainLayout;
	QGridLayout *gridLayout;
	QGridLayout *rightLayout; //最右侧放置tag标签等东西的layout
	//QDockWidget *rdocWidget;  // right Dock Widget
	//QTreeWidget* relationTreeWidget;

	// 右边的停留空间;****暂时没有使用******
	QDockWidget *rdocWidget;  // right Dock Widget
	QTreeWidget* treeWidget;
	QList<QTreeWidgetItem *> rootList;

	QGridLayout *threeDSceneLayout;


	// 菜单栏
	QMenu *fileMenu;
	QMenu *imageEditMenu;
	QMenu *searchMenu; //用于输入relationship和进行search的menu
	QMenu *modelEditMenu; //应该是叫scene edit，因为是网scene中添加模型
	QMenu *sceneEditMenu;  //暂时没用到
	
	// 工具栏
	QToolBar *fileToolBar;
	QToolBar *segImageToolBar;   //用于对输入照片进行处理
	QToolBar *searchToolBar;     //用于从二维场景检索三维场景
	QToolBar *editModelToolBar;  //用于编辑3D场景中单个模型
	QToolBar *editSceneToolBar;  //用于编辑整个3D场景
	
	// 相关操作
	//场景相关
	QAction *openSceneAction;
	QAction *saveSceneAction;
	QAction *sceneOperationAction;  //通过点击按钮进入场景操作状态

	//action related with edit 3D scene object
	//QAction *pickupCubeAction;
	QAction *recommendAction;      //进行模型推荐
	QAction *exchangeModelAction;  //改变模型的外形
	QAction *removeModelAction;  //将某个模型设置成invisible


	//QAction *chooseModelAction;
	//QAction *transModelAction;
	//QAction *rotateModelAction;
	//QAction *scaleModelAction;

	//QAction *transSceneAction;
	//QAction *rotateSceneAction;
	//QAction *scaleSceneAction;

	//action related with image
	QAction *segImageAction;
	QAction *picImageAction;
	QAction *paintImageAction;
	QAction *eraseImageAction;

	//action related with search
	QAction *createRelationAction;
	QAction *searchSceneAction;
	//QAction *view3DSceneAction;

	//represent the file to be paint
	int selcted3DScene; //用于记录用户在3D scene List中的选择
    Scene *scene;
    QImage *SourceImage;  //represent the loaded image file

	////定义单个模型检索相关的变量
	//int selected3DModel;   //-1代表什么都没有选
	//pObjectMatRes pObjectMatchResult;

	//定义二维的场景相关变量
	TwoDScene  twdScene;          //定义二维场景的数据
	int twdObjectCount;           //定义二维场景的object 个数
	CSegObject** objectList;      //保存所有被分割object后保存的地址
	int **relationship;            //开辟一个一维数组来保存所有的二维object之间的relationship
    pSceneMatRes pSceneMatResult; //用来保存场景检索结果的指针
	
	// 自定义方法
public:


signals:
	void SetChooseMode();
	void SetDisScene(Scene* scene); // 将读入的场景指针传入显示界面
	void OpenImageFile(QImage *SourceImage); //Open image file
	void PickImageObject(int state); //pick object from the segmented image
	void SendPicDisMesg(QSegPictureDisplay *segPictureDisplayWidget); //send QPictureDisplay the poniter of QSegPictureDisplay
	void SaveSegObject(QString tag, int weight); //用于提醒QPictureDisplay.h去保存被分割好的物体
	void ClearDrawRect(); //发送给QSegPictureDisplay,用于让其擦除上一次的boject
public slots:
    void OpenSceneFile();
    void SaveSceneFile();
    void MOpenImageFile();  //********added by liu xiang; for open picture*********
	void ClickImageSaveButton();  //用于应对在保存单幅照片的tag和weight
	void SetRelations();     //根据菜单和图标trigger事件来生成relationship的dialog
	void Search3DScenes();   //用来响应菜单和图标的场景检索功能
	//void searchInseartObject();  //用来响应菜单的单个模型检索功能
	//void Inseart3DModel(int selectedModel);   //用来响应单个模型检索列表中单击后挑选的结果，同时再次触发QSceneDisplay去insert模型并重绘
	
private:
    Ui::MainWindow *ui;
	void CreateDockWidget();
	void CreateRelationItem(); // 添加QTreeWidget的内容
	void CreateActions();
	void CreateMenu();
	void CreateToolbar();
	void CreateCentralWidget();
	void CreateConnections(); //用来将mainwindow和各个widget建立connections
	void mouseDoubleClickEvent ( QMouseEvent *event); //用于捕获在3DsearchList里的双击事件

	bool OpenSceneOfSearch(const char *threeDSceneFilePath); //用于去打开检索得到的3D场景
};

#endif // MAINWINDOW_H
