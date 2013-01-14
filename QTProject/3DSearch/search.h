/************************************************************************/
/* this is used by 3D search                                                                     */
/************************************************************************/

#include "3DSearch/ds.h"
#include "MathLib.h"

void ShowResults(char *filename, pMatRes pTop, int ClassNum);

// matching FightField descriptor ( ART + Fourier )
int MatchLF(unsigned char dest_ArtCoeff[SRC_ANGLE][CAMNUM][ART_COEF], 
	unsigned char dest_FdCoeff_q8[ANGLE][CAMNUM][FD_COEF],
	unsigned char dest_CirCoeff_q8[ANGLE][CAMNUM],
	unsigned char dest_EccCoeff_q8[ANGLE][CAMNUM]);

//计算和的方式，即方案二
int MatchLF1(unsigned char dest_ArtCoeff[SRC_ANGLE][CAMNUM][ART_COEF], 
	unsigned char dest_FdCoeff_q8[ANGLE][CAMNUM][FD_COEF],
	unsigned char dest_CirCoeff_q8[ANGLE][CAMNUM],
	unsigned char dest_EccCoeff_q8[ANGLE][CAMNUM]);

//计算和的方式，即方案二
int MatchLF2(unsigned char dest_ArtCoeff[SRC_ANGLE][CAMNUM][ART_COEF], 
	unsigned char dest_FdCoeff_q8[ANGLE][CAMNUM][FD_COEF],
	unsigned char dest_CirCoeff_q8[ANGLE][CAMNUM],
	unsigned char dest_EccCoeff_q8[ANGLE][CAMNUM]);

int search(char* err);

//用来检索单个3D模型,用于用户手绘检索
int search3DModel(char* err,pObjectMatRes pObjectMatResult);

//通过二维照片分割结果来检索3D场景
int Find3DScene(TwoDScene twds,char* err);
//同样用来检索3D场景，不需要从文件读取照片的描述子
int Find3DSceneFromBuffer(TwoDScene twds,char* err,pSceneMatRes pSceneMatResult);

//同样用来检索3D场景，用来检索新的数据库以及使用新的检索算法
int Search3DSceneFromBuffer(TwoDScene twds,int **relationship,char* err,pSceneMatRes pSceneMatResult);
