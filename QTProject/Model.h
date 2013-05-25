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

	vector<Face*> modelFaces;  //���ڱ����google warehouse��download�ĵ�����άģ�͵���Ƭ����Ϊ������άģ�͵ĸ�ʽ��ͬ�ڴӳ����зָ����άģ�͸�ʽ
	vector<point*> pointVect;
	vector<vnormal*> normalVect;

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
	void DrawNewInseartModel(); //���ڻ����¼��볡���е�ģ��
	void need_bbox();
	void need_bbox_newInseartOBJ();  //Ϊ�²����OBJ�����Χ��
	void need_bsphere();
	void need_bsphere_newInseartOBJ(); //Ϊ�²����OBJ�����Χ��
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
	void ReadOBJ(string filePath); //��ȡ��google sketchup download��ģ�ͣ���Ϊtrimesh�ķ�����ȡ����
	void ModelTess(const vector<int> &thisv,const vector<int> &thisvn); //��д��trimesh�ķ���tess
	void ReadOBJHelp(string filePath); //
	void PCAOperation();  //��ת��pca���򣬲����е�λ��
	void DrawTrimesh();
};

#endif
