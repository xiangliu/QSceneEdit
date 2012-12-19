#ifndef MATH_LIB_H
#define MATH_LIB_H

/***
2012/12/16, 改struct face成mface，因为和BaseStruct.h里的Face 定义相冲突
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

typedef struct MVector    //向量
{
	double x, y, z;
	/*MVector(const MVector& vec)
	{
		this->x = vec.x;
		this->y = vec.y;
		this->z = vec.z;
	}*/
	MVector(double xx, double yy, double zz)    //构造函数
	{
		x = xx;
		y = yy;
		z = zz;
	}
	MVector()         //默认构造函数
	{
		x = y = z = 0;
	}
} MVector;

typedef struct Vertex   //定点
{
	float x, y, z;
	int index;			// 记录在顶点列表中的索引（方便输出）
} Vertex;

//typedef struct Face    //面
//{
//	Face(void) : nverts(0), verts(0) {};
//	int nverts;      //点的个数
//	Vertex **verts;
//	MVector* normal;		// 法向（如果法向过小，则该项为空）
//	Vertex mass;		// 质心
//	double area;		// 面积
//	vector<unsigned int> vertexNum;//记录每个面所包含的点的序号，以便保存
//} Face;


typedef struct MFace    //面
{
	int v[3];
	double area;		// 面积
	Vertex mass;		// 质心
	MVector* normal;		// 法向（如果法向过小，则该项为空）
	MFace() {}
	MFace(const int &v0, const int &v1, const int &v2)
	{ v[0] = v0; v[1] = v1; v[2] = v2; }
	MFace(const int *v_)
	{ v[0] = v_[0]; v[1] = v_[1]; v[2] = v_[2]; }
} MFace;



typedef struct Mesh   //网格
{
	Mesh(void) : verts(0),faces(0) {};
	vector<Vertex> verts;
	vector<MFace> faces;
	MVector vx, vy, vz;	// 三个主轴方向
	int nfaces;
	int nverts;
	BoundingBox mesh_bb; //用于来记录mesh的大小，在摆放的时候需要适当进行缩放
	//added by liu xiang; 用来记录group开始的face序号
	vector<int> groups; 
	float center[3]; // boundingbox中心点
} Mesh;

typedef struct MiniBall
{
	Vertex center;
	float radius;
}MiniBall;


//*********added by liuxiang;用于保存三维场景的数据

//用于保存场景的检索结果
typedef struct SceneMatRes_ *pSceneMatRes;
typedef struct SceneMatRes_ {				// Match Result
	char			name[200];	// file name
	float			similarity ;
}SceneMatRes;

//用于保存单个物体的检索结果
typedef struct ObjectMatRes_ *pObjectMatRes;
typedef struct ObjectMatRes_ {				// Match Result
	char			name[200];	// file name
	float			similarity ;
}ObjectMatRes;

typedef float Matrix3D[16];//每个三维模型包含的4*4矩阵

typedef struct ThreeDScene
{
	unsigned int modelNum;//表示三维模型个数
	vector<float*> matrixP;//每个三维模型包含的4*4矩阵，只管伸缩和移动
	vector<float*> rotationMatrixP;//每个三维模型包含的4*4旋转矩阵
	Matrix3D matrixAll; //为所有模型共同设置的模视矩阵
	Matrix3D rotationMatrixAll; //为所有模型共同设置的模视矩阵
	BoundingBox allMesh_bb; //为所有的Mesh设定一个boundingbox，用来记录最后模型的大小
	float sCenter[3]; //用来表示这个场景包围盒的中心
}ThreeDScene;

//用于保存读取的二维照片分割结果
typedef struct TwoDScene
{
	unsigned int modelNum;//表示被分割模型的总个数
	vector<char*> texture;//每个物体的纹理，其中是按顺序保存的
	float importance[MAXSEGMENTNUMBER]; //用于记录物体的重要程度，重要程度正比于物体大小和反比于相同tag物体个数
	char relationship[MAXSEGMENTNUMBER][MAXSEGMENTNUMBER]; //用来保存物体之间的关系矩阵
	char tag1[MAXSEGMENTNUMBER][30];//用来保存模型对应的标签，即模型的类型，例如桌子椅子等 
	char pictureFilename[200]; //用于保存照片的文件名
}TwoDScene;

//这里的设置和search里有重叠，所以暂时不再使用
//用于读取三维场景分割结果
//typedef struct ThreeDSource
//{
	//unsigned int modelNum;//表示被分割模型的总个数
	//vector<char*> texture;//每个物体的纹理，其中是按顺序保存的
	//float importance[MAXSEGMENTNUMBER]; //用于记录物体的重要程度，重要程度正比于物体大小和反比于相同tag物体个数
	//char relationship[MAXSEGMENTNUMBER][MAXSEGMENTNUMBER]; //用来保存物体之间的关系矩阵,用char的原因是后面可以扩展到26个字母，这样每种关系只用一个字母就可以表示
	//char tag1[MAXSEGMENTNUMBER][30];//用来保存模型对应的标签，即模型的类型，例如桌子椅子等 
//}ThreeDSource;

//用于保存3D模型的材质，copy from 3D search
typedef struct Meterial_ *pMeterial;
typedef struct Meterial_ {
	char			name[100];
	double			r, g, b;
	pMeterial		pointer;
}Meterial;

//将缩放操作设置到矩阵matrix里
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


//将matrix单位矩阵化
//inline void Matrix3DSetIdentity(Matrix3D matrix);
inline void Matrix3DSetIdentity(Matrix3D matrix)
{
	matrix[0] = matrix[5] =  matrix[10] = matrix[15] = 1.0;
	matrix[1] = matrix[2] = matrix[3] = matrix[4] = 0.0;
	matrix[6] = matrix[7] = matrix[8] = matrix[9] = 0.0;
	matrix[11] = matrix[12] = matrix[13] = matrix[14] = 0.0;
}
//将两个矩阵相乘放入result里头
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
//将平移操作设置到矩阵matrix里
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
//将旋转操作设置到矩阵matrix里
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


//把矩阵source赋值给result
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

double GetLength(const Vertex& v1, const Vertex& v2);					// 计算两个顶点之间的欧式距离
double GetArea(const Vertex& v1, const Vertex& v2, const Vertex& v3);	// 计算一个三角形的面积（用海伦公式）
Vertex VertexAdd(const Vertex& v1, const MVector& v2);					// 两个向量相加		
Vertex VertexScale(const Vertex& v1, double scale);						// 向量数乘
double InnerProduct(const MVector& v1, const MVector& v2);				// 向量内积
MVector OuterProduct(const MVector& v1, const MVector& v2);				// 向量外积
MVector VectorNormalize(const MVector& v1);								// 向量归一化
double VectorLength(const MVector& v1);									// 计算向量的长度
//float SceneComparison(TwoDScene twod, ThreeDSource threed);


struct VectorCompare
{
	bool operator()(const MVector& lv, const MVector& rv)	// 只管方向，不管大小
	{
		//double ip = InnerProduct(lv, rv);				// 假设两个向量都已经归一化了
		//if( abs(ip - 1) < MINERROR)						// 两个向量相同				
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