#ifndef MATH_LIB_H
#define MATH_LIB_H

/***
2012/12/16, ��struct face��mface����Ϊ��BaseStruct.h���Face �������ͻ
*/

#include "math.h"
#include <map>
#include <vector>
#include "Global.h"
using namespace std;

#define MINERROR 0.00001

#define M_PI 3.14
#define LINE_MAX_LEN 256

typedef struct BoundingBox
{
	float minX, maxX;
	float minY, maxY;
	float minZ, maxZ;
}BoundingBox;

typedef struct MVector    //����
{
	double x, y, z;
	/*MVector(const MVector& vec)
	{
		this->x = vec.x;
		this->y = vec.y;
		this->z = vec.z;
	}*/
	MVector(double xx, double yy, double zz)    //���캯��
	{
		x = xx;
		y = yy;
		z = zz;
	}
	MVector()         //Ĭ�Ϲ��캯��
	{
		x = y = z = 0;
	}
} MVector;

typedef struct Vertex   //����
{
	float x, y, z;
	int index;			// ��¼�ڶ����б��е����������������
} Vertex;

//typedef struct Face    //��
//{
//	Face(void) : nverts(0), verts(0) {};
//	int nverts;      //��ĸ���
//	Vertex **verts;
//	MVector* normal;		// ������������С�������Ϊ�գ�
//	Vertex mass;		// ����
//	double area;		// ���
//	vector<unsigned int> vertexNum;//��¼ÿ�����������ĵ����ţ��Ա㱣��
//} Face;


typedef struct MFace    //��
{
	int v[3];
	double area;		// ���
	Vertex mass;		// ����
	MVector* normal;		// ������������С�������Ϊ�գ�
	MFace() {}
	MFace(const int &v0, const int &v1, const int &v2)
	{ v[0] = v0; v[1] = v1; v[2] = v2; }
	MFace(const int *v_)
	{ v[0] = v_[0]; v[1] = v_[1]; v[2] = v_[2]; }
} MFace;



typedef struct Mesh   //����
{
	Mesh(void) : verts(0),faces(0) {};
	vector<Vertex> verts;
	vector<MFace> faces;
	MVector vx, vy, vz;	// �������᷽��
	int nfaces;
	int nverts;
	BoundingBox mesh_bb; //��������¼mesh�Ĵ�С���ڰڷŵ�ʱ����Ҫ�ʵ���������
	//added by liu xiang; ������¼group��ʼ��face���
	vector<int> groups; 
	float center[3]; // boundingbox���ĵ�
} Mesh;

typedef struct MiniBall
{
	Vertex center;
	float radius;
}MiniBall;


//*********added by liuxiang;���ڱ�����ά����������

//���ڱ��泡���ļ������
typedef struct SceneMatRes_ *pSceneMatRes;
typedef struct SceneMatRes_ {				// Match Result
	char			name[200];	// file name
	float			similarity ;
}SceneMatRes;

//���ڱ��浥������ļ������
typedef struct ObjectMatRes_ *pObjectMatRes;
typedef struct ObjectMatRes_ {				// Match Result
	char			name[200];	// file name
	float			similarity ;
}ObjectMatRes;

typedef float Matrix3D[16];//ÿ����άģ�Ͱ�����4*4����

typedef struct ThreeDScene
{
	unsigned int modelNum;//��ʾ��άģ�͸���
	vector<float*> matrixP;//ÿ����άģ�Ͱ�����4*4����ֻ���������ƶ�
	vector<float*> rotationMatrixP;//ÿ����άģ�Ͱ�����4*4��ת����
	Matrix3D matrixAll; //Ϊ����ģ�͹�ͬ���õ�ģ�Ӿ���
	Matrix3D rotationMatrixAll; //Ϊ����ģ�͹�ͬ���õ�ģ�Ӿ���
	BoundingBox allMesh_bb; //Ϊ���е�Mesh�趨һ��boundingbox��������¼���ģ�͵Ĵ�С
	float sCenter[3]; //������ʾ���������Χ�е�����
}ThreeDScene;

//���ڱ����ȡ�Ķ�ά��Ƭ�ָ���
typedef struct TwoDScene
{
	unsigned int modelNum;//��ʾ���ָ�ģ�͵��ܸ���
	vector<char*> texture;//ÿ����������������ǰ�˳�򱣴��
	float importance[MAXSEGMENTNUMBER]; //���ڼ�¼�������Ҫ�̶ȣ���Ҫ�̶������������С�ͷ�������ͬtag�������
	char relationship[MAXSEGMENTNUMBER][MAXSEGMENTNUMBER]; //������������֮��Ĺ�ϵ����
	char tag1[MAXSEGMENTNUMBER][30];//��������ģ�Ͷ�Ӧ�ı�ǩ����ģ�͵����ͣ������������ӵ� 
	char pictureFilename[200]; //���ڱ�����Ƭ���ļ���
}TwoDScene;

