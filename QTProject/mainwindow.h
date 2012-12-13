#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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
#include "QRelationDisplay.h"
#include "QTagDisplay.h"

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
	// 界面相关
    QSceneDisplay *sceneDisplayWidget;
	QPictureDisplay *pictureDisplayWidget;
	QSegPictureDisplay *segPictureDisplayWidget;
	QRelationDisplay *relationDisplayWidget;
	//QTagDisplay *tagDisplayWidget;

	//用来对整个屏幕区域进行布局的
	QGridLayout *mainLayout;
	QSplitter *mainSplitter;
	QSplitter *rightSplitter;

	//用于mainwindow右边布局使用的控件
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

	// 菜单栏
	QMenu *fileMenu;
	QMenu *modelEditMenu;
	QMenu *sceneEditMenu;
	QMenu *imageEditMenu;

	// 工具栏
	QToolBar *fileToolBar;
	QToolBar *segImageToolBar;
	QToolBar *editModelToolBar;
	QToolBar *editSceneToolBar;
	

	// 相关操作
	QAction *openSceneAction;
	QAction *saveSceneAction;

	QAction *chooseModelAction;
	QAction *transModelAction;
	QAction *rotateModelAction;
	QAction *scaleModelAction;

	QAction *transSceneAction;
	QAction *rotateSceneAction;
	QAction *scaleSceneAction;

	//action related with image
	QAction *segImageAction;
	QAction *picImageAction;
	QAction *paintImageAction;
	QAction *eraseImageAction;

	//represent the file to be paint
    Scene *scene;
    QImage *SourceImage;  //represent the loaded image file
	enum entireProcess {imageProcess ,threeDProcess};
	entireProcess hState;      

    // 自定义方法
public:


signals:
	void SetDisScene(Scene* scene); // 将读入的场景指针传入显示界面
	void OpenImageFile(QImage *SourceImage); //Open image file
	void PickImageObject(int state); //pick object from the segmented image
	void SendPicDisMesg(QSegPictureDisplay *segPictureDisplayWidget); //send QPictureDisplay the poniter of QSegPictureDisplay
	void SaveSegObject(QString tag, int weight); //用于提醒QPictureDisplay.h去保存被分割好的物体

public slots:
    void OpenSceneFile();
    void SaveSceneFile();
    void MOpenImageFile();  //********added by liu xiang; for open picture*********
	void ClickImageSaveButton();

private:
    Ui::MainWindow *ui;
	void CreateDockWidget();
	void CreateRelationItem(); // 添加QTreeWidget的内容
	void CreateActions();
	void CreateMenu();
	void CreateToolbar();
	void CreateCentralWidget();
	void CreateConnections(); //用来将mainwindow和各个widget建立connections
};

#endif // MAINWINDOW_H
