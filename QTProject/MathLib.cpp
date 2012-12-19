#include "MathLib.h"

double GetLength(const Vertex& v1, const Vertex& v2)				// ������������֮���ŷʽ����
{
	double length = 0;
	length += (v1.x - v2.x) * (v1.x - v2.x);
	length += (v1.y - v2.y) * (v1.y - v2.y);
	length += (v1.z - v2.z) * (v1.z - v2.z);
	return sqrt(length);
}

double GetArea(const Vertex& v1, const Vertex& v2, const Vertex& v3)		// ����һ�������ε�������ú��׹�ʽ��
{
	double length1 = GetLength(v1, v2);
	double length2 = GetLength(v2, v3);
	double length3 = GetLength(v3, v1);
	double halfLength = (length1 + length2 + length3) / 2;

	double area = halfLength * (halfLength - length1) * (halfLength - length2) * (halfLength - length3);
	return sqrt(area);
}

Vertex VertexAdd(const Vertex& v1, const MVector& v2)					// �����������
{
	Vertex result;
	result.x = v1.x + v2.x;
	result.y = v1.y + v2.y;
	result.z = v1.z + v2.z;
	return result;
}

Vertex VertexScale(const Vertex& v1, double scale)				// ��������
{
	Vertex result;
	result.x = v1.x * scale;
	result.y = v1.y * scale;
	result.z = v1.z * scale;
	return result;
}

double InnerProduct(const MVector& v1, const MVector& v2)			// �����ڻ�
{
	double result = 0;
	result += v1.x * v2.x;
	result += v1.y * v2.y;
	result += v1.z * v2.z;
	return result;
}

MVector OuterProduct(const MVector& v1, const MVector& v2)			// �������
{
	MVector result;
	result.x = v1.y * v2.z - v1.z * v2.y;
	result.y = v1.z * v2.x - v1.x * v2.z;
	result.z = v1.x * v2.y - v1.y * v2.x;
	return result;
}

MVector VectorNormalize(const MVector& v1)						// ������һ��
{
	MVector result = v1;
	double length = VectorLength(result);
	if(length > 0)
	{
		result.x /= length;
		result.y /= length;
		result.z /= length;
	}
	return result;
}

double VectorLength(const MVector& v1)							// ���������ĳ���
{
	double length = 0;
	length += v1.x * v1.x;
	length += v1.y * v1.y;
	length += v1.z * v1.z;
	length = sqrt(length);
	return length;
}

//inline void Matrix3DSetScaling(Matrix3D matrix, float xScale,
//	float yScale, float zScale)
//{
//	matrix[1] = matrix[2] = matrix[3] = matrix[4] = 0.0;
//	matrix[6] = matrix[7] = matrix[8] = matrix[9] = 0.0;
//	matrix[11] = matrix[12] = matrix[13] = matrix[14] = 0.0;
//	matrix[0] = xScale;
//	matrix[5] = yScale;
//	matrix[10] = zScale;
//	matrix[15] = 1.0;
//}

//inline void Matrix3DSetIdentity(Matrix3D matrix)
//{
//	matrix[0] = matrix[5] =  matrix[10] = matrix[15] = 1.0;
//	matrix[1] = matrix[2] = matrix[3] = matrix[4] = 0.0;
//	matrix[6] = matrix[7] = matrix[8] = matrix[9] = 0.0;
//	matrix[11] = matrix[12] = matrix[13] = matrix[14] = 0.0;
//}

//inline void Matrix3DMultiply(Matrix3D m1, Matrix3D m2, Matrix3D result)
//{
//	result[0] = m1[0] * m2[0] + m1[4] * m2[1] + m1[8] * m2[2] + m1[12] * m2[3];
//	result[1] = m1[1] * m2[0] + m1[5] * m2[1] + m1[9] * m2[2] + m1[13] * m2[3];
//	result[2] = m1[2] * m2[0] + m1[6] * m2[1] + m1[10] * m2[2] + m1[14] * m2[3];
//	result[3] = m1[3] * m2[0] + m1[7] * m2[1] + m1[11] * m2[2] + m1[15] * m2[3];
//
//	result[4] = m1[0] * m2[4] + m1[4] * m2[5] + m1[8] * m2[6] + m1[12] * m2[7];
//	result[5] = m1[1] * m2[4] + m1[5] * m2[5] + m1[9] * m2[6] + m1[13] * m2[7];
//	result[6] = m1[2] * m2[4] + m1[6] * m2[5] + m1[10] * m2[6] + m1[14] * m2[7];
//	result[7] = m1[3] * m2[4] + m1[7] * m2[5] + m1[11] * m2[6] + m1[15] * m2[7];
//
//	result[8] = m1[0] * m2[8] + m1[4] * m2[9] + m1[8] * m2[10] + m1[12] * m2[11];
//	result[9] = m1[1] * m2[8] + m1[5] * m2[9] + m1[9] * m2[10] + m1[13] * m2[11];
//	result[10] = m1[2] * m2[8] + m1[6] * m2[9] + m1[10] * m2[10] + m1[14] * m2[11];
//	result[11] = m1[3] * m2[8] + m1[7] * m2[9] + m1[11] * m2[10] + m1[15] * m2[11];
//
//	result[12] = m1[0] * m2[12] + m1[4] * m2[13] + m1[8] * m2[14] + m1[12] * m2[15];
//	result[13] = m1[1] * m2[12] + m1[5] * m2[13] + m1[9] * m2[14] + m1[13] * m2[15];
//	result[14] = m1[2] * m2[12] + m1[6] * m2[13] + m1[10] * m2[14] + m1[14] * m2[15];
//	result[15] = m1[3] * m2[12] + m1[7] * m2[13] + m1[11] * m2[14] + m1[15] * m2[15];
//}
//inline void Matrix3DSetTranslation(Matrix3D matrix, float xTranslate,
//	float yTranslate, float zTranslate)
//{
//	matrix[0] = matrix[5] =  matrix[10] = matrix[15] = 1.0;
//	matrix[1] = matrix[2] = matrix[3] = matrix[4] = 0.0;
//	matrix[6] = matrix[7] = matrix[8] = matrix[9] = 0.0;
//	matrix[11] = 0.0;
//	matrix[12] = xTranslate;
//	matrix[13] = yTranslate;
//	matrix[14] = zTranslate;
//}


