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
warning: �ڼ��㳡����model������ʱ��һ��Ҫʹ��modelSize��������Ϊ�ڲ���������ʱ��������model��û�б�
�Ž����������Ѿ�����һ��model������õ���������ģ�͵�vector֮��
*/

// ������Material��ע��
//#ifndef DefMaterial
//#define  DefMaterial
//#endif
/*
	��Ӧһ���������������Ժ��Ƕ��ģ��
*/

class Model;

class Scene : public QObject
{
	 Q_OBJECT

public:
    Scene(QObject* parent=0);
	~Scene(void);

	// ��Ҫ����
public:

	////����ָʾ�������ڵ�״̬��������ʾ�����е�ģ�͡���ʾ������ĳ��ģ�͵İ�Χ�С���ʾ�����д������ģ�͵İ�Χ��
	//enum SceneDrawState{DrawSceneModel, DrawSceneModelBBox, DrawSceneModelInseartBBox};
	//SceneDrawState sceneDrawState; //����ָʾ������ǰ��״̬

	string scenePath; // obj���ڵ�·����E://abc//a.obj
	string dirPath; //  ��·�����磺E://abc//��

	point *points;
	int Vsize; // ��ĸ���

	vnormal *vnormals;
	int Vnsize; // �������

	vector<Face*> faces;  //����������������Ƭ

#ifdef DefMaterial
	string mtlPath;

	vtexture* vtextures;
	int Vtsize; // �������

	Material *materials;
	int MtlSize;

	int TextureNum; // ����εĸ�������Ӧusemtl����һ��

	vector<int> usemtlSlice; // ÿ�γ���usemtlʱ��faces�ĸ�����
	vector<int> mtlMark; // ��usemtlSlice��Ӧ��usemtlSlice[i]~~usemtlSlice[i+1]��Ӧ��mtlΪ��mtlMark[i];
#endif

	//���е�modelSize ����������invisible��model
	int modelSize; // ��ģ�͸���������ǽ��(���modelSize��������ԭ�г�����model�ĸ���)
	int insertModelSize; //�¼����ģ�͸���
	int allModelSize;  //���ڱ�ʾ�ܵ�ģ�͸�����ԭ�ͳ����ĺ��²���ģ�ÿ��ֻҪ��������֮����һ���ı䣬ͬʱҲ��������

	//Byte* mvisible;  // ��ģ���Ƿ�ɼ�
	vector<Model*> sceneModels; //�����е�ģ��
	vector<Model*> newInsertModels; //���������¼����ģ��
	vector<LightModel> lightSceneModels; //added by liuxiang,��������model������3D����
	vector<float>modelImportance;  //���ڼ�¼�������Ҫ�̶ȣ������Χ�д�С��relationship��������������ͬlabel����

	int** relationTable;  // ��ϵ��
	map<string,int> ModelMap; // Tag : Model_index;��һ����0
	map<string,vector<int>> RelationMap; // Tag : Relationship
	box objectSearchBBox;  //���ڼ�����������ڷ��������еİ�Χ��
	box bbox; //���������İ�Χ��
	BSphere bsphere; // ��Χ��
	point sceneCenter;   //����ָʾ�������������ģ��ɼ��㳡����Χ�е��㷨ȥ����

	// ��Ҫ��������
public:
    bool readScene(const char* filename);
	void DrawScene();
	void DrawSceneWithNewInsertModel(); //�������ƴ����²���ģ�͵ĳ���
	void need_bbox();
	void need_bsphere();
	void DrawTest();
	void DrawSimpleScene(); //ֱ�ӻ���ģ�ͣ�����model
	void SaveScene(); // ���泡���ļ�

	void SaveRelationFile(); // ���泡���ṹ�����ļ�
	void ReadRelationFile(string path); // ��ȡpath��Ӧ�ĳ��������ļ�

	/************************************************************************/
	/* added by liuxiang                                               */
	/************************************************************************/
	void LightReadRelationFile(string path); // �������Ķ�ȡpath��Ӧ�ĳ��������ļ�������3D��������
	void BuildRelationTable1(int **relationTable); //added by liuxiang
	void DrawModelSearchBBox();   //������������������ڵ������������������
	void CalculateModelImportance();   //���ڼ�������ģ�͵���Ҫ�̶�
	int GetAllModelSize();  //���ڻ�ȡ������ԭ��ģ�ͺ��¼���ģ�͵�������������������invisible��ģ��
	Model* GetModel(int selectedModel);  //����ģ����modelMap����������ȡģ��
	// ��������
private:
	// return false,û���ҵ���Ӧ��ʽ�������
    bool readHelper(const char* filename);
	// ��ȡobj��ʽ��Scene
    bool read_obj(const char* filename);
	// �ҵ�·�����ļ��е�ַ
	void ExtractBasePath(const char* filename);

	// �ָ��ɶ�����㹹�ɵ���Ƭ
	void tess(const vector<int> &thisv,const vector<int> &thisvn);

#ifdef DefMaterial
	// ��ȡmtl�����ļ���Ϣ
	void LoadMtl(string mtlPath);
	// �ָ��ɶ�����㹹�ɵ���Ƭ,��������
	void tess(const vector<int> &thisv,const vector<int> &thisvt,const vector<int> &thisvn);
#endif
	// ����ģ�͵Ľ���Faces
	void CompleteModelSetting();
	// �ҵ�ģ�͵�Tag
	static string FindModelTag(string name);
	// ������ϵ��
	void BuildRelationTable();
	

};

#endif
