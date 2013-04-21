/************************************************************************/
/* �޸�1. ��int state; �ĳ�enum���� enum SceneDisplayState,��ʵ��ô���state�����Եúܸ��ӣ��������Ż�
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

	// �Զ�������
public:
	//typedef pair<string, float> PAIR;  
	float xangle; //�ۼƽǶ�ֵ
	float yangle; // �ۼƽǶ�ֵ
	float scale;
    QPoint btnDown; // �������ʱ����Ļ���λ��
	vec eye; // ���ߵķ���
	vec up; // ���ϵķ���
	float radius;  // ����������İ뾶
	GLfloat plane[4];

	/*
	0: Ĭ��״̬�����ԶԳ�����������ʰȡ��ƽ�ƣ���ת������
	1: ƽ�Ƴ���
	2: ��ת����
	3: ƽ������
	4: ��ת����
	5: ��������
	6: �������ڲ��������������
	7: ���ҵ�������ģ��
	*/
	//int state; // ָʾ��ǰ��״̬����ƽ�ƻ���ѡ������
	enum SceneDisplayState{PrepareState, SceneTranslation, SceneRotation,
		ObjectTranslation, ObjectRotation, ObjectZoom,
		ObjectSelected,/*InseartObjectCube,*/SearchSingleModel};

	SceneDisplayState sceneDisplayState; //����ָʾ������ǰ��״̬

	int selectModel;  // ʰȡʱѡ�е�ģ�͵�index��ע�����ֵ���ģ�ͼ���ʱ�����,���û��ѡ������Ϊ-1,(�±��Ǵ�0��ʼ����)

	Scene *scene;    //������ʾ�ĳ���

	GLdouble *glProjectionM;  // Projection Matrix
	GLdouble *glModelM; // Model Matrix
	int *glViewM;   // viewport

	//************���嵥��ģ�ͼ�����صı���***************
	string sceneStyle;  //���ڱ�ʾ��ǰ���������bedroom�������ȣ�
	int selected3DModel;   //-1����ʲô��û��ѡ
	pObjectMatRes pObjectMatchResult;
	//vector<string>recommendLabelPath;  //���ڱ����Ƽ���������ݿ�·��
	map<string,map<string,int>>sceneLabelAndCount;  //���ڱ���ÿ�������������ӵ�е�label����
	map<string,int>sourceSceneLabels;  //�û����볡���������ĳ�������
	map<string,int>currentSceneLabels;  //�����õ��ĵ�ǰ������ӵ�е�label����
	map<string,map<string,map<int,map<string,map<int,double>>>>> sceneLabelRelevence;  //���治ͬ�����в�ͬ����label�µ���ض�
	map<string,double> recommendLabelAndWeight; //���ڱ��浱ǰ�Ƽ����������label��Ȩ��

	//**************�뵥��ģ�ͼ�������б�չʾ��صı���***************
	QModelListDialog* modelListDialog;
	QSameModelListDialog* sameModelListDialog;   //�����滻����ģ�����ĶԻ���

	// �Զ��巽��
public:
	void DrawCoodinates();
	void DrawScene();
	bool isSelectedModelValid();
	void SetProjectionModelView();

public slots:
	void SetDisScene(Scene* scene);  // ������ĳ������������������в���
	void ChooseModelAction();  // ��Ӧѡ��ģ��Action
	void TransModelAction();
	void RotateModelAction();
	void pickupCubeAction();  //ѡ��������������µ�ģ�ͣ����ȼ�ѡ�����һ�������壬����ٽ�������ƽ�Ƶ�����λ�÷Ŵ���С
	void searchInseartObject();  //������Ӧ�˵���ͼ���еĵ���ģ�ͼ�������
	void Inseart3DModel(int selectedModel);  //������ӦQModelListDialog������źţ�download����ѡ�����壬����ٲ��볡�����ó����ػ�

signals:

protected:
	void initializeGL();
	void paintGL();
	void resizeGL(int width,int height);
	int ProcessSelection(int xPos,int yPos); //���ڴ����û���ʰȡ������ֵ�����û��Ƿ�ʰȡ�ɹ�
	void ProcessModels(GLuint *pSelectBuff);
	void recommendModelsBySelectedLabel(int recommendBasedModel);   //��������ѡ����ģ�����Ƽ��µ�ģ��
	void recommendModelsBySelectedLabel2(int recommendBasedModel); //��������ѡ����ģ�����Ƽ��µ�ģ��,�ڶ��汾�Ƽ��㷨
	
	// �¼�����
protected:
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void mousePressEvent(QMouseEvent *event);
	


private:
};

#endif // QSCENEDISPLAY_H
