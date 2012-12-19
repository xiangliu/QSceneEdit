#ifndef SCENE_H
#define SCENE_H

#include <string>
#include <vector>
#include <map>
#include <QObject>
#include "BaseStruct.h"
#include "trimesh/Vec.h"
#include "Material.h"
#include "Model.h"
#include "trimesh/Box.h"
#include "trimesh/bsphere.h"

using namespace std;

//// 不处理Material则注释
//#ifndef DefMaterial
//#define  DefMaterial
//#endif
/*
	对应一个场景，场景可以涵盖多个模型
*/

class Model;

class Scene : public QObject
{
	 Q_OBJECT

public:
    Scene(QObject* parent=0);
	~Scene(void);

	// 主要属性
public:
	string scenePath; // obj所在的路径，E://abc//a.obj
	string dirPath; //  根路径，如：E://abc//；

	point *points;
	int Vsize; // 点的个数

	vnormal *vnormals;
	int Vnsize; // 法向个数

	vtexture* vtextures;
	int Vtsize; // 纹理个数

	vector<Face*> faces;
	int TextureNum; // 纹理段的个数，对应usemtl的那一段

	Material *materials;
	int MtlSize;

	vector<int> usemtlSlice; // 每次出现usemtl时，faces的个数。
	vector<int> mtlMark; // 与usemtlSlice对应，usemtlSlice[i]~~usemtlSlice[i+1]对应的mtl为：mtlMark[i];

	int modelSize; // 总模型个数，算上墙壁
	//Byte* mvisible;  // 该模型是否可见
	vector<Model*> sceneModels; //场景中的模型


	int** relationTable;  // 关系表单
	map<string,int> ModelMap; // Tag : Model_index
	map<string,vector<int>> RelationMap; // Tag : Relationship

	box bbox; //包围盒
	BSphere bsphere; // 包围球

	// 主要操作方法
public:
    bool readScene(const char* filename);
	void DrawScene();
	void need_bbox();
	void need_bsphere();
	void DrawTest();
	void DrawSimpleScene(); //直接绘制模型，不分model
	// 辅助操作
public:
	// return false,没有找到对应格式处理程序
    bool readHelper(const char* filename);
	// 读取obj格式的Scene
    bool read_obj(const char* filename);
	// 找到路径的文件夹地址
	void ExtractBasePath(const char* filename);
	// 分割由多个顶点构成的面片
	void tess(const vector<int> &thisv,const vector<int> &thisvt,const vector<int> &thisvn);
	// 读取mtl纹理文件信息
	void LoadMtl(string mtlPath);
	// 补充模型的结束Faces
	void CompleteModelSetting();
	// 找到模型的Tag
	static string FindModelTag(string name);
	// 建立关系表
	void BuildRelationTable();
};

#endif
