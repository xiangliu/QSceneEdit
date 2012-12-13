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

    // �Զ�������
public:
	// �������
    QSceneDisplay *sceneDisplayWidget;
	QPictureDisplay *pictureDisplayWidget;
	QSegPictureDisplay *segPictureDisplayWidget;
	QRelationDisplay *relationDisplayWidget;
	//QTagDisplay *tagDisplayWidget;

	//������������Ļ������в��ֵ�
	QGridLayout *mainLayout;
	QSplitter *mainSplitter;
	QSplitter *rightSplitter;

	//����mainwindow�ұ߲���ʹ�õĿؼ�
	QLineEdit *tagEdit ; // ����������Ƭ�ָ���object tag
	QLabel *tagLabel ;   // ����������Ƭ�ָ���object tag
	QSpinBox *weightSpinBox; //�������������Ȩ��
	QLabel *weightLabel ;    // �������������Ȩ�� tag
	QHBoxLayout *tagLayout ;
	QPushButton *saveButton;
	QWidget *tagWidget;

	//QHBoxLayout *mainLayout;
	QGridLayout *gridLayout;
	QGridLayout *rightLayout; //���Ҳ����tag��ǩ�ȶ�����layout
	//QDockWidget *rdocWidget;  // right Dock Widget
	//QTreeWidget* relationTreeWidget;

	// �ұߵ�ͣ���ռ�;****��ʱû��ʹ��******
	QDockWidget *rdocWidget;  // right Dock Widget
	QTreeWidget* treeWidget;
	QList<QTreeWidgetItem *> rootList;

	// �˵���
	QMenu *fileMenu;
	QMenu *modelEditMenu;
	QMenu *sceneEditMenu;
	QMenu *imageEditMenu;

	// ������
	QToolBar *fileToolBar;
	QToolBar *segImageToolBar;
	QToolBar *editModelToolBar;
	QToolBar *editSceneToolBar;
	

	// ��ز���
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

    // �Զ��巽��
public:


signals:
	void SetDisScene(Scene* scene); // ������ĳ���ָ�봫����ʾ����
	void OpenImageFile(QImage *SourceImage); //Open image file
	void PickImageObject(int state); //pick object from the segmented image
	void SendPicDisMesg(QSegPictureDisplay *segPictureDisplayWidget); //send QPictureDisplay the poniter of QSegPictureDisplay
	void SaveSegObject(QString tag, int weight); //��������QPictureDisplay.hȥ���汻�ָ�õ�����

public slots:
    void OpenSceneFile();
    void SaveSceneFile();
    void MOpenImageFile();  //********added by liu xiang; for open picture*********
	void ClickImageSaveButton();

private:
    Ui::MainWindow *ui;
	void CreateDockWidget();
	void CreateRelationItem(); // ���QTreeWidget������
	void CreateActions();
	void CreateMenu();
	void CreateToolbar();
	void CreateCentralWidget();
	void CreateConnections(); //������mainwindow�͸���widget����connections
};

#endif // MAINWINDOW_H
