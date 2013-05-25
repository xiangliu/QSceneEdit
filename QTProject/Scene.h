#ifndef SCENE_H
#define SCENE_H

#include <string>
#include <vector>
#include <map>
#include <QObject>
#include <limits>
#include "BaseStruct.h"
#include "trimesh/Vec.h"
#include "Material.h"
#include "Model.h"
#include "trimesh/Box.h"
#include "trimesh/bsphere.h"

using namespace std;

/*
warning: 在计算场景中model个数的时候，一定要使用modelSize变量，因为在插入输入框的时候，真正的model还没有被
放进来，但是已经生成一个model将其放置到保存所有模型的vector之中
*/

// 不处理Material则注释
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

	////用于指示场景所在的状态，比如显示场景中的模型、显示场景中某个模型的包围盒、显示场景中待插入的模型的包围盒
	//enum SceneDrawState{DrawSceneModel, DrawSceneModelBBox, DrawSceneModelInseartBBox};
	//SceneDrawState sceneDrawState; //用于指示场景当前的状态

	string scenePath; // obj所在的路径，E://abc//a.obj
	string dirPath; //  根路径，如：E://abc//；

	point *points;
	int Vsize; // 点的个数

	vnormal *vnormals;
	int Vnsize; // 法向个数

	vector<Face*> faces;  //整个场景的所有面片

#ifdef DefMaterial
	string mtlPath;

	vtexture* vtextures;
	int Vtsize; // 纹理个数

	Material *materials;
	int MtlSize;

	int TextureNum; // 纹理段的个数，对应usemtl的那一段

	vector<int> usemtlSlice; // 每次出现usemtl时，faces的个数。
	vector<int> mtlMark; // 与usemtlSlice对应，usemtlSlice[i]~~usemtlSlice[i+1]对应的mtl为：mtlMark[i];
#endif

	//所有的modelSize 都不包含非invisible的model
	int modelSize; // 总模型个数，算上墙壁(这个modelSize仅仅包括原有场景中model的个数)
	int insertModelSize; //新加入的模型个数
	int allModelSize;  //用于表示总的模型个数（原油场景的和新插入的，每次只要上面两个之中有一个改变，同时也更新它）

	//Byte* mvisible;  // 该模型是否可见
	vector<Model*> sceneModels; //场景中的模型
	vector<Model*> newInsertModels; //往场景中新加入的模型
	vector<LightModel> lightSceneModels; //added by liuxiang,轻量级的model，用于3D检索
	vector<float>modelImportance;  //用于记录物体的重要程度：物体包围盒大小、relationship涵盖物体数、相同label个数

	int** relationTable;  // 关系表单
	map<string,int> ModelMap; // Tag : Model_index;第一个填0
	map<string,vector<int>> RelationMap; // Tag : Relationship
	box objectSearchBBox;  //用于检索单个物体摆放至场景中的包围盒
	box bbox; //整个场景的包围盒
	BSphere bsphere; // 包围球
	point sceneCenter;   //用于指示整个场景的中心，由计算场景包围盒的算法去计算

	// 主要操作方法
public:
    bool readScene(const char* filename);
	void DrawScene();
	void DrawSceneWithNewInsertModel(); //用来绘制带有新插入模型的场景
	void need_bbox();
	void need_bsphere();
	void DrawTest();
	void DrawSimpleScene(); //直接绘制模型，不分model
	void SaveScene(); // 保存场景文件

	void SaveRelationFile(); // 保存场景结构配置文件
	void ReadRelationFile(string path); // 读取path对应的场景配置文件

	/************************************************************************/
	/* added by liuxiang                                               */
	/************************************************************************/
	void LightReadRelationFile(string path); // 轻量级的读取path对应的场景配置文件，用于3D场景检索
	void BuildRelationTable1(int **relationTable); //added by liuxiang
	void DrawModelSearchBBox();   //用来往场景中添加用于单个物体检索的立方体
	void CalculateModelImportance();   //用于计算所有模型的重要程度
	int GetAllModelSize();  //用于获取场景中原有模型和新加入模型的总数，包括被设置了invisible的模型
	Model* GetModel(int selectedModel);  //根据模型在modelMap里的序号来获取模型
	// 辅助操作
private:
	// return false,没有找到对应格式处理程序
    bool readHelper(const char* filename);
	// 读取obj格式的Scene
    bool read_obj(const char* filename);
	// 找到路径的文件夹地址
	void ExtractBasePath(const char* filename);

	// 分割由多个顶点构成的面片
	void tess(const vector<int> &thisv,const vector<int> &thisvn);

#ifdef DefMaterial
	// 读取mtl纹理文件信息
	void LoadMtl(string mtlPath);
	// 分割由多个顶点构成的面片,处理纹理
	void tess(const vector<int> &thisv,const vector<int> &thisvt,const vector<int> &thisvn);
#endif
	// 补充模型的结束Faces
	void CompleteModelSetting();
	// 找到模型的Tag
	static string FindModelTag(string name);
	// 建立关系表
	void BuildRelationTable();
	

};

#endif
