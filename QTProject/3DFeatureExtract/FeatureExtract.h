/************************************************************************/
/* it's used by 3D feature extract     
the difference between this and FeatureExtract.h is : this file is the 
interface to the out */
/************************************************************************/

#include "ImageSeg/SegObject.h"

void FindCenter(unsigned char *srcBuff, int width, int height, double *CenX, double *CenY);

int FeatureExtractByFile(char* error,CSegObject& object);

//����coordinate������������Ĵ������꣬��diagonal�������������Χ�жԽ��߳���
int FeatureExtract(char* error,CSegObject& object);

//Ϊ�ָ����ȡ������
int FeatureExtractForPictureScene(char* err, CSegObject &object,int i,char* filename);

//Ϊ��������ĵ�����Ƭ��ȡ������
int FeatureExtractForPictureObject(char* err, CvvImage& picture);