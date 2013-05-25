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
对应一个模型，应该涵盖了自己的包围盒、包围球等属性
*/
class Scene;

class Model
{
public:
	Model(void);
	~Model(void);

	// 自定义属性
public:
	int faceStart;  // 对应该Object的Face区间开始的值，假设区间为faceStart<= x < faceEnd；该值为对应的index值。
	int faceEnd;  // 对应该Object的Face区间的结束的值
	string name; // 对应物体的名称，表示为Tag0Num
	string tag; // 对应物体的类别，如：Chair，Desk，etc
	bool visible; // 对应物体是否绘制到场景中，false则不需要绘制该物体
	Scene *scene; // 指向所属的场景
	map<int,point> pointMap; // 记录该模型所用到的点集，避免在计算bbox和bsphere时过慢，为后续模型变化做准备

	vector<Face*> modelFaces;  //用于保存从google warehouse中download的单个三维模型的面片，因为该类三维模型的格式不同于从场景中分割的三维模型格式
	vector<point*> pointVect;
	vector<vnormal*> normalVect;

	TriMesh *mesh; // 读取模型

	box bbox; //包围盒
	BSphere bsphere; //包围球

	GLfloat tx; // translate的x
	GLdouble ty; // translate的y
	GLdouble tz; // translate的z

	GLdouble scaled; // 缩放大小

	GLdouble xangle;  // 绕x的角度
	GLdouble yangle;  // 绕y的角度

	GLdouble *ModelMatrix; // 模型矩阵

    //arcball ball; // 每个物体的旋转球，包含了对应的变换矩阵，作为保存物体的时候，坐标变换的矩阵

	GLfloat *faceAreaSum;
	point *samplePoints;
	int pointNumber;
	// 自定义方法
public:
	void DrawModel();
	void DrawNewInseartModel(); //用于绘制新加入场景中的模型
	void need_bbox();
	void need_bbox_newInseartOBJ();  //为新插入的OBJ计算包围盒
	void need_bsphere();
	void need_bsphere_newInseartOBJ(); //为新插入的OBJ计算包围球
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

	// 描述子方法
public:
	void ReadModel(string filename); // 读取
	void ReadOBJ(string filePath); //读取从google sketchup download的模型，因为trimesh的方法读取不了
	void ModelTess(const vector<int> &thisv,const vector<int> &thisvn); //改写的trimesh的方法tess
	void ReadOBJHelp(string filePath); //
	void PCAOperation();  //旋转到pca方向，并进行单位化
	void DrawTrimesh();
};

#endif