//inline void Matrix3DSetRotationByRadians(Matrix3D matrix, float angle,
//	float x, float y, float z)
//{
//	float mag = sqrtf((x*x) + (y*y) + (z*z));
//	if (mag == 0.0)
//	{
//		x = 1.0;
//		y = 0.0;
//		z = 0.0;
//	}
//	else if (mag != 1.0)
//	{
//		x /= mag;
//		y /= mag;
//		z /= mag;
//	}
//
//	float c = cosf(angle);
//	float s = -sinf(angle);
//	//float s = fastSinf(angle);
//	matrix[3] = matrix[7] = matrix[11] = matrix[12] = matrix[13] = matrix[14] = 0.0;
//	matrix[15] = 1.0;
//
//	matrix[0] = (x*x)*(1-c) + c;
//	matrix[1] = (y*x)*(1-c) + (z*s);
//	matrix[2] = (x*z)*(1-c) - (y*s);
//	matrix[4] = (x*y)*(1-c)-(z*s);
//	matrix[5] = (y*y)*(1-c)+c;
//	matrix[6] = (y*z)*(1-c)+(x*s);
//	matrix[8] = (x*z)*(1-c)+(y*s);
//	matrix[9] = (y*z)*(1-c)-(x*s);
//	matrix[10] = (z*z)*(1-c)+c;
//
//}
//inline void Matrix3DSetRotationByDegrees(Matrix3D matrix, float angle,
//	float x, float y, float z)
//{
//	Matrix3DSetRotationByRadians(matrix, angle * M_PI / 180.0, x, y, z);
//}

/*
inline bool compareTwoTag(char* t1, char* t2)
{
	while(*t1!='\0' && *t2!='\0' )
	{
		if(*t1 != *t2)
		{
			return false ;
		}else
		{
			t1++;
			t2++;
		}
	}

	if(*t1=='\0' && *t2=='\0')
	{
            return true ;
	}
	else
	{
		return false ;
	}
	
}
//���ڱȽ�2D��3D����֮������ƶ�
//����ֵ�����-1������ʾ���3D�������ƶȵ��ڷ�ֵ��������
float SceneComparison(TwoDScene twod, ThreeDSource threed)
{
	int temp = 0;
	float temp1 = 0.0;
	//����tag�Ƚϵĸ������飬Ԥ��ģ�͵ĸ���������100��
	int signTest[100];
	//��ʼ��
	for(int i = 0; i<threed.modelNum;i++)
		signTest[i] = 0;

	//���ڲ�����������֮�����Ƶı�ǩ����
	for(int i = 0; i< twod.modelNum ;i++)
	{
		for(int j = 0; j< threed.modelNum ;j++)
		{
			//��ʾ�����ǩû�б�ʹ���Һ���Ƭ��ǩһ��
			if( (!signTest[j]) && compareTwoTag(twod.tag.at(i),threed.tag.at(j)) )
			{
				temp ++; //���ڼ�¼tagһ�µĸ���
				break;
			}
		}
	}
	//����tag��ض�
	temp1 = (float)temp/(threed.modelNum+twod.modelNum);
	//����tag��ض��Ƿ���ڷ�ֵ��������ڣ�ֱ�ӷ���-1�����ｲ��ֵ����Ϊ0.2
    if(temp1 < 0.2)
	{
		return -1;
	}

	//���³�ʼ��
	for(int i = 0; i<threed.modelNum;i++)
		signTest[i] = 0;
	//���ڲ�����������֮�����Ƶı�ǩ����
	for(int i = 0; i< twod.modelNum ;i++)
	{
		for(int j = 0; j< threed.modelNum ;j++)
		{
			//��ʾ�����ǩû�б�ʹ���Һ���Ƭ��ǩһ��
			if( (!signTest[j]) && compareTwoTag(twod.tag.at(i),threed.tag.at(j)) )
			{
				temp ++; //���ڼ�¼tagһ�µĸ���
				break;
			}
		}
	}
}

*/