/************************************************************************/
/* �޸�1. ��int state; �ĳ�enum���� enum SceneDisplayState,��ʵ��ô���state�����Եúܸ��ӣ��������Ż�
*/
/************************************************************************/
#ifndef QSCENEDISPLAY_H
#define QSCENEDISPLAY_H

#include <QGLWidget>
#include <QPoint>
#include <QMouseEvent>
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
	*/
	//int state; // ָʾ��ǰ��״̬����ƽ�ƻ���ѡ������
	enum SceneDisplayState{PrepareState, SceneTranslation, SceneRotation,
		ObjectTranslation, ObjectRotation, ObjectZoom};
	SceneDisplayState sceneDisplayState; //����ָʾ������ǰ��״̬

	int selectModel;  // ѡ�е�ģ�͵�index

	Scene *scene;

	GLdouble *glProjectionM;  // Projection Matrix
	GLdouble *glModelM; // Model Matrix
	int *glViewM;   // viewport

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

signals:

protected:
	void initializeGL();
	void paintGL();
	void resizeGL(int width,int height);
	void ProcessSelection(int xPos,int yPos);
	void ProcessModels(GLuint *pSelectBuff);

	// �¼�����
protected:
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void mousePressEvent(QMouseEvent *event);
	


private:
};

#endif // QSCENEDISPLAY_H
