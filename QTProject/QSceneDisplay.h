/************************************************************************/
/* 修改1. 将int state; 改成enum类型 enum SceneDisplayState,其实这么多的state还是显得很复杂，后面再优化
*/
/************************************************************************/
#ifndef QSCENEDISPLAY_H
#define QSCENEDISPLAY_H
#include <string>
#include <map>
#include <set>
#include <QGLWidget>
#include <QPoint>
#include <QMouseEvent>
#include "QModelListDialog.h"
#include "QSameModelListDialog.h"
#include "gl/GL.h"
#include "gl/GLU.h"
#include "gl/glut.h"
#include "trimesh/Vec.h"
#include "Scene.h"

namespace Ui {
	class QSceneDisplay;
}

class QSceneDisplay : public QGLWidget
{
	Q_OBJECT

public:
	QSceneDisplay(QWidget *parent = 0);
	~QSceneDisplay();

	// 自定义属性
public:
	//typedef pair<string, float> PAIR;  
	float xangle; //累计角度值
	float yangle; // 累计角度值
	float scale;
    QPoint btnDown; // 左键按下时，屏幕点的位置
	vec eye; // 视线的方向
	vec up; // 向上的方向
	float radius;  // 视线所在球的半径
	GLfloat plane[4];

	/************这种方案暂停使用***************************
	0: 默认状态，可以对场景进行物体拾取，平移，旋转，缩放
	1: 平移场景
	2: 旋转场景
	3: 平移物体
	4: 旋转物体
	5: 缩放物体
	6: 插入用于查找物体的立方体
	7: 查找单个插入模型
	*/
	//int state; // 指示当前的状态，是平移还是选择物体
	//enum SceneDisplayState{PrepareState, SceneTranslation, SceneRotation,
	//	ObjectTranslation, ObjectRotation, ObjectZoom,
	//	ObjectSelected,/*InseartObjectCube,*/SearchSingleModel};


	/***************新的三维场景交互设计方式*********************
	 总体的状态简化成两大类：场景的整体操作 和 场景内单个模型的操作(初始情况为场景状态)
	 场景的整体操作包括三大方面：旋转、平移、缩放；而这三种方式都不需要进行区分，因为可以通过不同的输入来区分
	     其中旋转用鼠标左键、缩放用wheel、平移用鼠标右键
	 而场景内单个模型的操作包括以下几类：单个模型的旋转、平移、缩放；同时还有包括单个模型推荐、单个模型exchange，还包括单个模型的delete
	 问题1：场景整体操作和场景内单个模型操作之间的相互转换
	 解决方案：当点击场景操作中的任意图标，则进入场景操作；双击场景内任意物体拾取成功或者点击场景内单个物体的任意操作则进入单个物体操作
	 问题2：场景内单个物体操作方式之间的相互转换
	 解决方案：1.（前提条件为任意情况：有物体被选中，没有物体被选中）当点击推荐物体时，弹出推荐列表，再双击插入模型之后转入单个模型操作阶段；
	 2.当点击物体中的exchange时，前提条件必须是有物体被选中，弹出exchange列表之后双击进行替换，同样进入单个模型的交互之中
	 3.当点击remove按钮时，前提条件必须是某个模型通过拾取选中，点击remove之后该模型invisible，同时转入场景处理状态
	
	*/
	enum SceneDisplayState{ObjectState,SceneState};

	SceneDisplayState sceneDisplayState; //用于指示场景当前的状态

	int selectModel;  // 拾取时选中的模型的index，注意区分单个模型检索时的情况,如果没有选定则置为-1,(下标是从0开始计算)

	Scene *scene;    //保存显示的场景

	GLdouble *glProjectionM;  // Projection Matrix
	GLdouble *glModelM; // Model Matrix
	int *glViewM;   // viewport

	//************定义单个模型推荐（检索）相关的变量***************
	map<string,map<string,float>> sceneModelSizeRatio; //用于保存通过计算得到的场景内物体之间的大小比例关系，都是与chair的比值
	bool isFirstRecommend;  //用于表示是否是第一次进行推荐，因为如果是第一次则很多变量需要初始化
	string sceneStyle;  //用于表示当前场景的类别（bedroom。。。等）
	int selected3DModel;   //-1代表什么都没有选(这是用来记录在检索列表中的选择，注意区分于拾取)
	pObjectMatRes pObjectMatchResult;
	//vector<string>recommendLabelPath;  //用于保存推荐物体的数据库路径
	map<string,map<string,int>>sceneLabelAndCount;  //用于保存每个场景类别中所拥有的label集合
	map<string,int>sourceSceneLabels;  //用户输入场景所包含的场景集合
	map<string,int>currentSceneLabels;  //检索得到的当前场景所拥有的label集合
	map<string,map<string,map<int,map<string,map<int,double>>>>> sceneLabelRelevence;  //保存不同场景中不同物体label下的相关度
	map<string,double> recommendLabelAndWeight; //用于保存当前推荐所有物体的label和权重
	vector<string> recomendModelLabel; //用于保存最新一次推荐模型的所有label，使用之前必须清空

	//**************与单个模型检索结果列表展示相关的变量***************
	QModelListDialog* modelListDialog;
	QSameModelListDialog* sameModelListDialog;   //用于替换单个模型类别的对话框
	
	//***********************画材质相关变量***************************
	GLubyte checkImage[64][64][4];
	GLuint texName;

	// 自定义方法
public:
	void DrawCoodinates();
	void DrawScene();
	bool isSelectedModelValid();
	void SetProjectionModelView();
	void generateInseartModleInformation(Model* insertModel);  //为插入的模型计算模型相关信息
	void CalculateModelSizeRatio(Model* inseartModelT); //用于计算当前插入模型放大缩小的比例
	void makeCheckImage();   //画地板材质使用的方法
	void generateExchangeModleInformation(Model* exchangeModel);  //为exchange的模型计算模型相关信息

public slots:
	void SetDisScene(Scene* scene);  // 将读入的场景传入过来，方便进行操作
	//void ChooseModelAction();  // 响应选择模型Action
	//void TransModelAction();
	//void RotateModelAction();
	void sceneOperationAction();  //用于响应菜单中进入场景整体操作
	void searchInseartObject();  //用于响应菜单和图标中的单个模型检索功能
	void exchangeModelAction();   //用于将场景中某个物体用同类别的模型进行替换
	void removeModelAction(); // 用于将场景中某个模型删除，事实上是设置成invisible

	//void pickupCubeAction();  //选择往场景中添加新的模型，首先即选择添加一个立方体，随后再将立方体平移到合适位置放大缩小
	void Inseart3DModel(int selectedModel);  //用于响应QModelListDialog发射的信号，download被挑选的物体，随后再插入场景，让场景重绘
	void Exchange3DModel(int clickedModel);   //用于响应QSameModelListDialog发射的信号，download被挑选的物体，随后再插入场景，替换原有物体，随后让场景重绘
signals:

protected:
	void initializeGL();
	void paintGL();
	void resizeGL(int width,int height);
	int ProcessSelection(int xPos,int yPos); //用于处理用户的拾取，返回值表明用户是否拾取成功
	void ProcessModels(GLuint *pSelectBuff);
	void recommendModelsBySelectedLabel(int recommendBasedModel);   //基于用于选定的模型来推荐新的模型
	void recommendModelsBySelectedLabel2(int recommendBasedModel); //基于用于选定的模型来推荐新的模型,第二版本推荐算法
	
	// 事件处理
protected:
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);  //仅仅用于拾取


private:
};

#endif // QSCENEDISPLAY_H
