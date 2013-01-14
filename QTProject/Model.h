#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <string>
#include "Scene.h"
#include "BaseStruct.h"
#include "trimesh/TriMesh.h"
#include "trimesh/Box.h"
#include "trimesh/bsphere.h"
#include "trimesh/TriMesh_algo.h"
#include <map>
#include "mersenne-twister.h"
//#include <gsl/gsl_qrng.h>
#include <time.h>

using namespace std;
/*
��Ӧһ��ģ�ͣ�Ӧ�ú������Լ��İ�Χ�С���Χ�������
*/
class Scene;

class Model
{
public:
	Model(void);
	~Model(void);

	// �Զ�������
public:
	int faceStart;  // ��Ӧ��Object��Face���俪ʼ��ֵ����������ΪfaceStart<= x < faceEnd����ֵΪ��Ӧ��indexֵ��
	int faceEnd;  // ��Ӧ��Object��Face����Ľ�����ֵ
	string name; // ��Ӧ��������ƣ���ʾΪTag0Num
	string tag; // ��Ӧ���������磺Chair��Desk��etc
	bool visible; // ��Ӧ�����Ƿ���Ƶ������У�false����Ҫ���Ƹ�����
	Scene *scene; // ָ�������ĳ���
	map<int,point> pointMap; // ��¼��ģ�����õ��ĵ㼯�������ڼ���bbox��bsphereʱ������Ϊ����ģ�ͱ仯��׼��

	TriMesh *mesh; // ��ȡģ��

	box bbox; //��Χ��
	BSphere bsphere; //��Χ��

	GLfloat tx; // translate��x
	GLdouble ty; // translate��y
	GLdouble tz; // translate��z

	GLdouble scaled; // ���Ŵ�С

	GLdouble xangle;  // ��x�ĽǶ�
	GLdouble yangle;  // ��y�ĽǶ�

	GLdouble *ModelMatrix; // ģ�;���

    //arcball ball; // ÿ���������ת�򣬰����˶�Ӧ�ı任������Ϊ���������ʱ������任�ľ���

	GLfloat *faceAreaSum;
	point *samplePoints;
	int pointNumber;
	// �Զ��巽��
public:
	void DrawModel();
	void need_bbox();
	void need_bsphere();
	void DrawBbox();
	void CalculateArea();
	void GeneratePoints(int total);
	void DrawPoints();
	void SaveSceneModel();
	//
    //gsl_qrng * qRand;
	void InitRand();
	float GetRand(int type);
	void EndRand();

	// �����ӷ���
public:
	void ReadModel(string filename); // ��ȡ
	void PCAOperation();  //��ת��pca���򣬲����е�λ��
	void DrawTrimesh();
};

#endif
