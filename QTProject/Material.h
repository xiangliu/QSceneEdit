#pragma once
#include <string>
using namespace std;

class Material
{
public:
	Material(void);
	~Material(void);
	// 主要属性
public:
	string picPath; // map_Kd 后跟的地址
	string name; // 唯一标识该纹理的名称
	float Ka[4]; // 环境光
	float Kd[4]; // 漫反射
	float Ks[4]; // 镜面反射

	// 主要方法
public:

};

