/************************************************************************/
/* it's used by 3D feature extract                                                                     */
/************************************************************************/
//#include "../ImageSeg/SegObject.h"
#include "ImageSeg/SegObject.h"

int ReadObj(char *filename, pVer *vertex, pTri *triangle, int *NumVer, int *NumTri);
void SaveObj(char *filename, pVer vertex, pTri triangle, int NumVer, int NumTri);
void SaveMergeObj(char *filename, pVer vertex1, pTri triangle1, int NumVer1, int NumTri1, 
								  pVer vertex2, pTri triangle2, int NumVer2, int NumTri2);
int ReadData(char *filename, pVer *vertex, pTri *triangle, int *NumVer, int *NumTri);
//***************added by liuxiang
int FindTheRect(unsigned char *bmBits,int width, int height,sPOINT* pLU,sPOINT*pRD);
int DoubleToInt(double s);
int ReadPicture(char *filename,unsigned char *bmBits, /*unsigned char *bmColor,*/int w,int h,double *centx,double *centy);
int ProccessPicture(unsigned char *bmBits,unsigned char *NewbmBits,int ow,int oh, int nw,int nh,double *centx,double *centy);
int ScalePicture(unsigned char *bmBits,unsigned char *NewbmBits,int ow,int oh,int nw,int nh,double scale,double *centx,double *centy);
int Read3DFile(char *filename, pVer *vertex, pTri *triangle, int *NumVer, int *NumTri);
int ReadPictureByOpencv(unsigned char *bmBits, /*unsigned char *bmColor,*/int w,int h,CSegObject &object);
void TranslateImgBits(unsigned char *src,unsigned char *dest,int srcW ,int srcH, int destW,int destH, sPOINT* pLU,sPOINT*pRD);
int LoadPicture(unsigned char *bmBits,int w,int h,CvvImage &picture);
//*******************end of added