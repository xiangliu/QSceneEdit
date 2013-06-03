#ifndef BASESTRUCT_h
#define BASESTRUCT_h

#include "trimesh/Vec.h"
#include "trimesh/Box.h"
#include "trimesh/Color.h"

#include "gl/glaux.h"
#include <string>
using namespace std;

// 包围球
struct BSphere{
	point center;
	float r;
	bool valid;
	BSphere():valid(false){}
};

// 对应obj中的三角面片
struct Face{
	int v[3];
	int vn[3]; // 法向
	int vt[3]; // 纹理

	Face() {}
	Face(const int &v0, const int &v1, const int &v2)
	{ v[0] = v0; v[1] = v1; v[2] = v2; }
	Face(const int *v_)
	{ v[0] = v_[0]; v[1] = v_[1]; v[2] = v_[2]; }
	template <class S> explicit Face(const S &x)
	{ v[0] = x[0];  v[1] = x[1];  v[2] = x[2]; }
	int &operator[] (int i) { return v[i]; }
	const int &operator[] (int i) const { return v[i]; }
	operator const int * () const { return &(v[0]); }
	operator const int * () { return &(v[0]); }
	operator int * () { return &(v[0]); }
	int indexof(int v_) const
	{
		return (v[0] == v_) ? 0 :
			(v[1] == v_) ? 1 :
			(v[2] == v_) ? 2 : -1;
	}
};

//材质信息
struct Materia{
	string name;
	string path;
	float Ka[4]; 
	float Kd[4];
	float Ks[4];
	GLuint TextureId;
};

// obj中的mesh；一个物体可能对应1到多个Mesh
// 需要根据label信息考虑怎么处理
struct SMesh{
	string name;
	int faceStart;
	int faceEnd;
};

typedef struct LightModel{
	string modelName;
	string modelTag;
}LightModel;

#endif
