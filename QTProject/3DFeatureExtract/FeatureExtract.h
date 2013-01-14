/************************************************************************/
/* it's used by 3D feature extract     
the difference between this and FeatureExtract.h is : this file is the 
interface to the out */
/************************************************************************/

#include "ImageSeg/SegObject.h"

void FindCenter(unsigned char *srcBuff, int width, int height, double *CenX, double *CenY);

int FeatureExtractByFile(char* error,CSegObject& object);

//其中coordinate用来代表物体的大致坐标，而diagonal用来代表物体包围盒对角线长度
int FeatureExtract(char* error,CSegObject& object);

//为分割场景提取描述子
int FeatureExtractForPictureScene(char* err, CSegObject &object,int i,char* filename);

//为交互输入的单幅照片提取描述子
int FeatureExtractForPictureObject(char* err, CvvImage& picture);