#pragma once
#include <string>
using namespace std;

class Material
{
public:
	Material(void);
	~Material(void);
	// ��Ҫ����
public:
	string picPath; // map_Kd ����ĵ�ַ
	string name; // Ψһ��ʶ�����������
	float Ka[4]; // ������
	float Kd[4]; // ������
	float Ks[4]; // ���淴��

	// ��Ҫ����
public:

};

