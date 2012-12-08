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

#include "QSceneDisplay.h"
#include "Scene.h"
#include "QPictureDisplay.h"
#include "QSegPictureDisplay.h"

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

	QGridLayout *gridLayout;
	QGridLayout *rightLayout; //最右侧放置tag标签等东西的layout
	//QDockWidget *rdocWidget;  // right Dock Widget
	//QTreeWidget* relationTreeWidget;

	QLineEdit *tagEdit ; // 用于输入照片分割后的object tag
	QLabel *tagLabel ;   // 用于输入照片分割后的object tag

	// 右边的停留空间
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

public slots:
    void OpenSceneFile();
    void SaveSceneFile();
    void MOpenImageFile();  //********added by liu xiang; for open picture*********

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