//��������ú�search�����ص���������ʱ����ʹ��
//���ڶ�ȡ��ά�����ָ���
//typedef struct ThreeDSource
//{
	//unsigned int modelNum;//��ʾ���ָ�ģ�͵��ܸ���
	//vector<char*> texture;//ÿ����������������ǰ�˳�򱣴��
	//float importance[MAXSEGMENTNUMBER]; //���ڼ�¼�������Ҫ�̶ȣ���Ҫ�̶������������С�ͷ�������ͬtag�������
	//char relationship[MAXSEGMENTNUMBER][MAXSEGMENTNUMBER]; //������������֮��Ĺ�ϵ����,��char��ԭ���Ǻ��������չ��26����ĸ������ÿ�ֹ�ϵֻ��һ����ĸ�Ϳ��Ա�ʾ
	//char tag1[MAXSEGMENTNUMBER][30];//��������ģ�Ͷ�Ӧ�ı�ǩ����ģ�͵����ͣ������������ӵ� 
//}ThreeDSource;

//���ڱ���3Dģ�͵Ĳ��ʣ�copy from 3D search
typedef struct Meterial_ *pMeterial;
typedef struct Meterial_ {
	char			name[100];
	double			r, g, b;
	pMeterial		pointer;
}Meterial;

//�����Ų������õ�����matrix��
inline void Matrix3DSetScaling(Matrix3D matrix, float xScale,
	float yScale, float zScale)
{
	matrix[1] = matrix[2] = matrix[3] = matrix[4] = 0.0;
	matrix[6] = matrix[7] = matrix[8] = matrix[9] = 0.0;
	matrix[11] = matrix[12] = matrix[13] = matrix[14] = 0.0;
	matrix[0] = xScale;
	matrix[5] = yScale;
	matrix[10] = zScale;
	matrix[15] = 1.0;
}


//��matrix��λ����
//inline void Matrix3DSetIdentity(Matrix3D matrix);
inline void Matrix3DSetIdentity(Matrix3D matrix)
{
	matrix[0] = matrix[5] =  matrix[10] = matrix[15] = 1.0;
	matrix[1] = matrix[2] = matrix[3] = matrix[4] = 0.0;
	matrix[6] = matrix[7] = matrix[8] = matrix[9] = 0.0;
	matrix[11] = matrix[12] = matrix[13] = matrix[14] = 0.0;
}
//������������˷���result��ͷ
// m2 x m1
inline void Matrix3DMultiply(Matrix3D m1, Matrix3D m2, Matrix3D result)
{
	result[0] = m1[0] * m2[0] + m1[4] * m2[1] + m1[8] * m2[2] + m1[12] * m2[3];
	result[1] = m1[1] * m2[0] + m1[5] * m2[1] + m1[9] * m2[2] + m1[13] * m2[3];
	result[2] = m1[2] * m2[0] + m1[6] * m2[1] + m1[10] * m2[2] + m1[14] * m2[3];
	result[3] = m1[3] * m2[0] + m1[7] * m2[1] + m1[11] * m2[2] + m1[15] * m2[3];

	result[4] = m1[0] * m2[4] + m1[4] * m2[5] + m1[8] * m2[6] + m1[12] * m2[7];
	result[5] = m1[1] * m2[4] + m1[5] * m2[5] + m1[9] * m2[6] + m1[13] * m2[7];
	result[6] = m1[2] * m2[4] + m1[6] * m2[5] + m1[10] * m2[6] + m1[14] * m2[7];
	result[7] = m1[3] * m2[4] + m1[7] * m2[5] + m1[11] * m2[6] + m1[15] * m2[7];

	result[8] = m1[0] * m2[8] + m1[4] * m2[9] + m1[8] * m2[10] + m1[12] * m2[11];
	result[9] = m1[1] * m2[8] + m1[5] * m2[9] + m1[9] * m2[10] + m1[13] * m2[11];
	result[10] = m1[2] * m2[8] + m1[6] * m2[9] + m1[10] * m2[10] + m1[14] * m2[11];
	result[11] = m1[3] * m2[8] + m1[7] * m2[9] + m1[11] * m2[10] + m1[15] * m2[11];

	result[12] = m1[0] * m2[12] + m1[4] * m2[13] + m1[8] * m2[14] + m1[12] * m2[15];
	result[13] = m1[1] * m2[12] + m1[5] * m2[13] + m1[9] * m2[14] + m1[13] * m2[15];
	result[14] = m1[2] * m2[12] + m1[6] * m2[13] + m1[10] * m2[14] + m1[14] * m2[15];
	result[15] = m1[3] * m2[12] + m1[7] * m2[13] + m1[11] * m2[14] + m1[15] * m2[15];
}
//��ƽ�Ʋ������õ�����matrix��
inline void Matrix3DSetTranslation(Matrix3D matrix, float xTranslate,
	float yTranslate, float zTranslate)
{
	matrix[0] = matrix[5] =  matrix[10] = matrix[15] = 1.0;
	matrix[1] = matrix[2] = matrix[3] = matrix[4] = 0.0;
	matrix[6] = matrix[7] = matrix[8] = matrix[9] = 0.0;
	matrix[11] = 0.0;
	matrix[12] = xTranslate;
	matrix[13] = yTranslate;
	matrix[14] = zTranslate;
}
//����ת�������õ�����matrix��
inline void Matrix3DSetRotationByRadians(Matrix3D matrix, float angle,
	float x, float y, float z)
{
	float mag = sqrtf((x*x) + (y*y) + (z*z));
	if (mag == 0.0)
	{
		x = 1.0;
		y = 0.0;
		z = 0.0;
	}
	else if (mag != 1.0)
	{
		x /= mag;
		y /= mag;
		z /= mag;
	}

	float c = cosf(angle);
	float s = -sinf(angle);
	//float s = fastSinf(angle);
	matrix[3] = matrix[7] = matrix[11] = matrix[12] = matrix[13] = matrix[14] = 0.0;
	matrix[15] = 1.0;

	matrix[0] = (x*x)*(1-c) + c;
	matrix[1] = (y*x)*(1-c) + (z*s);
	matrix[2] = (x*z)*(1-c) - (y*s);
	matrix[4] = (x*y)*(1-c)-(z*s);
	matrix[5] = (y*y)*(1-c)+c;
	matrix[6] = (y*z)*(1-c)+(x*s);
	matrix[8] = (x*z)*(1-c)+(y*s);
	matrix[9] = (y*z)*(1-c)-(x*s);
	matrix[10] = (z*z)*(1-c)+c;

}
inline void Matrix3DSetRotationByDegrees(Matrix3D matrix, float angle,
	float x, float y, float z)
{
	Matrix3DSetRotationByRadians(matrix, angle * M_PI / 180.0, x, y, z);
}


