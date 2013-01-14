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

//����͵ķ�ʽ����������
int MatchLF1(unsigned char dest_ArtCoeff[SRC_ANGLE][CAMNUM][ART_COEF], 
	unsigned char dest_FdCoeff_q8[ANGLE][CAMNUM][FD_COEF],
	unsigned char dest_CirCoeff_q8[ANGLE][CAMNUM],
	unsigned char dest_EccCoeff_q8[ANGLE][CAMNUM]);

//����͵ķ�ʽ����������
int MatchLF2(unsigned char dest_ArtCoeff[SRC_ANGLE][CAMNUM][ART_COEF], 
	unsigned char dest_FdCoeff_q8[ANGLE][CAMNUM][FD_COEF],
	unsigned char dest_CirCoeff_q8[ANGLE][CAMNUM],
	unsigned char dest_EccCoeff_q8[ANGLE][CAMNUM]);

int search(char* err);

//������������3Dģ��,�����û��ֻ����
int search3DModel(char* err,pObjectMatRes pObjectMatResult);

//ͨ����ά��Ƭ�ָ���������3D����
int Find3DScene(TwoDScene twds,char* err);
//ͬ����������3D����������Ҫ���ļ���ȡ��Ƭ��������
int Find3DSceneFromBuffer(TwoDScene twds,char* err,pSceneMatRes pSceneMatResult);

//ͬ����������3D���������������µ����ݿ��Լ�ʹ���µļ����㷨
int Search3DSceneFromBuffer(TwoDScene twds,int **relationship,char* err,pSceneMatRes pSceneMatResult);
