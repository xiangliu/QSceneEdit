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
#include "QTagDisplay.h"
#include "MathLib.h"
#include "QSetRelationDialog.h"

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

	//��ʾ�������ڵ�״̬��������Ƭ����imageHandle������Ƭ�����ϵ���ɣ�setRelationship��
	//����3D������search3DScene������ʾ3D������display3DScene��
	enum entireProcessState { imageHandle, setRelationship ,search3DScene , display3DScene,threeDProcess};
	entireProcessState entireState ;

	//����relationship�����ࣺ 0--Ĭ���޹�ϵ��1--ˮƽ֧�֣�support����2--��Χ��enclosured����3--��ֱ�Ӵ���vContact����4--group��ϵ
	//5--ˮƽ��֧�֣� 6--����Χ


	// �������
    QSceneDisplay *sceneDisplayWidget;
	QPictureDisplay *pictureDisplayWidget;
	QSegPictureDisplay *segPictureDisplayWidget;
	QSetRelationDialog *setRelationDialog;

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
	QMenu *imageEditMenu;
	QMenu *searchMenu; //��������relationship�ͽ���search��menu
	QMenu *modelEditMenu;
	QMenu *sceneEditMenu;
	
	// ������
	QToolBar *fileToolBar;
	QToolBar *segImageToolBar;   //���ڶ�������Ƭ���д���
	QToolBar *searchToolBar;     //���ڴӶ�ά����������ά����
	QToolBar *editModelToolBar;  //���ڱ༭3D�����е���ģ��
	QToolBar *editSceneToolBar;  //���ڱ༭����3D����
	

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

	//action related with search
	QAction *createRelationAction;
	QAction *searchSceneAction;
	QAction *view3DSceneAction;

	//represent the file to be paint
    Scene *scene;
    QImage *SourceImage;  //represent the loaded image file

	//�����ά�ĳ�����ر���
	TwoDScene  twdScene;  //�����ά����������
	int twdObjectCount;   //�����ά������object ����
	CSegObject** objectList; //�������б��ָ�object�󱣴�ĵ�ַ
	int *relationship;    //����һ��һά�������������еĶ�άobject֮���relationship
    // �Զ��巽��
public:


signals:
	void SetDisScene(Scene* scene); // ������ĳ���ָ�봫����ʾ����
	void OpenImageFile(QImage *SourceImage); //Open image file
	void PickImageObject(int state); //pick object from the segmented image
	void SendPicDisMesg(QSegPictureDisplay *segPictureDisplayWidget); //send QPictureDisplay the poniter of QSegPictureDisplay
	void SaveSegObject(QString tag, int weight); //��������QPictureDisplay.hȥ���汻�ָ�õ�����
	void ClearDrawRect(); //���͸�QSegPictureDisplay,�������������һ�ε�boject
public slots:
    void OpenSceneFile();
    void SaveSceneFile();
    void MOpenImageFile();  //********added by liu xiang; for open picture*********
	void ClickImageSaveButton();  //����Ӧ���ڱ��浥����Ƭ��tag��weight
	//void PrepareRelations();      //�����ڽ���relationship��set֮ǰ��������
	void SetRelations();     //���ݲ˵���ͼ��trigger�¼�������relationship��dialog
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