//�Ѿ���source��ֵ��result
inline void Matrix3DCopy(Matrix3D source,Matrix3D result)
{
	result[0] = source[0] ;result[1] = source[1] ;result[2] = source[2] ;
	result[3] = source[3] ;result[4] = source[4] ;result[5] = source[5] ;
	result[6] = source[6] ;result[7] = source[7] ;result[8] = source[8] ;
	result[9] = source[9] ;result[10] = source[10] ;result[11] = source[11] ;
	result[12] = source[12] ;result[13] = source[13] ;result[14] = source[14] ;
	result[15] = source[15] ;

}

//********************************************************

double GetLength(const Vertex& v1, const Vertex& v2);					// ������������֮���ŷʽ����
double GetArea(const Vertex& v1, const Vertex& v2, const Vertex& v3);	// ����һ�������ε�������ú��׹�ʽ��
Vertex VertexAdd(const Vertex& v1, const MVector& v2);					// �����������		
Vertex VertexScale(const Vertex& v1, double scale);						// ��������
double InnerProduct(const MVector& v1, const MVector& v2);				// �����ڻ�
MVector OuterProduct(const MVector& v1, const MVector& v2);				// �������
MVector VectorNormalize(const MVector& v1);								// ������һ��
double VectorLength(const MVector& v1);									// ���������ĳ���
//float SceneComparison(TwoDScene twod, ThreeDSource threed);


struct VectorCompare
{
	bool operator()(const MVector& lv, const MVector& rv)	// ֻ�ܷ��򣬲��ܴ�С
	{
		//double ip = InnerProduct(lv, rv);				// ���������������Ѿ���һ����
		//if( abs(ip - 1) < MINERROR)						// ����������ͬ				
		//	return false;
		if(lv.x < rv.x)			return true;
		if(lv.x > rv.x)			return false;
		if(lv.y < rv.y)			return true;
		if(lv.y > rv.y)			return false;
		if(lv.z < rv.z)			return true;
		if(lv.z > rv.z)			return false;

		return false;
	}
};

typedef struct RidgeData
{
	int lineType;
	double strength;
	double sharpness;
	std::vector<Vertex> polyline;
}RidgeData;

#endif