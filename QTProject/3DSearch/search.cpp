//#include "stdafx.h"
#include "3DSearch/ds.h"
#include "3DSearch/search.h"
#include "MathLib.h"
#include "Scene.h"
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <limits.h>
#include <float.h>
#include <time.h>

#define			DataBaseModelNum    873   //ģ������
//#define         MAXOBJECTNUMBER   50      //�ٶ�����������������Ϊ50��

// #1
unsigned char	q8_table[256][256]; //ÿ��λ�ô洢�ľ����к���֮��

unsigned char	src_ArtCoeff[SRC_ANGLE][CAMNUM][ART_COEF];
unsigned char	src_ArtCoeff1[ART_COEF];     //new added var by liuxiang
unsigned char	dest_ArtCoeff[DataBaseModelNum][SRC_ANGLE][CAMNUM][ART_COEF];

unsigned char	align10[60][CAMNUM_2];
int				cost[SRC_ANGLE][ANGLE][CAMNUM][CAMNUM];

// for Fourier Descriptor matching
unsigned char	src_FdCoeff_q8[ANGLE][CAMNUM][FD_COEF];//һ��srcֻ��һ�����������������ҵ�ģ��
unsigned char	src_FdCoeff_q81[FD_COEF];    //new added var by liuxiang
unsigned char	dest_FdCoeff_q8[DataBaseModelNum][ANGLE][CAMNUM][FD_COEF];

// for Circularity
unsigned char	src_CirCoeff_q8[ANGLE][CAMNUM];
unsigned char	src_CirCoeff_q81;//new added var by liuxiang
unsigned char	dest_CirCoeff_q8[DataBaseModelNum][ANGLE][CAMNUM];

// for Eccentricity������
unsigned char	src_EccCoeff_q8[ANGLE][CAMNUM];
unsigned char	src_EccCoeff_q81;//new added var by liuxiang
unsigned char	dest_EccCoeff_q8[DataBaseModelNum][ANGLE][CAMNUM];


//******************������Ƭ������***********************
unsigned char	src_ArtCoeff2[MAXSEGMENTNUMBER][ART_COEF];     //��������һ����Ƭ�зָ��������е�������
unsigned char	src_FdCoeff_q82[MAXSEGMENTNUMBER][FD_COEF];    //������Ƭ������
unsigned char	src_CirCoeff_q82[MAXSEGMENTNUMBER];            //������Ƭ����
unsigned char	src_EccCoeff_q82[MAXSEGMENTNUMBER];            //������Ƭ����
	
//*******************����3D�����ָ���������******************
unsigned char	dest_ArtCoeff2[MAXSEGMENTNUMBER][SRC_ANGLE][CAMNUM][ART_COEF];
unsigned char	dest_FdCoeff_q82[MAXSEGMENTNUMBER][ANGLE][CAMNUM][FD_COEF];
unsigned char	dest_CirCoeff_q82[MAXSEGMENTNUMBER][ANGLE][CAMNUM];
unsigned char	dest_EccCoeff_q82[MAXSEGMENTNUMBER][ANGLE][CAMNUM];


//abs���������ֵ�ķ���
#define abs(a) ((a)>0)?(a):(-(a))
#define			RESAM		1000


// #1
void ShowResults(char *filename, pMatRes pTop, int ClassNum)
{
	FILE	*fpt;
	int		i;
	//	char	fn[200];

	fpt = fopen(filename, "w");
	if(fpt==NULL)
	{
		printf("can't open the file: s%",filename);
		return;
	}
	fprintf(fpt, "<html>\n<head><title>Results</title></head>\n\n<body>");

	//for(i=0; i<5*ClassNum; i++)
	for(i=0; i<ClassNum; i++)
	{
		fprintf(fpt, "<img src=\"..\\..\\GroundTruthDatabase\\%s.jpg\"> %s %d<br>\n", pTop[i].name, pTop[i].name, pTop[i].sim);
		if( i == ClassNum )
			fprintf(fpt, "\n<hr>\n");
	}

	fprintf(fpt, "\n</body></html>\n");
	fclose(fpt);

	//	sprintf(fn, "\"d:\\program files\\internet explorer\\iexplore.exe \"%s\"\"", filename);
	//	system(fn);
}


// matching FightField descriptor ( ART + Fourier )
int MatchLF(unsigned char dest_ArtCoeff[SRC_ANGLE][CAMNUM][ART_COEF], 
	unsigned char dest_FdCoeff_q8[ANGLE][CAMNUM][FD_COEF],
	unsigned char dest_CirCoeff_q8[ANGLE][CAMNUM],
	unsigned char dest_EccCoeff_q8[ANGLE][CAMNUM])
{
	int						i, j, srcCam, destCam;
	register unsigned char	m;
	int						err, MinErr;
	int						art_distance, fd_distance, cir_distance, ecc_distance;
	static int				cost_q8[ANGLE][ANGLE][CAMNUM][CAMNUM];

	// compare each coefficients pair from the two models first
	for(srcCam=0; srcCam<ANGLE; srcCam++)
		for(destCam=0; destCam<ANGLE; destCam++)
			for(i=0; i<CAMNUM; i++)
				for(j=0; j<CAMNUM; j++)
				{
					// un-loop to speed-up
					// for ART (Zernike moment)
					m=0;
					art_distance = q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff[srcCam][j][m++]];
					art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff[srcCam][j][m++]];
					art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff[srcCam][j][m++]];
					art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff[srcCam][j][m++]];
					art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff[srcCam][j][m++]];
					art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff[srcCam][j][m++]];
					art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff[srcCam][j][m++]];
					art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff[srcCam][j][m++]];
					art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff[srcCam][j][m++]];
					art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff[srcCam][j][m++]];
					// 11	
					art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff[srcCam][j][m++]];
					art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff[srcCam][j][m++]];
					art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff[srcCam][j][m++]];
					art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff[srcCam][j][m++]];
					art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff[srcCam][j][m++]];
					art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff[srcCam][j][m++]];
					art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff[srcCam][j][m++]];
					art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff[srcCam][j][m++]];
					art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff[srcCam][j][m++]];
					art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff[srcCam][j][m++]];
					// 21
					art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff[srcCam][j][m++]];
					art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff[srcCam][j][m++]];
					art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff[srcCam][j][m++]];
					art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff[srcCam][j][m++]];
					art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff[srcCam][j][m++]];
					art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff[srcCam][j][m++]];
					art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff[srcCam][j][m++]];
					art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff[srcCam][j][m++]];
					art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff[srcCam][j][m++]];
					art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff[srcCam][j][m++]];
					// 31
					art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff[srcCam][j][m++]];
					art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff[srcCam][j][m++]];
					art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff[srcCam][j][m++]];
					art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff[srcCam][j][m++]];
					art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff[srcCam][j][m++]];

					// for Fourier Descriptor
					m=0;
					fd_distance = q8_table[dest_FdCoeff_q8[destCam][i][m]][src_FdCoeff_q8[srcCam][j][m++]];
					fd_distance += q8_table[dest_FdCoeff_q8[destCam][i][m]][src_FdCoeff_q8[srcCam][j][m++]];
					fd_distance += q8_table[dest_FdCoeff_q8[destCam][i][m]][src_FdCoeff_q8[srcCam][j][m++]];
					fd_distance += q8_table[dest_FdCoeff_q8[destCam][i][m]][src_FdCoeff_q8[srcCam][j][m++]];
					fd_distance += q8_table[dest_FdCoeff_q8[destCam][i][m]][src_FdCoeff_q8[srcCam][j][m++]];
					fd_distance += q8_table[dest_FdCoeff_q8[destCam][i][m]][src_FdCoeff_q8[srcCam][j][m++]];
					fd_distance += q8_table[dest_FdCoeff_q8[destCam][i][m]][src_FdCoeff_q8[srcCam][j][m++]];
					fd_distance += q8_table[dest_FdCoeff_q8[destCam][i][m]][src_FdCoeff_q8[srcCam][j][m++]];
					fd_distance += q8_table[dest_FdCoeff_q8[destCam][i][m]][src_FdCoeff_q8[srcCam][j][m++]];
					fd_distance += q8_table[dest_FdCoeff_q8[destCam][i][m]][src_FdCoeff_q8[srcCam][j][m++]];
					fd_distance <<= 1;

					// for Circularity
					cir_distance = q8_table[dest_CirCoeff_q8[destCam][i]][src_CirCoeff_q8[srcCam][j]];
					cir_distance <<= 1;

					// for Eccentricity
					ecc_distance = q8_table[dest_EccCoeff_q8[destCam][i]][src_EccCoeff_q8[srcCam][j]];
					//ecc_distance <<= 1;

					cost_q8[srcCam][destCam][i][j] = art_distance + fd_distance + cir_distance + ecc_distance;
				}

				// find minimum error of the two models from all camera pairs
				MinErr = INT_MAX;
				for(srcCam=0; srcCam<SRC_ANGLE; srcCam++)		// each src angle
					for(destCam=0; destCam<ANGLE; destCam++)	// each dest angle
						for(j=0; j<60; j++)						// each align
						{
							//					err = 0;
							//					for(m=0; m<CAMNUM; m++)				// each vertex
							//						err += cost_q8[srcCam][destCam][align10[j][m]][align10[0][m++]];

							// un-loop to speed-up
							m=0;
							err = cost_q8[srcCam][destCam][align10[j][m]][align10[0][m++]];
							err += cost_q8[srcCam][destCam][align10[j][m]][align10[0][m++]];
							err += cost_q8[srcCam][destCam][align10[j][m]][align10[0][m++]];
							err += cost_q8[srcCam][destCam][align10[j][m]][align10[0][m++]];
							err += cost_q8[srcCam][destCam][align10[j][m]][align10[0][m++]];
							err += cost_q8[srcCam][destCam][align10[j][m]][align10[0][m++]];
							err += cost_q8[srcCam][destCam][align10[j][m]][align10[0][m++]];
							err += cost_q8[srcCam][destCam][align10[j][m]][align10[0][m++]];
							err += cost_q8[srcCam][destCam][align10[j][m]][align10[0][m++]];
							err += cost_q8[srcCam][destCam][align10[j][m]][align10[0][m++]];

							//				err += cost_q8[srcCam][destCam][align10[j][m]][align10[0][m++]];
							//				err += cost_q8[srcCam][destCam][align10[j][m]][align10[0][m++]];
							//				err += cost_q8[srcCam][destCam][align10[j][m]][align10[0][m++]];
							//				err += cost_q8[srcCam][destCam][align10[j][m]][align10[0][m++]];
							//				err += cost_q8[srcCam][destCam][align10[j][m]][align10[0][m++]];
							//				err += cost_q8[srcCam][destCam][align10[j][m]][align10[0][m++]];
							//				err += cost_q8[srcCam][destCam][align10[j][m]][align10[0][m++]];
							//				err += cost_q8[srcCam][destCam][align10[j][m]][align10[0][m++]];
							//				err += cost_q8[srcCam][destCam][align10[j][m]][align10[0][m++]];
							//				err += cost_q8[srcCam][destCam][align10[j][m]][align10[0][m++]];

							if( err < MinErr )
								MinErr = err;
						}

						return MinErr;
}

//����͵ķ�ʽ����������
int MatchLF1(unsigned char dest_ArtCoeff[SRC_ANGLE][CAMNUM][ART_COEF], 
	unsigned char dest_FdCoeff_q8[ANGLE][CAMNUM][FD_COEF],
	unsigned char dest_CirCoeff_q8[ANGLE][CAMNUM],
	unsigned char dest_EccCoeff_q8[ANGLE][CAMNUM])
{
	static int				cost_q81[ANGLE][CAMNUM];
	int						i, j, srcCam, destCam;
	register unsigned char	m;
	//int						err, MinErr;
	int                     err1,MinErr1;//added by liuxiang,Ϊ�˼�����Сֵ
	int						art_distance, fd_distance, cir_distance, ecc_distance;
	//static int				cost_q8[ANGLE][ANGLE][CAMNUM][CAMNUM];
	//static int				cost_q81[ANGLE][CAMNUM];

	// compare each coefficients pair from the two models first
	//for(srcCam=0; srcCam<ANGLE; srcCam++)
	for(destCam=0; destCam<ANGLE; destCam++)
		for(i=0; i<CAMNUM; i++)
			//for(j=0; j<CAMNUM; j++)
		{
			// un-loop to speed-up
			// for ART (Zernike moment)
			//q8_table�����������ٵ�
			m=0;
			art_distance = q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			// 11	
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			// 21
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			// 31
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];

			// for Fourier Descriptor
			m=0;
			fd_distance = q8_table[dest_FdCoeff_q8[destCam][i][m]][src_FdCoeff_q81[m++]];
			fd_distance += q8_table[dest_FdCoeff_q8[destCam][i][m]][src_FdCoeff_q81[m++]];
			fd_distance += q8_table[dest_FdCoeff_q8[destCam][i][m]][src_FdCoeff_q81[m++]];
			fd_distance += q8_table[dest_FdCoeff_q8[destCam][i][m]][src_FdCoeff_q81[m++]];
			fd_distance += q8_table[dest_FdCoeff_q8[destCam][i][m]][src_FdCoeff_q81[m++]];
			fd_distance += q8_table[dest_FdCoeff_q8[destCam][i][m]][src_FdCoeff_q81[m++]];
			fd_distance += q8_table[dest_FdCoeff_q8[destCam][i][m]][src_FdCoeff_q81[m++]];
			fd_distance += q8_table[dest_FdCoeff_q8[destCam][i][m]][src_FdCoeff_q81[m++]];
			fd_distance += q8_table[dest_FdCoeff_q8[destCam][i][m]][src_FdCoeff_q81[m++]];
			fd_distance += q8_table[dest_FdCoeff_q8[destCam][i][m]][src_FdCoeff_q81[m++]];
			fd_distance <<= 2;//�㲻�����Ϊʲô���ƣ��ǲ��Ǹ���̫���ˣ�����̫���ˣ���ΪARTû��

			// for Circularity
			cir_distance = q8_table[dest_CirCoeff_q8[destCam][i]][src_CirCoeff_q81];
			cir_distance <<= 1;

			// for Eccentricity
			ecc_distance = q8_table[dest_EccCoeff_q8[destCam][i]][src_EccCoeff_q81];
			//ecc_distance <<= 1;

			cost_q81[destCam][i] = art_distance + fd_distance + cir_distance + ecc_distance;
		}

		// find minimum error of the two models from all camera pairs
		//�˴��޸�Ϊ��һ��Ƕ����ҳ���С�ã����ս�������С�Ķ�������
		err1 = 0;//���ֵ�������ó����ģ�Ҫ��Ȼ���ȽϽ����Ϊ
		MinErr1 = 0;
		//for(srcCam=0; srcCam<SRC_ANGLE; srcCam++)		// each src angle
		for(destCam=0; destCam<ANGLE; destCam++)	// each dest angle
		{
			err1 = cost_q81[destCam][0];
			for(j=1; j<CAMNUM; j++)						// each align������ԭָ����һ��ƥ��
			{

				if(cost_q81[destCam][j]<err1)
				{
					err1 = cost_q81[destCam][j];
				}
			}

			MinErr1 +=err1;

		}

		return MinErr1;
}


//����͵ķ�ʽ����������
int MatchLF2(unsigned char dest_ArtCoeff[SRC_ANGLE][CAMNUM][ART_COEF], 
	unsigned char dest_FdCoeff_q8[ANGLE][CAMNUM][FD_COEF],
	unsigned char dest_CirCoeff_q8[ANGLE][CAMNUM],
	unsigned char dest_EccCoeff_q8[ANGLE][CAMNUM])
{
	static int				cost_q81[ANGLE][CAMNUM];
	int						i, j, srcCam, destCam;
	register unsigned char	m;
	//int						err, MinErr;
	int                     err1,MinErr1;//added by liuxiang,Ϊ�˼�����Сֵ
	int						art_distance, fd_distance, cir_distance, ecc_distance;
	//static int				cost_q8[ANGLE][ANGLE][CAMNUM][CAMNUM];
	//static int				cost_q81[ANGLE][CAMNUM];

	// compare each coefficients pair from the two models first
	//for(srcCam=0; srcCam<ANGLE; srcCam++)
	for(destCam=0; destCam<ANGLE; destCam++)
		for(i=0; i<CAMNUM; i++)
			//for(j=0; j<CAMNUM; j++)
		{
			// un-loop to speed-up
			// for ART (Zernike moment)
			//q8_table�����������ٵ�
			m=0;
			art_distance = q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			// 11	
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			// 21
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			// 31
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[destCam][i][m]][src_ArtCoeff1[m++]];

			// for Fourier Descriptor
			m=0;
			fd_distance = q8_table[dest_FdCoeff_q8[destCam][i][m]][src_FdCoeff_q81[m++]];
			fd_distance += q8_table[dest_FdCoeff_q8[destCam][i][m]][src_FdCoeff_q81[m++]];
			fd_distance += q8_table[dest_FdCoeff_q8[destCam][i][m]][src_FdCoeff_q81[m++]];
			fd_distance += q8_table[dest_FdCoeff_q8[destCam][i][m]][src_FdCoeff_q81[m++]];
			fd_distance += q8_table[dest_FdCoeff_q8[destCam][i][m]][src_FdCoeff_q81[m++]];
			fd_distance += q8_table[dest_FdCoeff_q8[destCam][i][m]][src_FdCoeff_q81[m++]];
			fd_distance += q8_table[dest_FdCoeff_q8[destCam][i][m]][src_FdCoeff_q81[m++]];
			fd_distance += q8_table[dest_FdCoeff_q8[destCam][i][m]][src_FdCoeff_q81[m++]];
			fd_distance += q8_table[dest_FdCoeff_q8[destCam][i][m]][src_FdCoeff_q81[m++]];
			fd_distance += q8_table[dest_FdCoeff_q8[destCam][i][m]][src_FdCoeff_q81[m++]];
			//fd_distance <<= 1;//�㲻�����Ϊʲô���ƣ��ǲ��Ǹ���̫���ˣ�����̫���ˣ���ΪARTû��
			fd_distance <<= 2;

			// for Circularity
			cir_distance = q8_table[dest_CirCoeff_q8[destCam][i]][src_CirCoeff_q81];
			cir_distance <<= 1;

			// for Eccentricity
			ecc_distance = q8_table[dest_EccCoeff_q8[destCam][i]][src_EccCoeff_q81];
			//ecc_distance <<= 1;

			cost_q81[destCam][i] = art_distance + fd_distance + cir_distance + ecc_distance;
		}

		// find minimum error of the two models from all camera pairs
		//�˴��޸�Ϊ��һ��Ƕ����ҳ���С�ã����ս�������С�Ķ�������
		err1 = 0;//���ֵ�������ó����ģ�Ҫ��Ȼ���ȽϽ����Ϊ
		MinErr1 = 0;
		err1 = cost_q81[0][0];
		//for(srcCam=0; srcCam<SRC_ANGLE; srcCam++)		// each src angle
		for(destCam=0; destCam<ANGLE; destCam++)	// each dest angle
		{
			//err1 = cost_q81[destCam][0];
			for(j=1; j<CAMNUM; j++)						// each align������ԭָ����һ��ƥ��
			{

				if(cost_q81[destCam][j]<err1)
				{
					err1 = cost_q81[destCam][j];
				}
			}

			//MinErr1 +=err1;

		}

		return err1;
}

//����͵ķ�ʽ�����������ĸ�д��
//����pNum������Ƭ�ָ����еĵڼ������壬��tNum�����3Dĳ�������ĵ�tNUM������
//ֻ�Ƿ���ֵ��ֵ��С����
float MatchLF3(int pNum, int tNum)
{
	static int				cost_q81[ANGLE][CAMNUM];
	int						i, j, srcCam, destCam;
	register unsigned char	m;
	//int						err, MinErr;
	int                     err1,MinErr1;//added by liuxiang,Ϊ�˼�����Сֵ
	int						art_distance, fd_distance, cir_distance, ecc_distance;
	//static int				cost_q8[ANGLE][ANGLE][CAMNUM][CAMNUM];
	//static int				cost_q81[ANGLE][CAMNUM];
	float res;


	// compare each coefficients pair from the two models first
	//for(srcCam=0; srcCam<ANGLE; srcCam++)
	for(destCam=0; destCam<ANGLE; destCam++)
		for(i=0; i<CAMNUM; i++)
			//for(j=0; j<CAMNUM; j++)
		{
			// un-loop to speed-up
			// for ART (Zernike moment)
			//q8_table�����������ٵ�
			m=0;
			art_distance = q8_table[dest_ArtCoeff2[tNum][destCam][i][m]][src_ArtCoeff2[pNum][m++]];
			art_distance += q8_table[dest_ArtCoeff2[tNum][destCam][i][m]][src_ArtCoeff2[pNum][m++]];
			art_distance += q8_table[dest_ArtCoeff2[tNum][destCam][i][m]][src_ArtCoeff2[pNum][m++]];
			art_distance += q8_table[dest_ArtCoeff2[tNum][destCam][i][m]][src_ArtCoeff2[pNum][m++]];
			art_distance += q8_table[dest_ArtCoeff2[tNum][destCam][i][m]][src_ArtCoeff2[pNum][m++]];
			art_distance += q8_table[dest_ArtCoeff2[tNum][destCam][i][m]][src_ArtCoeff2[pNum][m++]];
			art_distance += q8_table[dest_ArtCoeff2[tNum][destCam][i][m]][src_ArtCoeff2[pNum][m++]];
			art_distance += q8_table[dest_ArtCoeff2[tNum][destCam][i][m]][src_ArtCoeff2[pNum][m++]];
			art_distance += q8_table[dest_ArtCoeff2[tNum][destCam][i][m]][src_ArtCoeff2[pNum][m++]];
			art_distance += q8_table[dest_ArtCoeff2[tNum][destCam][i][m]][src_ArtCoeff2[pNum][m++]];
			// 11	
			art_distance += q8_table[dest_ArtCoeff2[tNum][destCam][i][m]][src_ArtCoeff2[pNum][m++]];
			art_distance += q8_table[dest_ArtCoeff2[tNum][destCam][i][m]][src_ArtCoeff2[pNum][m++]];
			art_distance += q8_table[dest_ArtCoeff2[tNum][destCam][i][m]][src_ArtCoeff2[pNum][m++]];
			art_distance += q8_table[dest_ArtCoeff2[tNum][destCam][i][m]][src_ArtCoeff2[pNum][m++]];
			art_distance += q8_table[dest_ArtCoeff2[tNum][destCam][i][m]][src_ArtCoeff2[pNum][m++]];
			art_distance += q8_table[dest_ArtCoeff2[tNum][destCam][i][m]][src_ArtCoeff2[pNum][m++]];
			art_distance += q8_table[dest_ArtCoeff2[tNum][destCam][i][m]][src_ArtCoeff2[pNum][m++]];
			art_distance += q8_table[dest_ArtCoeff2[tNum][destCam][i][m]][src_ArtCoeff2[pNum][m++]];
			art_distance += q8_table[dest_ArtCoeff2[tNum][destCam][i][m]][src_ArtCoeff2[pNum][m++]];
			art_distance += q8_table[dest_ArtCoeff2[tNum][destCam][i][m]][src_ArtCoeff2[pNum][m++]];
			art_distance += q8_table[dest_ArtCoeff2[tNum][destCam][i][m]][src_ArtCoeff2[pNum][m++]];
			art_distance += q8_table[dest_ArtCoeff2[tNum][destCam][i][m]][src_ArtCoeff2[pNum][m++]];
			// 21
			art_distance += q8_table[dest_ArtCoeff2[tNum][destCam][i][m]][src_ArtCoeff2[pNum][m++]];
			art_distance += q8_table[dest_ArtCoeff2[tNum][destCam][i][m]][src_ArtCoeff2[pNum][m++]];
			art_distance += q8_table[dest_ArtCoeff2[tNum][destCam][i][m]][src_ArtCoeff2[pNum][m++]];
			art_distance += q8_table[dest_ArtCoeff2[tNum][destCam][i][m]][src_ArtCoeff2[pNum][m++]];
			art_distance += q8_table[dest_ArtCoeff2[tNum][destCam][i][m]][src_ArtCoeff2[pNum][m++]];
			art_distance += q8_table[dest_ArtCoeff2[tNum][destCam][i][m]][src_ArtCoeff2[pNum][m++]];
			art_distance += q8_table[dest_ArtCoeff2[tNum][destCam][i][m]][src_ArtCoeff2[pNum][m++]];
			art_distance += q8_table[dest_ArtCoeff2[tNum][destCam][i][m]][src_ArtCoeff2[pNum][m++]];
			art_distance += q8_table[dest_ArtCoeff2[tNum][destCam][i][m]][src_ArtCoeff2[pNum][m++]];
			art_distance += q8_table[dest_ArtCoeff2[tNum][destCam][i][m]][src_ArtCoeff2[pNum][m++]];
			// 31
			art_distance += q8_table[dest_ArtCoeff2[tNum][destCam][i][m]][src_ArtCoeff2[pNum][m++]];
			art_distance += q8_table[dest_ArtCoeff2[tNum][destCam][i][m]][src_ArtCoeff2[pNum][m++]];
			art_distance += q8_table[dest_ArtCoeff2[tNum][destCam][i][m]][src_ArtCoeff2[pNum][m++]];
			art_distance += q8_table[dest_ArtCoeff2[tNum][destCam][i][m]][src_ArtCoeff2[pNum][m++]];
			art_distance += q8_table[dest_ArtCoeff2[tNum][destCam][i][m]][src_ArtCoeff2[pNum][m++]];

			// for Fourier Descriptor
			m=0;
			fd_distance = q8_table[dest_FdCoeff_q82[tNum][destCam][i][m]][src_FdCoeff_q82[pNum][m++]];
			fd_distance += q8_table[dest_FdCoeff_q82[tNum][destCam][i][m]][src_FdCoeff_q82[pNum][m++]];
			fd_distance += q8_table[dest_FdCoeff_q82[tNum][destCam][i][m]][src_FdCoeff_q82[pNum][m++]];
			fd_distance += q8_table[dest_FdCoeff_q82[tNum][destCam][i][m]][src_FdCoeff_q82[pNum][m++]];
			fd_distance += q8_table[dest_FdCoeff_q82[tNum][destCam][i][m]][src_FdCoeff_q82[pNum][m++]];
			fd_distance += q8_table[dest_FdCoeff_q82[tNum][destCam][i][m]][src_FdCoeff_q82[pNum][m++]];
			fd_distance += q8_table[dest_FdCoeff_q82[tNum][destCam][i][m]][src_FdCoeff_q82[pNum][m++]];
			fd_distance += q8_table[dest_FdCoeff_q82[tNum][destCam][i][m]][src_FdCoeff_q82[pNum][m++]];
			fd_distance += q8_table[dest_FdCoeff_q82[tNum][destCam][i][m]][src_FdCoeff_q82[pNum][m++]];
			fd_distance += q8_table[dest_FdCoeff_q82[tNum][destCam][i][m]][src_FdCoeff_q82[pNum][m++]];
			//fd_distance <<= 1;//�㲻�����Ϊʲô���ƣ��ǲ��Ǹ���̫���ˣ�����̫���ˣ���ΪARTû��
			fd_distance <<= 2;

			// for Circularity
			cir_distance = q8_table[dest_CirCoeff_q82[tNum][destCam][i]][src_CirCoeff_q82[pNum]];
			cir_distance <<= 1;

			// for Eccentricity
			ecc_distance = q8_table[dest_EccCoeff_q82[tNum][destCam][i]][src_EccCoeff_q82[pNum]];
			//ecc_distance <<= 1;

			cost_q81[destCam][i] = art_distance + fd_distance + cir_distance + ecc_distance;
		}

		// find minimum error of the two models from all camera pairs
		//�˴��޸�Ϊ��һ��Ƕ����ҳ���С�ã����ս�������С�Ķ�������
		err1 = 0;//���ֵ�������ó����ģ�Ҫ��Ȼ���ȽϽ����Ϊ
		MinErr1 = 0;
		err1 = cost_q81[0][0];
		//for(srcCam=0; srcCam<SRC_ANGLE; srcCam++)		// each src angle
		for(destCam=0; destCam<ANGLE; destCam++)	// each dest angle
		{
			//err1 = cost_q81[destCam][0];
			for(j=1; j<CAMNUM; j++)						// each align������ԭָ����һ��ƥ��
			{

				if(cost_q81[destCam][j]<err1)
				{
					err1 = cost_q81[destCam][j];
				}
			}

			//MinErr1 +=err1;

		}

		//return err1;
		res = 1000/(float)err1;
		return res;
}


//matchLF2�ĸ�д����ΪmatchLF2��ͨ�����ݲ��������м���������������Ҫʱ�䣬���Դη�������Ҫ��������
//�˷�������ƥ��һ���û��ֻ���Ƭ��3Dģ�Ϳ��е�ģ��
//����databaseNum����3Dģ�����ݿ��еĵڼ���ģ��
int MatchLF4(int databaseNum)
{
	static int				cost_q81[ANGLE][CAMNUM];
	int						i, j, srcCam, destCam;
	register unsigned char	m;
	int                     err1,MinErr1;//added by liuxiang,Ϊ�˼�����Сֵ
	int						art_distance, fd_distance, cir_distance, ecc_distance;


	// compare each coefficients pair from the two models first
	//for(srcCam=0; srcCam<ANGLE; srcCam++)
	for(destCam=0; destCam<ANGLE; destCam++)
		for(i=0; i<CAMNUM; i++)
			//for(j=0; j<CAMNUM; j++)
		{
			// un-loop to speed-up
			// for ART (Zernike moment)
			//q8_table�����������ٵ�
			m=0;
			art_distance = q8_table[dest_ArtCoeff[databaseNum][destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[databaseNum][destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[databaseNum][destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[databaseNum][destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[databaseNum][destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[databaseNum][destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[databaseNum][destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[databaseNum][destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[databaseNum][destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[databaseNum][destCam][i][m]][src_ArtCoeff1[m++]];
			// 11	
			art_distance += q8_table[dest_ArtCoeff[databaseNum][destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[databaseNum][destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[databaseNum][destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[databaseNum][destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[databaseNum][destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[databaseNum][destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[databaseNum][destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[databaseNum][destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[databaseNum][destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[databaseNum][destCam][i][m]][src_ArtCoeff1[m++]];
			// 21
			art_distance += q8_table[dest_ArtCoeff[databaseNum][destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[databaseNum][destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[databaseNum][destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[databaseNum][destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[databaseNum][destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[databaseNum][destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[databaseNum][destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[databaseNum][destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[databaseNum][destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[databaseNum][destCam][i][m]][src_ArtCoeff1[m++]];
			// 31
			art_distance += q8_table[dest_ArtCoeff[databaseNum][destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[databaseNum][destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[databaseNum][destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[databaseNum][destCam][i][m]][src_ArtCoeff1[m++]];
			art_distance += q8_table[dest_ArtCoeff[databaseNum][destCam][i][m]][src_ArtCoeff1[m++]];

			// for Fourier Descriptor
			m=0;
			fd_distance = q8_table[dest_FdCoeff_q8[databaseNum][destCam][i][m]][src_FdCoeff_q81[m++]];
			fd_distance += q8_table[dest_FdCoeff_q8[databaseNum][destCam][i][m]][src_FdCoeff_q81[m++]];
			fd_distance += q8_table[dest_FdCoeff_q8[databaseNum][destCam][i][m]][src_FdCoeff_q81[m++]];
			fd_distance += q8_table[dest_FdCoeff_q8[databaseNum][destCam][i][m]][src_FdCoeff_q81[m++]];
			fd_distance += q8_table[dest_FdCoeff_q8[databaseNum][destCam][i][m]][src_FdCoeff_q81[m++]];
			fd_distance += q8_table[dest_FdCoeff_q8[databaseNum][destCam][i][m]][src_FdCoeff_q81[m++]];
			fd_distance += q8_table[dest_FdCoeff_q8[databaseNum][destCam][i][m]][src_FdCoeff_q81[m++]];
			fd_distance += q8_table[dest_FdCoeff_q8[databaseNum][destCam][i][m]][src_FdCoeff_q81[m++]];
			fd_distance += q8_table[dest_FdCoeff_q8[databaseNum][destCam][i][m]][src_FdCoeff_q81[m++]];
			fd_distance += q8_table[dest_FdCoeff_q8[databaseNum][destCam][i][m]][src_FdCoeff_q81[m++]];
			//fd_distance <<= 1;//�㲻�����Ϊʲô���ƣ��ǲ��Ǹ���̫���ˣ�����̫���ˣ���ΪARTû��
			fd_distance <<= 2;

			// for Circularity
			cir_distance = q8_table[dest_CirCoeff_q8[databaseNum][destCam][i]][src_CirCoeff_q81];
			cir_distance <<= 1;

			// for Eccentricity
			ecc_distance = q8_table[dest_EccCoeff_q8[databaseNum][destCam][i]][src_EccCoeff_q81];
			//ecc_distance <<= 1;

			cost_q81[destCam][i] = art_distance + fd_distance + cir_distance + ecc_distance;
		}

		// find minimum error of the two models from all camera pairs
		//�˴��޸�Ϊ��һ��Ƕ����ҳ���С�ã����ս�������С�Ķ�������
		err1 = 0;//���ֵ�������ó����ģ�Ҫ��Ȼ���ȽϽ����Ϊ
		MinErr1 = 0;
		err1 = cost_q81[0][0];
		//for(srcCam=0; srcCam<SRC_ANGLE; srcCam++)		// each src angle
		for(destCam=0; destCam<ANGLE; destCam++)	// each dest angle
		{
			//err1 = cost_q81[destCam][0];
			for(j=1; j<CAMNUM; j++)						// each align������ԭָ����һ��ƥ��
			{

				if(cost_q81[destCam][j]<err1)
				{
					err1 = cost_q81[destCam][j];
				}
			}

			//MinErr1 +=err1;
		}
		return err1;
}
//void main(int argc, char *argv[])
int search(char* err)
{
	// for region shape descriptor
	int				i, j, k, n, m, Count, TopNum, ClassNum, AllClassNum;
	pMatRes			pmr, pTop;
	int				SIM;
	//	double			SIM;
	//	clock_t			start, finish;
	FILE			*fpt, *fpt1,*fpt2;
	char			filename[200], cmpfn[200], srcfn[20][200], destfn[200], c1[200];//srcfn���������û������ģ��
	float			PreciseRecall[RESAM], OneClassPreciseRecall[RESAM], AllClassPreciseRecall[RESAM];
	float			locate, interpol_1, interpol_2;
	double			dtmp;
	clock_t			start, finish;

	// initialize: read camera pair
	fpt = fopen("SearchConfigure\\q8_table", "rb");
	if(fpt == NULL)
	{
		sprintf(err,"Can't open file : q8_table! \n");
		return 0;
	}
	else
	{
		fread(q8_table, sizeof(unsigned char), 65536, fpt);
		fclose(fpt);
	}


	// initialize: read camera pair
	//ò�������ﲻ��Ҫ��
	fpt = fopen("SearchConfigure\\align10.txt", "rb");
	if(fpt == NULL)
	{
		sprintf(err,"Can't open file : align10.txt! \n");
		return 0;
	}
	fread(align10, sizeof(unsigned char), 60*CAMNUM_2, fpt);
	fclose(fpt);

	//**************read feature of all models***************************
	//��ģ�Ϳ�
	fpt = fopen("SearchConfigure\\ModelList.txt", "r");
	if(fpt == NULL)
	{
		sprintf(err,"Can't open file : ModelList.txt! \n");
		return 0;
	}

	Count = 0;
	while( fgets(destfn, 200, fpt) )
	{
		// read coefficient from model 2
		destfn[strlen(destfn)-1] = 0x00;

		//printf("%d:%s's feature is reading", Count,destfn);

		// #1
		sprintf(filename, "%s_q8_v1.8.art", destfn);
		if( (fpt1 = fopen(filename, "rb")) == NULL )				
		{	
			//sprintf(err,"Can't open file : %s! ",filename);
			continue;	
		}
		fread(dest_ArtCoeff[Count], ANGLE * CAMNUM * ART_COEF, sizeof(unsigned char), fpt1);
		fclose(fpt1);

		sprintf(filename, "%s_q8_v1.8.fd", destfn);
		if( (fpt1 = fopen(filename, "rb")) == NULL )				
		{	
			//printf("%s does not exist.\n", filename);	
			continue;	
		}
		fread(dest_FdCoeff_q8[Count], sizeof(unsigned char),  ANGLE * CAMNUM * FD_COEF, fpt1);
		fclose(fpt1);

		sprintf(filename, "%s_q8_v1.8.cir", destfn);
		if( (fpt1 = fopen(filename, "rb")) == NULL )				
		{	
			//printf("%s does not exist.\n", filename);	
			continue;	
		}
		fread(dest_CirCoeff_q8[Count], sizeof(unsigned char),  ANGLE * CAMNUM, fpt1);
		fclose(fpt1);

		sprintf(filename, "%s_q8_v1.8.ecc", destfn);
		if( (fpt1 = fopen(filename, "rb")) == NULL )				
		{	
			//printf("%s does not exist.\n", filename);	
			continue;	
		}
		fread(dest_EccCoeff_q8[Count], sizeof(unsigned char),  ANGLE * CAMNUM, fpt1);
		fclose(fpt1);

		//printf("...Done \n");

		Count ++;
	}
	fclose(fpt);

	//���������ģ�͸���������ֵ��ͬ����ֱ���˳�����
	if( Count != DataBaseModelNum )
	{	
		//printf("The model count is different from ModelNum!!\n");	
		sprintf(err,"The model count is different from ModelNum!");
		return 0;	
	}
	//**************end of read feature of all models***************************
	
	//����ܹ���ģ�Ϳ��������5������ô����ֻ��ʾ�����5��
	if(DataBaseModelNum > MODELSEARCHRESULTNUMBER)
	{
		TopNum = MODELSEARCHRESULTNUMBER;
	}
	else
	{
		TopNum = DataBaseModelNum;		
	}

	//����ƥ�����ı�������
	pmr = (pMatRes) malloc (DataBaseModelNum * sizeof(MatRes));
	pTop = (pMatRes) malloc (TopNum * sizeof(MatRes));
	if(!pmr||!pTop)
	{
		sprintf(err,"buffer allocate failed! \n"); 
		return 0;
	}

	// initialize
	AllClassNum = 0;//�����û�����ģ�͸���

	//compare.txt-->  ÿһ�ж���Ϊһ���ļ�A-->  A��ÿһ����Ϊһ��model
	// read filename of two models
	if( (fpt = fopen("SearchConfigure\\CompareSource.txt", "r")) == NULL )				
	{	
		sprintf(err,"Can't open the file compare.txt");	
		return 0;	
	}

	//ÿ�ζ�ȡcompare.txt�е�һ��
	//while( fscanf(fpt, "%s", cmpfn) != EOF )
	//{
		//sprintf(filename, "SearchConfigure\\%s.txt", cmpfn);
		//fpt1 = fopen(filename, "r");
		//���ٴη���whileѭ��ʱ��classNum�Ѿ������㣬����%s.txt���������Ҫ�����ҵ��Ǹ�modle
		ClassNum = 0;

		//��compare.txt�µĸ������ݶ�ȡ
		//����ٽ������ļ��µ��ļ�����ȡ
		//�����ܸ���������ClassNum֮��
		while( fgets(srcfn[ClassNum], 200, fpt) )
		{   //srcfn��ʾsource file name
			srcfn[ClassNum][strlen(srcfn[ClassNum])-1] = 0x00;
			ClassNum++;
		}
		fclose(fpt);

		AllClassNum += ClassNum;//Ҳ���������ͳ�Ƶ�ʱ���õ���

		//���compare.txt�еø����ļ�������
		//�������û�������
		//printf("")
		//����compare��ÿһ����������ļ����е�ÿ���ļ����м���.
		for(n=0; n<ClassNum; n++)
		{
			//�Ӷ����ļ���С��˵�������û�����ͼƬ������
			// ART
			sprintf(filename, "%s_q8_v1.8.art", srcfn[n]);
			//if( (fpt1 = fopen("SearchSource/5.bmp_q4_v1.8.art", "rb")) == NULL )
			if( (fpt1 = fopen(filename, "rb")) == NULL )			
			{	printf("%s does not exist.\n", filename);	return 0;	}
			fread(src_ArtCoeff1, ART_COEF, sizeof(unsigned char), fpt1);
			fclose(fpt1);
			// FD
			sprintf(filename, "%s_q8_v1.8.fd", srcfn[n]);
			if( (fpt1 = fopen(filename, "rb")) == NULL )			{	printf("%s does not exist.\n", filename);	return 0;	}
			fread(src_FdCoeff_q81, sizeof(unsigned char),  FD_COEF, fpt1);
			fclose(fpt1);
			// CIR
			sprintf(filename, "%s_q8_v1.8.cir", srcfn[n]);
			if( (fpt1 = fopen(filename, "rb")) == NULL )			{	printf("%s does not exist.\n", filename);	return 0;	}
			fread(&src_CirCoeff_q81, sizeof(unsigned char),  1, fpt1);
			fclose(fpt1);
			// ECC
			sprintf(filename, "%s_q8_v1.8.ecc", srcfn[n]);
			if( (fpt1 = fopen(filename, "rb")) == NULL )			{	printf("%s does not exist.\n", filename);	return 0;	}
			fread(&src_EccCoeff_q81, sizeof(unsigned char), 1, fpt1);
			fclose(fpt1);

			// record execute time --- start
			start = clock();

			// FIRST MATCH
			//�����ٴδ�ͬһ���ļ���Ϊʲô��--������Ϊ�˶�ȡ�Ǹ����֣��ñ���
			//���ﴦ�ڵ�����ѭ����
			if((fpt1 = fopen("SearchConfigure\\ModelList.txt", "r"))==NULL)
			{
				sprintf(err,"Can't open the file ModelList.txt");	
				return 0;	
			}

			Count = 0;
			while( fgets(destfn, 200, fpt1) )
			{
				destfn[strlen(destfn)-1] = 0x00;
				// Matching
				// to be 1.0 : 0.55 : 0.48
				//				SIM =	(double)MatchART_q8(dest_ArtCoeff[Count]) + 
				//						(double)MatchFD_q8(dest_FdCoeff_q8[Count]) + 
				//						16 * MatchSH(SHdest_Coeff[Count]) + 
				//						2048 * MatchS3D(S3Ddest_Coeff[Count]);
				//				SIM = MatchART_q8(dest_ArtCoeff[Count]) + MatchFD_q8(dest_FdCoeff_q8[Count]);
				SIM = MatchLF2(dest_ArtCoeff[Count], dest_FdCoeff_q8[Count], dest_CirCoeff_q8[Count], dest_EccCoeff_q8[Count]);

				// add to a list
				strcpy(pmr[Count].name, destfn);
				pmr[Count].sim = SIM;
				pmr[Count].index = Count;

				Count ++;
			}
			fclose(fpt1);

		//	printf("%d \n",Count);

			// sort
			//sort֮��ʼ��
			for(i=0; i<TopNum; i++)
			{
				pTop[i].sim = INT_MAX;
				strcpy(pTop[i].name, "");
			}

			//��ƥ�������ģ�͵ĸ���
			for(k=0; k<DataBaseModelNum; k++)
				for(i=0; i<TopNum; i++)//���Ǻ�pTop�е�ÿ�������бȽϣ������С�����
					if( pmr[k].sim < pTop[i].sim )
					{
						//���TopNum >1
						if(TopNum>1)
						{
							//�����е�pTop��������
							//����TopNum-2Ϊ�����ڶ����������ƶ���������һ��
							for(j=TopNum-2; j>=i; j--)
							{
								strcpy(pTop[j+1].name, pTop[j].name);
								pTop[j+1].sim = pTop[j].sim;
								pTop[j+1].index = pTop[j].index;
							}
							strcpy(pTop[i].name, pmr[k].name);
							pTop[i].sim = pmr[k].sim;
							pTop[i].index = pmr[k].index;
							break;
						}
						else
						{
							strcpy(pTop[i].name, pmr[k].name);
							pTop[i].sim = pmr[k].sim;
							pTop[i].index = pmr[k].index;
							break;
						}

					}

					// record execute time --- end
					finish = clock();
					if( (fpt1 = fopen("SearchResult\\compare_time.txt", "w")) == NULL )				
					{	
						printf("compare_time.txt does not exist.\n", filename);	
					}
					else
					{
						fprintf(fpt1, "%s: %f sec\n", srcfn[n], (double)(finish - start) / CLOCKS_PER_SEC );
						fclose(fpt1);
					}

					if( (fpt2 = fopen("SearchResult\\ModelDisplay.txt", "w")) == NULL )				
					{	
						printf("ModelDisplay.txt does not exist.\n", filename);	
					}
					else
					{
						for(i = 0; i< TopNum;i++)
						{
							fprintf(fpt2, "%s\n",pTop[i].name);
						}
					}
					fclose(fpt2);

					sprintf(filename, "SearchResult\\searchResult.txt");
					fpt1 = fopen(filename, "w");
					if(fpt1==NULL) 
					{
						printf("can't open the file: %s\n",filename);
						return 0;
					}
					fprintf(fpt1, "@@@@@@@@@@@The search result of %s@@@@@@@@@@\n",srcfn[n]);
					//�ѽ��д��searchResult.txt��

					for(i = 0; i< TopNum;i++)
					{
						fprintf(fpt1, "The %d-th mathest result:%s, match rate is:%d\n",i,pTop[i].name,pTop[i].sim );
					}
					fclose(fpt1);

		}

	//}//the while of open"compare.txt"

	fclose(fpt);	
	free(pmr);
	free(pTop);

	return 1;
}

//������������3Dģ�͵ķ���,�����ں�׶��û��ֻ�ģ�͵ļ���
int search3DModel(char* err,pObjectMatRes pObjectMatResult)
{
	// for region shape descriptor
	int				i, j, k, n, m, Count, TopNum, ClassNum, AllClassNum;
	pMatRes			pmr, pTop;
	int				SIM;
	//	double			SIM;
	//	clock_t			start, finish;
	FILE			*fpt, *fpt1,*fpt2;
	char			filename[200], cmpfn[200], srcfn[20][200], destfn[200], c1[200];//srcfn���������û������ģ��
	float			PreciseRecall[RESAM], OneClassPreciseRecall[RESAM], AllClassPreciseRecall[RESAM];
	float			locate, interpol_1, interpol_2;
	double			dtmp;
	clock_t			start, finish;

	// initialize: read camera pair
	fpt = fopen("SearchConfigure\\q8_table", "rb");
	if(fpt == NULL)
	{
		sprintf(err,"Can't open file : q8_table! \n");
		return 0;
	}
	else
	{
		fread(q8_table, sizeof(unsigned char), 65536, fpt);
		fclose(fpt);
	}


	// initialize: read camera pair
	//ò�������ﲻ��Ҫ��
	fpt = fopen("SearchConfigure\\align10.txt", "rb");
	if(fpt == NULL)
	{
		sprintf(err,"Can't open file : align10.txt! \n");
		return 0;
	}
	fread(align10, sizeof(unsigned char), 60*CAMNUM_2, fpt);
	fclose(fpt);

	//**************read feature of all models***************************
	//��ģ�Ϳ�
	fpt = fopen("SearchConfigure\\ModelList.txt", "r");
	if(fpt == NULL)
	{
		sprintf(err,"Can't open file : ModelList.txt! \n");
		return 0;
	}

	Count = 0;
	while( fgets(destfn, 200, fpt) )
	{
		// read coefficient from model 2
		destfn[strlen(destfn)-1] = 0x00;

		// #1
		sprintf(filename, "%s_q8_v1.8.art", destfn);
		if( (fpt1 = fopen(filename, "rb")) == NULL )				
		{	continue;	}
		fread(dest_ArtCoeff[Count], ANGLE * CAMNUM * ART_COEF, sizeof(unsigned char), fpt1);
		fclose(fpt1);

		sprintf(filename, "%s_q8_v1.8.fd", destfn);
		if( (fpt1 = fopen(filename, "rb")) == NULL )				
		{	continue;	}
		fread(dest_FdCoeff_q8[Count], sizeof(unsigned char),  ANGLE * CAMNUM * FD_COEF, fpt1);
		fclose(fpt1);

		sprintf(filename, "%s_q8_v1.8.cir", destfn);
		if( (fpt1 = fopen(filename, "rb")) == NULL )				
		{	continue;	}
		fread(dest_CirCoeff_q8[Count], sizeof(unsigned char),  ANGLE * CAMNUM, fpt1);
		fclose(fpt1);

		sprintf(filename, "%s_q8_v1.8.ecc", destfn);
		if( (fpt1 = fopen(filename, "rb")) == NULL )				
		{	continue;	}
		fread(dest_EccCoeff_q8[Count], sizeof(unsigned char),  ANGLE * CAMNUM, fpt1);
		fclose(fpt1);

		Count ++;
	}
	fclose(fpt);

	//���������ģ�͸���������ֵ��ͬ����ֱ���˳�����
	if( Count != DataBaseModelNum )
	{	
		sprintf(err,"The model count is different from ModelNum!");
		return 0;	
	}
	//**************end of read feature of all models***************************
	
	//����ܹ���ģ�Ϳ��������5������ô����ֻ��ʾ�����5��
	if(DataBaseModelNum > MODELSEARCHRESULTNUMBER)
	{
		TopNum = MODELSEARCHRESULTNUMBER;
	}
	else
	{
		TopNum = DataBaseModelNum;		
	}

	//����ƥ�����ı�������
	pmr = (pMatRes) malloc (DataBaseModelNum * sizeof(MatRes));
	pTop = (pMatRes) malloc (TopNum * sizeof(MatRes));
	if(!pmr||!pTop)
	{
		sprintf(err,"buffer allocate failed! \n"); 
		return 0;
	}


	//*************����ǰ�׶���Ƭ��ȡ�б����������е������ӽ��м���
	//1.��3Dģ�����ݿ��б�
	//2.ÿ�ζ�ȡһ��ģ�ͣ�����Ƭģ����֮ƥ�䣬����ƥ����
	//3.��ƥ�������򣬱���ƥ�����ߵ�ģ��

	if((fpt1 = fopen("SearchConfigure\\ModelList.txt", "r"))==NULL)
	{
		sprintf(err,"Can't open the file ModelList.txt");	
		return 0;	
	}

	Count = 0;
	while( fgets(destfn, 200, fpt1) )
	{
		destfn[strlen(destfn)-1] = 0x00;
		// Matching
		// to be 1.0 : 0.55 : 0.48
		//				SIM =	(double)MatchART_q8(dest_ArtCoeff[Count]) + 
		//						(double)MatchFD_q8(dest_FdCoeff_q8[Count]) + 
		//						16 * MatchSH(SHdest_Coeff[Count]) + 
		//						2048 * MatchS3D(S3Ddest_Coeff[Count]);
		//				SIM = MatchART_q8(dest_ArtCoeff[Count]) + MatchFD_q8(dest_FdCoeff_q8[Count]);
		SIM = MatchLF4(Count);

		// add to a list
		strcpy(pmr[Count].name, destfn);
		pmr[Count].sim = SIM;
		pmr[Count].index = Count;

		Count ++;
	}
	fclose(fpt1);


	// sort,�������������
	//sort֮��ʼ��
	for(i=0; i<TopNum; i++)
	{
		pTop[i].sim = INT_MAX;
		strcpy(pTop[i].name, "");
	}

	//��ƥ�������ģ�͵ĸ���
	for(k=0; k<DataBaseModelNum; k++)
		for(i=0; i<TopNum; i++)//���Ǻ�pTop�е�ÿ�������бȽϣ������С�����
			if( pmr[k].sim < pTop[i].sim )
			{
				//���TopNum >1
				if(TopNum>1)
				{
					//�����е�pTop��������
					//����TopNum-2Ϊ�����ڶ����������ƶ���������һ��
					for(j=TopNum-2; j>=i; j--)
					{
						strcpy(pTop[j+1].name, pTop[j].name);
						pTop[j+1].sim = pTop[j].sim;
						pTop[j+1].index = pTop[j].index;
					}
					strcpy(pTop[i].name, pmr[k].name);
					pTop[i].sim = pmr[k].sim;
					pTop[i].index = pmr[k].index;
					break;
				}
				else
				{
					strcpy(pTop[i].name, pmr[k].name);
					pTop[i].sim = pmr[k].sim;
					pTop[i].index = pmr[k].index;
					break;
				}

			}


	//������������浽doc�ı�����ͷ
	for(int i = 0; i< MODELSEARCHRESULTNUMBER;i++)
	{
		strcpy(pObjectMatResult[i].name,pTop[i].name);
	}

	fclose(fpt);	
	free(pmr);
	free(pTop);

	return 1;
}

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

//���ڶ���Ƭ�ָ����Լ���ȡ�������ӣ�����ƥ���3D����
//1.�ȶ�ȡ��Ƭ�����и�������������
//2.��ȡ�����б���һ��������������
//3.���бȽϣ������ƶȱ���
//4.����2����
//5.(�ͷ��¿����ڴ�)
int Find3DScene(TwoDScene twds,char* err)
{
	// for region shape descriptor
	int			    TopNum;
	pMatRes		    pTop;
	FILE			*fpt, *fpt1,*fpt2;
	char			filename[200],destfn[200];//srcfn���������û������ģ��

	////******************������Ƭ������***********************
	//unsigned char	src_ArtCoeff2[MAXSEGMENTNUMBER][ART_COEF];     //��������һ����Ƭ�зָ��������е�������
	//unsigned char	src_FdCoeff_q82[MAXSEGMENTNUMBER][FD_COEF];    //������Ƭ������
	//unsigned char	src_CirCoeff_q82[MAXSEGMENTNUMBER];            //������Ƭ����
	//unsigned char	src_EccCoeff_q82[MAXSEGMENTNUMBER];            //������Ƭ����
	//
	////*******************����3D�����ָ���������******************
	//unsigned char	dest_ArtCoeff2[MAXSEGMENTNUMBER][SRC_ANGLE][CAMNUM][ART_COEF];
	//unsigned char	dest_FdCoeff_q82[MAXSEGMENTNUMBER][ANGLE][CAMNUM][FD_COEF];
	//unsigned char	dest_CirCoeff_q82[MAXSEGMENTNUMBER][ANGLE][CAMNUM];
	//unsigned char	dest_EccCoeff_q82[MAXSEGMENTNUMBER][ANGLE][CAMNUM];


	//ThreeDSource threeds;  //���������ȡ��3D����
	char temp2[400]; //����������ʱ��ȡ��scene.txt��ÿһ������
	int temp3,temp4,temp5; //��ʱ����
	char* next;
	char* next1;
	char threedTag[MAXSEGMENTNUMBER][30]; //����������ά��������tag�ı������ٶ��������������50����tag���Ȳ�����30
	int tagFlag[MAXSEGMENTNUMBER]; //����tag�Ƚ�ʱ�����ʱ����
	char relatinoship[MAXSEGMENTNUMBER][MAXSEGMENTNUMBER]; //������ʱ������ά����������֮���relationship���ٶ����岻����50��
	int edgeCount[10]; //������¼ĳ������������relationship�ĸ��� �������飬��Ϊ�ܹ���8��relationship
	float tempSimilarity; //������ʱ�洢��������֮������ƶ�
	float tempObjSimilarity; //������ʱ�洢��������֮������ƶ�
	MatResTemp matresT[MAXSEGMENTNUMBER][10]; //������¼ÿ�������Ӧ��һ����������ͬtag��������ƶ�,ͬ���ٶ����岻����50����ÿ��������ͬ��ǩ�ĸ���������10��
	int sameTagCount[MAXSEGMENTNUMBER]; //��¼ÿ�������ڱ�����3D�����������Ʊ�ǩ���������
	int modelNumber ; //������¼�������ָ��Ժ��������
	float importance[MAXSEGMENTNUMBER]; //���ڼ�¼�������Ҫ�̶ȣ���Ҫ�̶������������С�ͷ�������ͬtag�������

	//****************����similarity�и�������֮���Ȩ��*******************
	float a = 0.5; //������ʾ��������geometry���ƶ�֮���Ȩ��
	float b = 0.5; //������ʾ��������<edge,partner>���ƶ�֮���Ȩ��
	float c = 0.5; //������ʾ��������<edge,partner>���ƶ���edge��Ȩ��
	float d = 0.5; //������ʾ��������<edge,partner>���ƶ���partner��Ȩ��
	//float e = 0.5; //������ʾ������Ҫ�̶���count���ֵ�Ȩ��
	//float f = 0.5; //������ʾ������Ҫ�̶���size���ֵ�Ȩ��

	pTop = (pMatRes) malloc (10 * sizeof(MatRes));  //����ƥ�����ı�������,�ݶ�ѡ��10��

	// initialize: read camera pair
	fpt = fopen("SearchConfigure\\q8_table", "rb");
	if(fpt == NULL)
	{
		sprintf(err,"Can't open file : q8_table! \n");
		return 0;
	}
	else
	{
		fread(q8_table, sizeof(unsigned char), 65536, fpt);
		fclose(fpt);
	}


	// initialize: read camera pair
	//ò�������ﲻ��Ҫ��
	fpt = fopen("SearchConfigure\\align10.txt", "rb");
	if(fpt == NULL)
	{
		sprintf(err,"Can't open file : align10.txt! \n");
		return 0;
	}
	fread(align10, sizeof(unsigned char), 60*CAMNUM_2, fpt);
	fclose(fpt);

	//*********************����Ƭ�ķָ�����������******************************/
	int temp1 = twds.modelNum;

	if(temp1>MAXSEGMENTNUMBER)
	{
		err = "The segement number is max than MAXSEGMENTNUMBER!";
	}

	for(int i=0; i<temp1; i++)
	{
		//�Ӷ����ļ���С��˵�������û�����ͼƬ������
		// ART
		sprintf(filename, "%s_%d.bmp_q8_v1.8.art", twds.pictureFilename,i);
		//if( (fpt1 = fopen("SearchSource/5.bmp_q4_v1.8.art", "rb")) == NULL )
		if( (fpt1 = fopen(filename, "rb")) == NULL )			
		{	printf("%s does not exist.\n", filename);	return 0;	}
		fread(src_ArtCoeff2[i], sizeof(unsigned char),ART_COEF, fpt1);
		fclose(fpt1);
		// FD
		sprintf(filename, "%s_%d.bmp_q8_v1.8.fd", twds.pictureFilename,i);
		if( (fpt1 = fopen(filename, "rb")) == NULL )			{	printf("%s does not exist.\n", filename);	return 0;	}
		fread(src_FdCoeff_q82[i], sizeof(unsigned char),  FD_COEF, fpt1);
		fclose(fpt1);
		// CIR
		sprintf(filename, "%s_%d.bmp_q8_v1.8.cir", twds.pictureFilename,i);
		if( (fpt1 = fopen(filename, "rb")) == NULL )			{	printf("%s does not exist.\n", filename);	return 0;	}
		fread(&src_CirCoeff_q82[i], sizeof(unsigned char),  1, fpt1);
		fclose(fpt1);
		// ECC
		sprintf(filename, "%s_%d.bmp_q8_v1.8.ecc", twds.pictureFilename,i);
		if( (fpt1 = fopen(filename, "rb")) == NULL )			{	printf("%s does not exist.\n", filename);	return 0;	}
		fread(&src_EccCoeff_q82[i], sizeof(unsigned char), 1, fpt1);
		fclose(fpt1);
	}


	//**************��ȡ3D���������г����������Ӳ�����ƥ��***************************/
    //����˳��
	//1.��ȡ�������г����б�
	//2.���Ⱥ�˳���ȡ�����б��еĳ������ȶ�ȡ������Ϣscene.txt
	//3.�ȱȽ�scene.txt����Ƭ������tag�غ϶ȣ�����غ϶ȴ��ڷ�ֵ�����������򷵻�2��ȡ��һ������
	//4.�ٱȽ�������������ͬ��ǩ��relationship��partner��������ͬ��ǩ�Ķ��������Ҫ���ã�
	//5.�����յ����ƶȱ��棬����2
	//6.����

	//��һ�����򿪳����б�
	fpt = fopen("SearchConfigure\\SceneModelList.txt", "r");
	if(fpt == NULL)
	{
		sprintf(err,"Can't open file : SceneModelList.txt! \n");
		return 0;
	}

	//**********************�ڶ�����ѭ����ȡ�����б��еĳ�����Ϣ************
	TopNum = 0; //��¼�Ѿ��������ĳ����ĸ���

	while( fgets(destfn, 200, fpt) )
	{
		//���������ַ������Ͻ�����
		destfn[strlen(destfn)-1] = 0x00;

		//�޸��ļ�����������������.txt,������ȡ������Ϣ
		sprintf(filename, "%s.txt", destfn);

		//����޷���ȡ�����ȡ��һ������
		if( (fpt1 = fopen(filename, "rb")) == NULL )				
		{	continue;	}

		//**********************�ڶ���*******************
		//��ȡ3d�����е���Ϣscene.txt
		tempSimilarity = 0;
		while( fgets(temp2, LINE_MAX_LEN, fpt1) != NULL ) 
		{
			//���˵�ÿ���еĿ�ͷ�հ��ַ�
			for ( next = temp2; *next != '\0' && isspace(*next); next++ )
				;
			
			//���˵�ע�����Լ��հ׺�
			if ( *next == '\0' || *next == '#')
				continue;

			//�����ָ��������,��@total number��ʼ�������һ��Ӧ�ý���ģ�͸���
			if(*next =='@'&& *(next+1)=='t'&& *(next+2)=='o')
			{
				fgets(temp2, LINE_MAX_LEN, fpt);
				next = temp2;
				sscanf ( next, "%d",&modelNumber);
				continue;
			}
			
			//�����ָ������ǩ,��@tag��ʼ
			//�����һ��Ӧ�ý�����ǩ����ǩ֮�䲻�ܼ��
			if(*next =='@'&& *(next+1)=='t'&& *(next+2)=='a')
			{
				temp4 = 0;
				while(fgets(temp2, LINE_MAX_LEN, fpt)!=NULL && *temp2 == '\0')
				{
					sscanf (temp2, "%d %s",&temp3,threedTag[temp4]);
					temp4 ++;
				}

				//��������ı�ǩ�����������һ�£�˵������������
				if(temp4!=modelNumber) return 0;

				//ֱ�ӽ�����һ��ѭ��
				continue;
			}

			//��ȡ�����������relationship����
			//��@edge��ͷ
			//�����һ��Ӧ�ý���edge���󣬱�ǩ֮�䲻�ܼ��
			if(*next =='@'&& *(next+1)=='e'&& *(next+2)=='d')
			{
				temp4 = 0; //������¼��
				while(fgets(temp2, LINE_MAX_LEN, fpt)&& *temp2 == '\0')
				{
					next1 = temp2;
					temp5 = 0; //������¼��
					while(*next1!= '\0')
					{
						if(*next1==' ')
						{
							continue;
						}
						else
						{
							relatinoship[temp4][temp5] = *next1;
							temp5++;
						}
					}
					
					//���relationship����������ģ��������ô������ڴ���
					if(temp5!= modelNumber) return 0;
					temp4 ++;
				}

				//���relationship����������ģ��������ô������ڴ���
				if(temp4!= modelNumber) return 0;

				//ֱ�ӽ�����һ��ѭ��
				continue;
			}//if

			//********************������***********************
			//�Ƚ���Ƭ������3D����tag֮������ƶ�
			//���ڲ�����������֮�����Ƶı�ǩ����
			for(int i = 0; i< modelNumber; i++) //��ʼ������
			{
				tagFlag[i] = 0;
			}
			temp3 = 0;
			//tag �ȶ�
			for(int i = 0; i< twds.modelNum ;i++)
			{
				for(int j = 0; j< modelNumber ;j++)
				{
					//��ʾ�����ǩû�б�ʹ���Һ���Ƭ��ǩһ��
					if( (!tagFlag[j]) && compareTwoTag(twds.tag1[i],threedTag[j]) )
					{
						temp3 ++; //���ڼ�¼tagһ�µĸ���
						tagFlag[j] =1; //˵����j��tag�Ѿ���ʹ�ã��������ж����ͬtag�����
						break;
					}
				}
			}
			//����tag��ض�
			temp1 = (float)temp3/(modelNumber+twds.modelNum);
			//����tag��ض��Ƿ���ڷ�ֵ��������ڣ�ֱ�ӷ���-1�����ｲ��ֵ����Ϊ0.2
			if(temp1 < 0.2)
			{
				return 0;
			}

			//********���tag���ƶȳ�����ֵ����ʼ��ȡ3D�������ָ������������
			temp1 = modelNumber;
			for(int i=0; i<temp1; i++)
			{
				//�Ӷ����ļ���С��˵�������û�����ͼƬ������
				// ART
				sprintf(filename, "%s_%d.obj_q8_v1.8.art", destfn,i);
				//if( (fpt1 = fopen("SearchSource/5.bmp_q4_v1.8.art", "rb")) == NULL )
				if( (fpt2 = fopen(filename, "rb")) == NULL )			
				{	printf("%s does not exist.\n", filename);	return 0;	}
				fread(dest_ArtCoeff2[i], ANGLE * CAMNUM * ART_COEF, sizeof(unsigned char), fpt2);
				fclose(fpt2);
				// FD
				sprintf(filename, "%s_%d.bmp_q8_v1.8.fd", twds.pictureFilename,i);
				if( (fpt2 = fopen(filename, "rb")) == NULL )			{	printf("%s does not exist.\n", filename);	return 0;	}
				fread(dest_FdCoeff_q82[i], sizeof(unsigned char),  ANGLE * CAMNUM * FD_COEF, fpt2);
				fclose(fpt2);
				// CIR
				sprintf(filename, "%s_%d.bmp_q8_v1.8.cir", twds.pictureFilename,i);
				if( (fpt2 = fopen(filename, "rb")) == NULL )			{	printf("%s does not exist.\n", filename);	return 0;	}
				fread(dest_CirCoeff_q82[i], sizeof(unsigned char),  ANGLE * CAMNUM, fpt2);
				fclose(fpt2);
				// ECC
				sprintf(filename, "%s_%d.bmp_q8_v1.8.ecc", twds.pictureFilename,i);
				if( (fpt2 = fopen(filename, "rb")) == NULL )			{	printf("%s does not exist.\n", filename);	return 0;	}
				fread(dest_EccCoeff_q82[i], sizeof(unsigned char),  ANGLE * CAMNUM, fpt2);
				fclose(fpt2);
			}

			//���ڽ���¼��Ƭ��ÿ��������ͬ��ǩ�����������ʼ��
			for(int i = 0; i< MAXSEGMENTNUMBER; i++)
			{    sameTagCount[i] = 0;}

			//������Ƭ��ÿ�����壬����Ѱ�Ҳ��Ƚ���3D��������ͬ��ǩ�����geometry���ƶ�
			for(int i = 0; i< twds.modelNum ;i++)
			{
				for(int j = 0; j< modelNumber ;j++)
				{
					//��һ���ҵ���ǩ
					if(compareTwoTag(twds.tag1[i],threedTag[j]) )
					{
						matresT[i][sameTagCount[i]].objectNum = j;
						matresT[i][sameTagCount[i]].similarity_G = MatchLF3(i,j);
						sameTagCount[i]++;

					}
				}
			}

			//********************���Ĳ�***********************
			//�Ƚ�������������ͬ��ǩ��relationship��partner���ƶ�
			//������ʵ������һ����Զ�����⣬��ʱ�Ȳ����ǣ��򻯴���
			//�����������tag����ͬ����ô������Ϊ����֮������ƶ�Ϊ0���������ҵ����Ƶ�tag�����û�У�ֱ������
			//1.������Ƭ��ÿ�����壬�ҵ�����ͬ��ǩ������
			//2.���ڸ����������е�����<edge,partner>;���Ҷ�Ӧ������ͬ��<edge,partner>
			//2.����ͬ��ϵ��pair��pair֮���edge��Ϊ����
			//3.�����texture��geometry
			for(int i = 0; i< modelNumber; i++) //��ʼ������
			{
				tagFlag[i] = 0;
			}
			for(int i = 0; i< 10; i++)
			{
				edgeCount[i] = 0;
			}

			for(int i = 0; i< twds.modelNum; i++)
			{
				tempObjSimilarity = 0.0;
				
				//����������ڱ���������������ͬ��ǩ,�ٶ���ǰ����Ϊi
				if(!sameTagCount[i])
				{				
				    //1.������iͬtag��3D����j
					for(int j=0; j<sameTagCount[i];j++)
					{
						//2.��ʾ��������i,j��geometry��texture��tag���ƶ�,����aΪȨ��
						tempObjSimilarity += a*matresT[i][j].similarity_G;

						//3.����i��j���г���Ϊ2��relationship���Ƚ����ƶ�
						for(int k = 0; k<twds.modelNum ;k ++)
						{
							if(twds.relationship[i][k]!='0')
							{
								for(int l = 0; l<modelNumber; l++)
								{
									if(twds.relationship[i][k]==relatinoship[j][l])
									    tempObjSimilarity +=c*1;
									//���k��l��tag��ͬ
									if(!sameTagCount[k])
									{
										for(int n = 0; n <sameTagCount[k];n++)
										{
											if(matresT[k][n].objectNum == l)
											{
												tempObjSimilarity +=d*matresT[k][n].similarity_G;
											}
										}
									}//if
								}//for
							}//for

						}//for
							//��ƥ��������i��j����Ҫ�̶ȣ�Ȼ���ۼӵ�tempSimilarity
						tempObjSimilarity = tempObjSimilarity*twds.importance[i]*importance[j];
						tempSimilarity += tempObjSimilarity;
					}//for
				
				}//if
				
			}//for

			//�����ǰ�������Ľ��С��10������++������topnum����
			if(TopNum < 9)
			{
				pTop[TopNum].Precise = tempSimilarity;
				strcpy(pTop[TopNum].name, destfn);
				TopNum ++;
			}
			else if(TopNum == 9)
			{
				pTop[TopNum].Precise = tempSimilarity;
				strcpy(pTop[TopNum].name, destfn);
				TopNum ++;

				int l;
				float maxTemp;
				char tempFilename1[200];
				//������Ÿ��������ÿ���ѭ�������
				for (int i=0;i<9;i++)
				{
					l= i;
					maxTemp = pTop[0].Precise ;

					for(int j = i+1; j<10; j++)
					{
						if(pTop[j].Precise >maxTemp)
						{
							maxTemp = pTop[j].Precise;
							l = j;
						}
					}
					strcpy(tempFilename1,pTop[l].name);
					pTop[l].Precise = pTop[i].Precise;
					strcpy(pTop[l].name,pTop[i].name);
					pTop[i].Precise = maxTemp;
					strcpy(pTop[i].name,tempFilename1);

				}

			}
			else //��ptop����������ʼ����
			{
				for(int i = 9; i>0;i--)
				{
					if(pTop[i].Precise>=tempSimilarity)
					{
						if(i==9)
							break;
						else
						{
							//����������
							for(int j = 9; j<i-1;j--)
							{
								pTop[j].Precise = pTop[j-1].Precise;
								strcpy(pTop[j-1].name,pTop[j].name);
							}
							pTop[i-1].Precise = tempSimilarity;
							strcpy(pTop[i-1].name,destfn);
						}
					}
					else
					{

					}

				}//for
			}//else
				
			
		}//while -- 3D����list���ĳ������

		//�ر�����������ȡĳ��2D�������ļ���
		fclose(fpt1);

	}//while -- 3D����list������г���

	//���������������ļ�
	if( (fpt2 = fopen("SearchResult\\SceneDisplay.txt", "w")) == NULL )				
	{	
		printf("ModelDisplay.txt does not exist.\n", filename);	
	}
	else
	{
		for(int i = 0; i< TopNum;i++)
		{
			fprintf(fpt2, "%s\n",pTop[i].name);
		}
	}
	fclose(fpt2);

	fclose(fpt);	
	free(pTop);

	return 1;
}

//�ú�������Ҫ���ļ���ȡ��Ƭ��������
int Find3DSceneFromBuffer(TwoDScene twds,char* err,pSceneMatRes pSceneMatResult)
{
	// for region shape descriptor
	int			    TopNum;
	pMatRes		    pTop; //����ָ���������ƥ���
	FILE			*fpt, *fpt1,*fpt2;
	char			filename[200],destfn[200];//srcfn���������û������ģ��

	////******************������Ƭ������***********************
	//unsigned char	src_ArtCoeff2[MAXSEGMENTNUMBER][ART_COEF];     //��������һ����Ƭ�зָ��������е�������
	//unsigned char	src_FdCoeff_q82[MAXSEGMENTNUMBER][FD_COEF];    //������Ƭ������
	//unsigned char	src_CirCoeff_q82[MAXSEGMENTNUMBER];            //������Ƭ����
	//unsigned char	src_EccCoeff_q82[MAXSEGMENTNUMBER];            //������Ƭ����
	//
	////*******************����3D�����ָ���������******************
	//unsigned char	dest_ArtCoeff2[MAXSEGMENTNUMBER][SRC_ANGLE][CAMNUM][ART_COEF];
	//unsigned char	dest_FdCoeff_q82[MAXSEGMENTNUMBER][ANGLE][CAMNUM][FD_COEF];
	//unsigned char	dest_CirCoeff_q82[MAXSEGMENTNUMBER][ANGLE][CAMNUM];
	//unsigned char	dest_EccCoeff_q82[MAXSEGMENTNUMBER][ANGLE][CAMNUM];

	char temp2[400]; //����������ʱ��ȡ��scene.txt��ÿһ������
	int temp3,temp5; //��ʱ����
	float temp4;
	char* next;
	char* next1;
	char threedTag[MAXSEGMENTNUMBER][30]; //����������ά��������tag�ı������ٶ��������������50����tag���Ȳ�����30
	int tagFlag[MAXSEGMENTNUMBER]; //����tag�Ƚ�ʱ�����ʱ����
	char relatinoship[MAXSEGMENTNUMBER][MAXSEGMENTNUMBER]; //������ʱ������ά����������֮���relationship���ٶ����岻����50��
	int edgeCount[10]; //������¼ĳ������������relationship�ĸ��� �������飬��Ϊ�ܹ���8��relationship
	float tempSimilarity; //������ʱ�洢��������֮������ƶ�
	float tempObjSimilarity; //������ʱ�洢��������֮������ƶ�
	MatResTemp matresT[MAXSEGMENTNUMBER][10]; //������¼ÿ�������Ӧ��һ����������ͬtag��������ƶ�,ͬ���ٶ����岻����50����ÿ��������ͬ��ǩ�ĸ���������10��
	int sameTagCount[MAXSEGMENTNUMBER]; //��¼ÿ�������ڱ�����3D�����������Ʊ�ǩ���������
	int modelNumber; //���ڼ�¼���ָ���ά�������������
	float importance[MAXSEGMENTNUMBER]; //���ڼ�¼�������Ҫ�̶ȣ���Ҫ�̶������������С�ͷ�������ͬtag�������
	vector<int> temp6; //��������

	//****************����similarity�и�������֮���Ȩ��*******************
	float a = 0.5; //������ʾ��������geometry���ƶ�֮���Ȩ��
	float b = 0.5; //������ʾ��������<edge,partner>���ƶ�֮���Ȩ��
	float c = 0.25; //������ʾ��������<edge,partner>���ƶ���edge��Ȩ��
	float d = 0.25; //������ʾ��������<edge,partner>���ƶ���partner��Ȩ��
	//float e = 0.5; //������ʾ������Ҫ�̶���count���ֵ�Ȩ��
	//float f = 0.5; //������ʾ������Ҫ�̶���size���ֵ�Ȩ��

	pTop = (pMatRes) malloc (SCENEFORDISPLAY * sizeof(MatRes));  //����ƥ�����ı�������,�ݶ�ѡ��10��

	// initialize: read camera pair
	fpt = fopen("SearchConfigure\\q8_table", "rb");
	if(fpt == NULL)
	{
		sprintf(err,"Can't open file : q8_table! \n");
		return 0;
	}
	else
	{
		fread(q8_table, sizeof(unsigned char), 65536, fpt);
		fclose(fpt);
	}


	// initialize: read camera pair
	//ò�������ﲻ��Ҫ��
	fpt = fopen("SearchConfigure\\align10.txt", "rb");
	if(fpt == NULL)
	{
		sprintf(err,"Can't open file : align10.txt! \n");
		return 0;
	}
	fread(align10, sizeof(unsigned char), 60*CAMNUM_2, fpt);
	fclose(fpt);

	//*********************����Ƭ�ķָ�����������******************************/
	int temp1 = twds.modelNum;

	if(temp1>MAXSEGMENTNUMBER)
	{
		err = "The segement number is max than MAXSEGMENTNUMBER!";
	}

	
	//**************��ȡ3D���������г����������Ӳ�����ƥ��***************************/
    //����˳��
	//1.��ȡ�������г����б�
	//2.���Ⱥ�˳���ȡ�����б��еĳ������ȶ�ȡ������Ϣscene.txt
	//3.�ȱȽ�scene.txt����Ƭ������tag�غ϶ȣ�����غ϶ȴ��ڷ�ֵ�����������򷵻�2��ȡ��һ������
	//4.�ٱȽ�������������ͬ��ǩ��relationship��partner��������ͬ��ǩ�Ķ��������Ҫ���ã�
	//5.�����յ����ƶȱ��棬����2
	//6.����

	//��һ�����򿪳����б�
	fpt = fopen("SearchConfigure\\SceneModelList.txt", "r");
	if(fpt == NULL)
	{
		sprintf(err,"Can't open file : SceneModelList.txt! \n");
		return 0;
	}

	//**********************�ڶ�����ѭ����ȡ�����б��еĳ�����Ϣ************
	TopNum = 0; //��¼�Ѿ��������ĳ����ĸ���

	while( fgets(destfn, 200, fpt) )
	{
		//���������ַ������Ͻ�����
		destfn[strlen(destfn)-1] = 0x00;

		//�޸��ļ�����������������.txt,������ȡ������Ϣ
		sprintf(filename, "%s.txt", destfn);

		//����޷���ȡ�����ȡ��һ������
		if( (fpt1 = fopen(filename, "rb")) == NULL )				
		{	continue;	}

		//**********************�ڶ���*******************
		//��ȡ3d�����е���Ϣscene.txt
		tempSimilarity = 0;
		while( fgets(temp2, LINE_MAX_LEN, fpt1) != NULL ) 
		{
			//���˵�ÿ���еĿ�ͷ�հ��ַ�
			for ( next = temp2; *next != '\0' && isspace(*next); next++ )
				;
			
			//���˵�ע�����Լ��հ׺�
			if ( *next == '\0' || *next == '#')
				continue;

			//�����ָ��������,��@total number��ʼ�������һ��Ӧ�ý���ģ�͸���
			if(*next =='@'&& *(next+1)=='t'&& *(next+2)=='o')
			{
				fgets(temp2, LINE_MAX_LEN, fpt1);
				next = temp2;
				sscanf ( next, "%d",&modelNumber);
				continue;
			}
			
			//�����ָ������ǩ,��@tag��ʼ
			//�����һ��Ӧ�ý�����ǩ����ǩ֮�䲻�ܼ��
			if(*next =='@'&& *(next+1)=='t'&& *(next+2)=='a')
			{
	
				for(int i = 0;i< modelNumber ;i++)
				{
					if(fgets(temp2, LINE_MAX_LEN, fpt1))
					{
						sscanf (temp2, "%d %s %f",&temp3,threedTag[i],&importance[i]);
					}
					else//��������ı�ǩ�����������һ�£�˵������������
					{
                        return 0;
					}
				}
				//ֱ�ӽ�����һ��ѭ��
				continue;
			}

			//��ȡ�����������relationship����
			//��@edge��ͷ
			//�����һ��Ӧ�ý���edge���󣬱�ǩ֮�䲻�ܼ��
			if(*next =='@'&& *(next+1)=='e'&& *(next+2)=='d')
			{

				for(int i=0;i<modelNumber;i++)
				{
					if(fgets(temp2, LINE_MAX_LEN, fpt1))
					{
						next1 = temp2;
						for(int j =0; j<modelNumber;j++)
						{
							relatinoship[i][j] = *(next1+j*2);
						}
					}
					else//��������ı�ǩ�����������һ�£�˵������������
					{
						return 0;
					}
					
				}

				//ֱ�ӽ�����һ��ѭ��
				continue;
			}//if
		}//while--��ȡ������txt��

		fclose(fpt1);

		//********************������***********************
		//�Ƚ���Ƭ������3D����tag֮������ƶ�
		//���ڲ�����������֮�����Ƶı�ǩ����
		for(int i = 0; i< modelNumber; i++) //��ʼ������
		{
			tagFlag[i] = 0;
		}
		temp3 = 0;
		//tag �ȶ�
		for(int i = 0; i< twds.modelNum ;i++)
		{
			for(int j = 0; j< modelNumber ;j++)
			{
				//��ʾ�����ǩû�б�ʹ���Һ���Ƭ��ǩһ��
				if( (!tagFlag[j]) && compareTwoTag(twds.tag1[i],threedTag[j]) )
				{
					temp3 ++; //���ڼ�¼tagһ�µĸ���
					tagFlag[j] =1; //˵����j��tag�Ѿ���ʹ�ã��������ж����ͬtag�����
					break;
				}
			}
		}
		//����tag��ض�
		temp4 = (float)temp3/(modelNumber+twds.modelNum);
		//����tag��ض��Ƿ���ڷ�ֵ��������ڣ�ֱ�ӽ�����һ�������Ƚϣ����ｲ��ֵ����Ϊ0.2
		if(temp4 < 0.01)    
		{ continue; }

		//********���tag���ƶȳ�����ֵ����ʼ��ȡ3D�������ָ������������
		temp1 = modelNumber;
		for(int i=0; i<temp1; i++)
		{
			// ART
			sprintf(filename, "%s_%d.obj_q8_v1.8.art", destfn,i);
			if( (fpt2 = fopen(filename, "rb")) == NULL )			
			{	printf("%s does not exist.\n", filename);	return 0;	}
			fread(dest_ArtCoeff2[i], ANGLE * CAMNUM * ART_COEF, sizeof(unsigned char), fpt2);
			fclose(fpt2);
			// FD
			sprintf(filename, "%s_%d.obj_q8_v1.8.fd",destfn ,i);
			if( (fpt2 = fopen(filename, "rb")) == NULL )			{	printf("%s does not exist.\n", filename);	return 0;	}
			fread(dest_FdCoeff_q82[i], sizeof(unsigned char),  ANGLE * CAMNUM * FD_COEF, fpt2);
			fclose(fpt2);
			// CIR
			sprintf(filename, "%s_%d.obj_q8_v1.8.cir", destfn,i);
			if( (fpt2 = fopen(filename, "rb")) == NULL )			{	printf("%s does not exist.\n", filename);	return 0;	}
			fread(dest_CirCoeff_q82[i], sizeof(unsigned char),  ANGLE * CAMNUM, fpt2);
			fclose(fpt2);
			// ECC
			sprintf(filename, "%s_%d.obj_q8_v1.8.ecc",destfn ,i);
			if( (fpt2 = fopen(filename, "rb")) == NULL )			{	printf("%s does not exist.\n", filename);	return 0;	}
			fread(dest_EccCoeff_q82[i], sizeof(unsigned char),  ANGLE * CAMNUM, fpt2);
			fclose(fpt2);
		}

		//���ڽ���¼��Ƭ��ÿ��������ͬ��ǩ�����������ʼ��
		for(int i = 0; i< MAXSEGMENTNUMBER; i++)
		{    sameTagCount[i] = 0;}
		//��������3D������ÿ��������Ҫ�ȵĸ�������
		for(int i = 0; i< modelNumber; i++) //��ʼ������
		{  tagFlag[i] = 0; }

		//������Ƭ��ÿ�����壬����Ѱ�Ҳ��Ƚ���3D��������ͬ��ǩ�����geometry���ƶ�
		for(int i = 0; i< twds.modelNum ;i++)
		{
			for(int j = 0; j< modelNumber ;j++)
			{
				//��һ���ҵ���ǩ
				if(compareTwoTag(twds.tag1[i],threedTag[j]) )
				{
					matresT[i][sameTagCount[i]].objectNum = j;
					matresT[i][sameTagCount[i]].similarity_G = MatchLF3(i,j);
					sameTagCount[i]++;
					temp6.push_back(j); //��ÿһ������ͬ��ǩ�����嶼��¼����
				}
			}

			//����importance
			if(sameTagCount[i]) temp4 = 0.7f/sameTagCount[i] ;
		
			for(int k = 0; k<temp6.size()&&!tagFlag[temp6.at(k)];k++)
			{
					importance[temp6.at(k)] = importance[temp6.at(k)] *0.3;
				    importance[temp6.at(k)] += temp4;
				    tagFlag[temp6.at(k)] = 1;
			}
		}

		//********************���Ĳ�***********************
		//�Ƚ�������������ͬ��ǩ��relationship��partner���ƶ�
		//������ʵ������һ����Զ�����⣬��ʱ�Ȳ����ǣ��򻯴���
		//�����������tag����ͬ����ô������Ϊ����֮������ƶ�Ϊ0���������ҵ����Ƶ�tag�����û�У�ֱ������
		//1.������Ƭ��ÿ�����壬�ҵ�����ͬ��ǩ������
		//2.���ڸ����������е�����<edge,partner>;���Ҷ�Ӧ������ͬ��<edge,partner>
		//2.����ͬ��ϵ��pair��pair֮���edge��Ϊ����
		//3.�����texture��geometry
		for(int i = 0; i< modelNumber; i++) //��ʼ������
		{
			tagFlag[i] = 0;
		}
		for(int i = 0; i< 10; i++)
		{
			edgeCount[i] = 0;
		}

		for(int i = 0; i< twds.modelNum; i++)
		{
			tempObjSimilarity = 0.0;
				
			//����������ڱ���������������ͬ��ǩ,�ٶ���ǰ����Ϊi
			if(sameTagCount[i])
			{				
				//1.������iͬtag��3D����j
				for(int j=0; j<sameTagCount[i];j++)
				{
					//2.��ʾ��������i,j��geometry��texture��tag���ƶ�,����aΪȨ��
					tempObjSimilarity += a*matresT[i][j].similarity_G;

					//3.����i��j���г���Ϊ2��relationship���Ƚ����ƶ�
					for(int k = 0; k<twds.modelNum ;k ++)
					{
						if(twds.relationship[i][k]!='0')
						{
							for(int l = 0; l<modelNumber; l++)
							{
								if(twds.relationship[i][k]==relatinoship[j][l])
									tempObjSimilarity +=c*1;
								//���k��l��tag��ͬ
								if(sameTagCount[k])
								{
									for(int n = 0; n <sameTagCount[k];n++)
									{
										if(matresT[k][n].objectNum == l)
										{
											tempObjSimilarity +=d*matresT[k][n].similarity_G;
										}
									}
								}//if
							}//for
						}//for

					}//for
						//��ƥ��������i��j����Ҫ�̶ȣ�Ȼ���ۼӵ�tempSimilarity
					tempObjSimilarity = tempObjSimilarity*twds.importance[i]*importance[j];
					tempSimilarity += tempObjSimilarity;
				}//for
				
			}//if
				
		}//for

		//�����ǰ�������Ľ��С��SCENEFORDISPLAY������++������topnum����
		if(TopNum < SCENEFORDISPLAY-1)
		{
			pTop[TopNum].Precise = tempSimilarity;
			strcpy(pTop[TopNum].name, destfn);
			TopNum ++;
		}
		else if(TopNum == SCENEFORDISPLAY-1)
		{
			pTop[TopNum].Precise = tempSimilarity;
			strcpy(pTop[TopNum].name, destfn);
			TopNum ++;

			int l;
			float maxTemp;
			char tempFilename1[200];
			//������Ÿ��������ÿ���ѭ�������
			for (int i=0;i<SCENEFORDISPLAY-1;i++)
			{
				l= i;
				maxTemp = pTop[0].Precise ;

				for(int j = i+1; j<SCENEFORDISPLAY; j++)
				{
					if(pTop[j].Precise >maxTemp)
					{
						maxTemp = pTop[j].Precise;
						l = j;
					}
				}
				strcpy(tempFilename1,pTop[l].name);
				pTop[l].Precise = pTop[i].Precise;
				strcpy(pTop[l].name,pTop[i].name);
				pTop[i].Precise = maxTemp;
				strcpy(pTop[i].name,tempFilename1);

			}

		}
		else //��ptop����������ʼ����
		{
			for(int i = SCENEFORDISPLAY-1; i>0;i--)
			{
				if(pTop[i].Precise>=tempSimilarity)
				{
					if(i==SCENEFORDISPLAY-1)
						break;
					else
					{
						//����������
						for(int j = SCENEFORDISPLAY-1; j<i-1;j--)
						{
							pTop[j].Precise = pTop[j-1].Precise;
							strcpy(pTop[j-1].name,pTop[j].name);
						}
						pTop[i-1].Precise = tempSimilarity;
						strcpy(pTop[i-1].name,destfn);
					}
				}
				else
				{

				}

			}//for
		}//else
							

	}//while -- 3D����list������г���

	//���������������ļ�
	if( (fpt2 = fopen("SearchResult\\SceneDisplay.txt", "w")) == NULL )				
	{	
		printf("ModelDisplay.txt does not exist.\n", filename);	
	}
	else
	{
		for(int i = 0; i< TopNum;i++)
		{
			fprintf(fpt2, "%s\n",pTop[i].name);
		}
	}
	fclose(fpt2);

	//������������浽doc��ͷ
	for(int i = 0; i< TopNum;i++)
	{
		strcpy(pSceneMatResult[i].name,pTop[i].name);
	}

	fclose(fpt);	
	free(pTop);

	return 1;
}

//���ڼ���ʱvector�Ƚϵĸ�������
bool compareTwoSearchHelp(const ThreeDSceneSearchHelp &h1, const ThreeDSceneSearchHelp &h2)
{
	return h1.modelTag <  h2.modelTag;
}


//���ڳ����������ռ������������ȽϺ���
bool compareSearchResult(const SceneMatRes &s1, const SceneMatRes &s2)
{
	return s1.similarity <  s2.similarity;
}


/************************************************************************/
/* �˺�����Find3DSceneFromBuffer���������ڣ�
Find3DSceneFromBufferʹ�õ��ǵ�һ�汾��3D�������ݿ� 
��Search3DSceneFromBufferʹ�õ��ǵڶ��汾�����ݿ�
*/
/************************************************************************/
int Search3DSceneFromBuffer(TwoDScene twds,int **TwoDRelationship,char* err,pSceneMatRes pSceneMatResult)
{
	// for region shape descriptor
	int			    TopNum;
	pMatRes		    pTop; //����ָ���������ƥ���
	FILE			*fpt, *fpt1,*fpt2;
	char			filename[200],destfn[200];//srcfn���������û������ģ��

	////******************������Ƭ������***********************
	Scene tempScene;  //���ڱ������ʱ���ݿ��е�3D��������ʱ����
	vector<ThreeDSceneSearchHelp> twdSceneHelp;  //���г����������ӵȱȽ�ʱ�ĸ�������
	vector<ThreeDSceneSearchHelp> threedSceneHelp;
	char temp2[400]; //����������ʱ��ȡ��scene.txt��ÿһ������
	int temp3,temp5; //��ʱ����
	float temp4;
	char* next;
	char* next1;
	char threedTag[MAXSEGMENTNUMBER][30]; //����������ά��������tag�ı������ٶ��������������50����tag���Ȳ�����30
	int tagFlag[MAXSEGMENTNUMBER]; //����tag�Ƚ�ʱ�����ʱ����
	int **threeDRelatinoship; //������ʱ������ά����������֮���relationship���ٶ����岻����50��
	int edgeCount[10]; //������¼ĳ������������relationship�ĸ��� �������飬��Ϊ�ܹ���8��relationship
	float tempSimilarity; //������ʱ�洢��������֮������ƶ�
	float tempObjSimilarity; //������ʱ�洢��������֮������ƶ�
	MatResTemp matresT[MAXSEGMENTNUMBER][20]; //������¼ÿ�������Ӧ��һ����������ͬtag��������ƶ�,ͬ���ٶ����岻����50����ÿ��������ͬ��ǩ�ĸ���������10��
	int sameTagCount[MAXSEGMENTNUMBER]; //��¼ÿ�������ڱ�����3D�����������Ʊ�ǩ���������
	int modelNumber; //���ڼ�¼���ָ���ά�������������
	float importance[MAXSEGMENTNUMBER]; //���ڼ�¼�������Ҫ�̶ȣ���Ҫ�̶������������С�ͷ�������ͬtag�������
	vector<int> temp6; //��������

	//****************����similarity�и�������֮���Ȩ��*******************
	float a = 5.0; //������ʾ��������relationship��tag���ƶ�Ȩ��
	float b = 3.0; //������ʾ��������relationship�е�relationship���ƶ�Ȩ��
	float c = 2.0; //������ʾ��������relationship����ͬtag��relationship��Ľ���ֵ
	//float d = 0.25; //������ʾ��������<edge,partner>���ƶ���partner��Ȩ��
	float e = 6.0; //������ʾ��������geometry���ƶȲ��ֵ�Ȩ��
	float f = 4.0; //������ʾ��������relationship���ƶȲ��ֵ�Ȩ��

	//pTop = (pMatRes) malloc (SCENEFORDISPLAY * sizeof(MatRes));  //����ƥ�����ı�������,�ݶ�ѡ��10��
	vector<SceneMatRes> searchResult;          //����������
	SceneMatRes tempResult;

	//���ٱ���3D����relationship���ڴ�
	threeDRelatinoship = new int*[MAXSEGMENTNUMBER];
	for(int i = 0; i<MAXSEGMENTNUMBER; i++)
	{
		threeDRelatinoship[i] = new int[MAXSEGMENTNUMBER];
	}

	// initialize: read camera pair
	fpt = fopen("SearchConfigure\\q8_table", "rb");
	if(fpt == NULL)
	{
		sprintf(err,"Can't open file : q8_table! \n");
		return 0;
	}
	else
	{
		fread(q8_table, sizeof(unsigned char), 65536, fpt);
		fclose(fpt);
	}


	// initialize: read camera pair
	//ò�������ﲻ��Ҫ��
	fpt = fopen("SearchConfigure\\align10.txt", "rb");
	if(fpt == NULL)
	{
		sprintf(err,"Can't open file : align10.txt! \n");
		return 0;
	}
	fread(align10, sizeof(unsigned char), 60*CAMNUM_2, fpt);
	fclose(fpt);

	//*********************����Ƭ�ķָ�����������******************************/
	int temp1 = twds.modelNum;

	if(temp1>MAXSEGMENTNUMBER)  //Ĭ�ϱ��ָ�����������������50��
	{
		err = "The segement number is max than MAXSEGMENTNUMBER!";
	}

	//����׼��
	ThreeDSceneSearchHelp tt;  //��������ת���ĸ�������
	for(int i = 0; i< twds.modelNum ;i++) //���ڻ�ȡ2D�����е�����tag
	{	
		tt.modelIndex = i;
		tt.modelTag = string(twds.tag1[i]);
		tt.sameTageCount = 1;  //���ڳ�ʼ��

		//����ÿ������ӵ�е�relationship�Լ�relationship����һͷ��tag
		for(int j = 0; j<twds.modelNum ; j++)
		{
			if(TwoDRelationship[i][j]!= 0)  //'0'Ĭ��Ϊ�޹�ϵ
			{
				tt.relationships.push_back( TwoDRelationship[i][j]);
				tt.relationshipTag.push_back( twds.tag1[j]);
			}
		}
		twdSceneHelp.push_back(tt);
	}
	sort(twdSceneHelp.begin(),twdSceneHelp.end(),compareTwoSearchHelp);  //��������

	//**************��ȡ�µ�3D���������г����������Ӳ�����ƥ��***************************/
	//����˳��
	//1.��ȡ�������г����б�,���г����б�����NewSceneModelList.txt�����ں��������͵ļ������Է��ļ��м�����
	//2.���Ⱥ�˳���ȡ�����б��еĳ�������--�ȶ�ȡ������Ϣscene.txt��--���˴��ĳɶ�ȡ������conference_room26.fbx
	//3.�ȱȽ�scene.txt����Ƭ������tag�غ϶ȣ�����غ϶ȴ��ڷ�ֵ�����������򷵻�2��ȡ��һ������
	//4.�ٱȽ�������������ͬ��ǩ��relationship��partner��������ͬ��ǩ�Ķ��������Ҫ���ã�
	//5.�����յ����ƶȱ��棬����2
	//6.����

	//��һ�����򿪳����б�
	fpt = fopen("SearchConfigure\\NewSceneModelList.txt", "r");
	if(fpt == NULL)
	{
		sprintf(err,"Can't open file : NewSceneModelList.txt! \n");
		return 0;
	}

	//**********************�ڶ�����ѭ����ȡ�����б��еĳ�����Ϣ************
	TopNum = 0; //��¼�Ѿ��������ĳ����ĸ���
	while( fgets(destfn, 200, fpt) )
	{
		//���������ַ������Ͻ�����
		destfn[strlen(destfn)-1] = 0x00;

		//�޸��ļ�����������������Relationship.cng,������ȡ����tag��relationship��Ϣ
		sprintf(filename, "%sRelationship.cng", destfn);
		string filePath(filename); //��char* תstring
		tempScene.LightReadRelationFile(filePath);//ͨ������Scene�еĺ�������ȡ3D������Ϣ
		//ΪthreeDRelatinoship��ʼ��
		for(int i = 0; i< MAXSEGMENTNUMBER; i++)
		{
			memset(threeDRelatinoship[i],0,MAXSEGMENTNUMBER*4);
		}
		tempScene.BuildRelationTable1(threeDRelatinoship);   //Ϊ3D��������relationship table

		//********************������***********************
		/************************************************************************/
		/*�Ƚ���Ƭ������3D����tag֮������ƶ�,������2D������3D�����еı�ǩ������һ��vector<string>��
		  ��������vector��������Ȼ���ͷ�ȵ������Ϳ�����*/
		/************************************************************************/

		//*****************��Ϊ�ò���ֻ��Ҫ���2D������ȡһ�Σ�����Ӧ�÷ŵ�whileѭ������
		//ThreeDSceneSearchHelp tt;  //��������ת���ĸ�������
		//for(int i = 0; i< twds.modelNum ;i++) //���ڻ�ȡ2D�����е�����tag
		//{	
		//	tt.modelIndex = i;
		//	tt.modelTag = string(twds.tag1[i]);
		//	twdSceneHelp.push_back(tt);
		//}
		//sort(twdSceneHelp.begin(),twdSceneHelp.end(),compareTwoSearchHelp);  //��������
		   
		for(int i = 0; i< tempScene.modelSize ;i++)//���ڻ�ȡ3D�����е�����tag
		{
			importance[i] = 1; //��ʱ����importance�����������tag���д���
			tt.modelIndex = i;
			tt.modelTag = tempScene.lightSceneModels[i].modelTag;
			tt.sameTageCount = 1; //���ڳ�ʼ��

			//����ÿ������ӵ�е�relationship�Լ�relationship����һͷ��tag
			for(int j = 0; j<tempScene.modelSize ; j++)
			{
				if(threeDRelatinoship[i][j]!= '0')  //'0'Ĭ��Ϊ�޹�ϵ
				{
					tt.relationships.push_back( threeDRelatinoship[i][j] - '0');
					tt.relationshipTag.push_back( tempScene.lightSceneModels[j].modelTag);
				}
			}

			threedSceneHelp.push_back(tt);
		}
		sort(threedSceneHelp.begin(),threedSceneHelp.end(),compareTwoSearchHelp);  //��������

		temp3 = 0; //������¼������������ͬ��ǩ�ĸ���
		int tempI = 0; //������ʶtwdSceneTag��Ԫ�ص������ʱ����
		int tempJ = 0; //������ʶthreedSceneTag��Ԫ�ص������ʱ����
		while(tempI< twds.modelNum && tempJ< tempScene.modelSize) //���бȽ�
		{
			if(twdSceneHelp[tempI].modelTag ==threedSceneHelp[tempJ].modelTag )
			{
				tempI++;
				tempJ++;
				temp3++;
			}
			else if(twdSceneHelp[tempI].modelTag > threedSceneHelp[tempJ].modelTag)
			{
				tempJ++;
			}
			else
			{
				tempI++;
			}
		}

		//����tag��ضȣ��˴�ֵ����ȶ����ΪĿ�곡���е�������ܱ��޳������Կ���Ӱ�첻��
		temp4 = (float)temp3/(tempScene.modelSize + twds.modelNum);
		//����tag��ض��Ƿ���ڷ�ֵ��������ڣ�ֱ�ӽ�����һ�������Ƚϣ����ｲ��ֵ����Ϊ0.2(���Ե�ʱ����0.01)
		if(temp4 < 0.1)    
		{ 
			//��ѭ��ʹ�õı�����գ��ָ�ԭֵ
			//1.���threedSceneHelp
			threedSceneHelp.clear();
			//2.���tempScene
			tempScene.lightSceneModels.clear();
			tempScene.RelationMap.clear();
			continue;
		}

		//�ֱ����2D��3D��������ͬtag��ģ�͵ĸ���
		tempI = 1;
		while (tempI< twds.modelNum)
		{
			if(twdSceneHelp[tempI].modelTag ==twdSceneHelp[tempI-1].modelTag)
			{
				//�����ǰһ��tag��ͬ������Ŀ��1
				twdSceneHelp[tempI].sameTageCount = twdSceneHelp[tempI-1].sameTageCount +1;
			}
			else
			{
				//����ȥ��ÿ��ģ�Ͷ���ֵ��ͬtag����
				int tempCount =  twdSceneHelp[tempI-1].sameTageCount -1; // 
				int tempIndex = tempI-2;
				while(tempCount > 0)
				{
					twdSceneHelp[tempIndex].sameTageCount = twdSceneHelp[tempI-1].sameTageCount;
					tempCount --;
					tempIndex--;
				}
			}

			if(tempI ==(twds.modelNum -1) && twdSceneHelp[tempI].sameTageCount>1)
			{
				//����ȥ��ÿ��ģ�Ͷ���ֵ��ͬtag����
				int tempCount =  twdSceneHelp[tempI].sameTageCount -1; // 
				int tempIndex = tempI-1;
				while(tempCount > 0)
				{
					twdSceneHelp[tempIndex].sameTageCount = twdSceneHelp[tempI].sameTageCount;
					tempCount --;
					tempIndex--;
				}
			}
			tempI++;
		}

		tempI = 1;
		while (tempI< tempScene.modelSize)
		{
			if(threedSceneHelp[tempI].modelTag ==threedSceneHelp[tempI-1].modelTag)
			{
				//�����ǰһ��tag��ͬ������Ŀ��1
				threedSceneHelp[tempI].sameTageCount = threedSceneHelp[tempI-1].sameTageCount +1;
			}
			else
			{
				//����ȥ��ÿ��ģ�Ͷ���ֵ��ͬtag����
				int tempCount =  threedSceneHelp[tempI-1].sameTageCount -1; // 
				int tempIndex = tempI-2;
				while(tempCount > 0)
				{
					threedSceneHelp[tempIndex].sameTageCount = threedSceneHelp[tempI-1].sameTageCount;
					tempCount --;
					tempIndex--;
				}
			}

			if(tempI ==(tempScene.modelSize -1) && threedSceneHelp[tempI].sameTageCount>1)
			{
				//����ȥ��ÿ��ģ�Ͷ���ֵ��ͬtag����
				int tempCount =  threedSceneHelp[tempI].sameTageCount -1; // 
				int tempIndex = tempI-1;
				while(tempCount > 0)
				{
					threedSceneHelp[tempIndex].sameTageCount = threedSceneHelp[tempI].sameTageCount;
					tempCount --;
					tempIndex--;
				}
			}
			tempI++;
		}

		////��¼����ÿ��2D����������tag��ͬ��3D�����е������index
		//tempI = 0;
		//tempJ = 0;
		//while(tempI< twds.modelNum && tempJ< tempScene.modelSize)
		//{
		//	//���2D�����е�ǰ�����ǰһ�������tag��ͬ�������ֱ�Ӹ���ǰһ�������sameTagIndex
		//	if(tempI>0 )
		//	{
		//		if(twdSceneHelp[tempI].modelTag ==twdSceneHelp[tempI-1].modelTag)
		//		{
		//			int i = 0;
		//			while(i < twdSceneHelp[tempI-1].sameTagIndex.size())
		//			{
		//				twdSceneHelp[tempI].sameTagIndex.push_back(twdSceneHelp[tempI-1].sameTagIndex[i]);
		//				i++;
		//			}
		//			tempI ++;
		//			continue;
		//		}

		//	}
		//	//�����ǰ2D�����е������3D����������tag��ͬ���򽲶Է���ӵ��Լ���sameTagIndex�У�������3D�����������б��м�����������
		//	if(twdSceneHelp[tempI].modelTag ==threedSceneHelp[tempJ].modelTag )
		//	{
		//		twdSceneHelp[tempI].sameTagIndex.push_back(threedSceneHelp[tempJ].modelIndex);
		//		tempJ++;
		//	}
		//	else if(twdSceneHelp[tempI].modelTag > threedSceneHelp[tempJ].modelTag)
		//	{
		//		tempJ ++;
		//	}
		//	else
		//	{
		//		tempI++;
		//	}
		//}


		////��¼����ÿ��3D����������tag��ͬ��2D�����е������index
		//tempI = 0;
		//tempJ = 0;
		//while(tempJ< twds.modelNum && tempI< tempScene.modelSize)
		//{
		//	//���3D�����е�ǰ�����ǰһ�������tag��ͬ�������ֱ�Ӹ���ǰһ�������sameTagIndex
		//	if(tempI>0 && threedSceneHelp[tempI].modelTag ==threedSceneHelp[tempI-1].modelTag)
		//	{
		//		int i = 0;
		//		while(i < threedSceneHelp[tempI-1].sameTagIndex.size())
		//		{
		//			threedSceneHelp[tempI].sameTagIndex.push_back(threedSceneHelp[tempI-1].sameTagIndex[i]);
		//			i++;
		//		}
		//		tempI ++;
		//		continue;
		//	}
		//	//�����ǰ2D�����е������3D����������tag��ͬ���򽲶Է���ӵ��Լ���sameTagIndex�У�������3D�����������б��м�����������
		//	if(twdSceneHelp[tempJ].modelTag ==threedSceneHelp[tempI].modelTag )
		//	{
		//		threedSceneHelp[tempI].sameTagIndex.push_back(twdSceneHelp[tempJ].modelIndex);
		//		tempJ++;
		//	}
		//	else if(threedSceneHelp[tempI].modelTag > twdSceneHelp[tempJ].modelTag)
		//	{
		//		tempJ ++;
		//	}
		//	else
		//	{
		//		tempI++;
		//	}
		//}//while

		
		temp1 = tempScene.modelSize;
		for(int i=0; i<temp1; i++)
		{
			// ART
			sprintf(filename, "%s%s_q8_v1.8.art", destfn,tempScene.lightSceneModels[i].modelName.c_str());
			if( (fpt2 = fopen(filename, "rb")) == NULL )			
			{	printf("%s does not exist.\n", filename);	return 0;	}
			fread(dest_ArtCoeff2[i], ANGLE * CAMNUM * ART_COEF, sizeof(unsigned char), fpt2);
			fclose(fpt2);
			// FD
			sprintf(filename, "%s%s_q8_v1.8.fd", destfn,tempScene.lightSceneModels[i].modelName.c_str());
			if( (fpt2 = fopen(filename, "rb")) == NULL )			{	printf("%s does not exist.\n", filename);	return 0;	}
			fread(dest_FdCoeff_q82[i], sizeof(unsigned char),  ANGLE * CAMNUM * FD_COEF, fpt2);
			fclose(fpt2);
			// CIR
			sprintf(filename, "%s%s_q8_v1.8.cir", destfn,tempScene.lightSceneModels[i].modelName.c_str());
			if( (fpt2 = fopen(filename, "rb")) == NULL )			{	printf("%s does not exist.\n", filename);	return 0;	}
			fread(dest_CirCoeff_q82[i], sizeof(unsigned char),  ANGLE * CAMNUM, fpt2);
			fclose(fpt2);
			// ECC
			sprintf(filename, "%s%s_q8_v1.8.ecc", destfn,tempScene.lightSceneModels[i].modelName.c_str());
			if( (fpt2 = fopen(filename, "rb")) == NULL )			{	printf("%s does not exist.\n", filename);	return 0;	}
			fread(dest_EccCoeff_q82[i], sizeof(unsigned char),  ANGLE * CAMNUM, fpt2);
			fclose(fpt2);
		}


		//��¼ÿ��2D�����������3D��������ͬ��ǩ��������ƶ��Լ�3D������ÿ�������importance
		//����ٻ�ȡÿ��2D�����е��������ܴ�3D��������ͬtag���崦��ȡ��������ƶ�
		//�����ۻ����������������ƶ�֮��
		int tempIndexi = 0;
		int tmepIndexj = 0;
		tempSimilarity = 0.0f; //����2D������3D�������������ƶ�
		float tempObjMaxSimilarity ; //���ڱ���2D������ĳ�������3D��������ͬtag�����ܻ�õĵ�ǰ������ƶ�
		float tempRelationshipSimilarity; //������ʱ�������������relationship���ƶ�
		int sameTagCount;
		int sameRelationshipCount;
		int sameTagAndRelCount;
		int tempSum;
		tempI = 0;
		tempJ = 0;
		bool firstStepInTag = true;    //������¼�Ƿ��ǵ�һ��������ͬtag�����壬�Ǿ���true�� ���Ǿ���false��
		int sameTagBegineIndex = 0;     //������ͬtag��3D�����������index��ʼλ��
		
		//����ÿһ��2D�����е�������3D�����������������ƶȣ����ۼӵ�����ĳ������ƶ���
		for(tempI = 0; tempI < twds.modelNum; tempI++)
		{
			//����Ѱ����ͬtag��3D�����е�����λ��
			tempJ = 0;
			while( tempJ < tempScene.modelSize && twdSceneHelp[tempI].modelTag > threedSceneHelp[tempJ].modelTag )
			{
				tempJ ++;
			}
			//˵��indexΪtempI��2D�����е�������3D������û����֮tag��ͬ������
			if(tempJ < tempScene.modelSize && twdSceneHelp[tempI].modelTag < threedSceneHelp[tempJ].modelTag)
			{
				continue;
			}

			//������indexΪtempI��2D�����е�������ͬtag�������������ƶȣ���ȡ���ֵ�ۼӵ��������ƶ�
			tempObjSimilarity = 0.0f;  //����2D��������������3D��������ͬtag��������õ�������ƶ�
			tempIndexi = twdSceneHelp[tempI].modelIndex;  //��¼��ǰ2D�������������
			while(tempJ< tempScene.modelSize && twdSceneHelp[tempI].modelTag == threedSceneHelp[tempJ].modelTag)
			{
				tempObjMaxSimilarity = 0.0f;
				tempRelationshipSimilarity = 0.0f;

				//��ȡ��tempI��ͬtag��3D�����е������index
				tmepIndexj = threedSceneHelp[tempJ].modelIndex;

				//1.�������ƶ�
				tempObjMaxSimilarity += e*MatchLF3(tempIndexi,tmepIndexj); 

				//2.relationship���ƶ�
				//2.1 tag���ƶ� �� 2.2 relationshipTag���ƶ� ��2.3 ����ֵ
				sameTagCount = 0;
				sameRelationshipCount = 0;
				sameTagAndRelCount = 0;
				bool relationBool = false;   //�Լ���2D�����е�ĳ�������relationship��Ӧ��relationshipTag�Ƿ��Ѿ���3D�����ж�Ӧ����tempJ��relationship���ҵ�����ͬ��relationshipTag
				bool tagBool = false;  //�Լ���2D�����е�ĳ�������relationship��Ӧ��tag�Ƿ��Ѿ���3D�����ж�Ӧ����tempJ��relationship���ҵ�����ͬ��tag
				//ͳ����ͬ��tag�Լ�relationshiptag
				for(int i = 0; i<twdSceneHelp[tempI].relationshipTag.size() ; i++)
				{
					relationBool = false; 
					tagBool = false;
					for(int j =0; j< threedSceneHelp[tempJ].relationshipTag.size() ; j++)
					{
						if(!tagBool && twdSceneHelp[tempI].relationshipTag[i] == threedSceneHelp[tempJ].relationshipTag[j] )
						{
							++sameTagCount ;
							tagBool = true;
						}

						if(!relationBool && twdSceneHelp[tempI].relationships[i] == threedSceneHelp[tempJ].relationships[j] )
						{
							++sameRelationshipCount;
							relationBool = true;
						}

						if(twdSceneHelp[tempI].relationships[i] == threedSceneHelp[tempJ].relationships[j] && twdSceneHelp[tempI].relationshipTag[i] == threedSceneHelp[tempJ].relationshipTag[j] )
						{
							//++sameRelationshipCount;
							++sameTagAndRelCount;
							break;
						}

					}//for
				}//for

				//������������ͬtag������ĸ���֮��
				tempSum = twdSceneHelp[tempI].relationshipTag.size() + threedSceneHelp[tempJ].relationshipTag.size();

				//��֤��������������relationship
				if(tempSum > 0)
				{
					tempRelationshipSimilarity +=  a*(sameTagCount*2/ (float)tempSum) ;  //2.1 tag��ֵͬ
					tempRelationshipSimilarity +=  b*(sameRelationshipCount*2/ (float)tempSum) ;  //2.2 relationship��ֵͬ
					tempRelationshipSimilarity +=  c*(sameTagAndRelCount*2/ (float)tempSum) ;  //2.2  ����ֵ
				}

				tempObjMaxSimilarity += f*tempRelationshipSimilarity;
				//importance
				tempObjMaxSimilarity = tempObjMaxSimilarity*twds.importance[tempIndexi]*importance[tmepIndexj]; 
				//�ͷ�ֵ��������ģ�͸�������Ҫ�̶�Խ��
				if(twdSceneHelp[tempI].sameTageCount > threedSceneHelp[tempJ].sameTageCount)
				{
					tempObjMaxSimilarity = tempObjMaxSimilarity*(1+threedSceneHelp[tempJ].sameTageCount)/(1+twdSceneHelp[tempI].sameTageCount); 
				}
				else
				{
					tempObjMaxSimilarity = tempObjMaxSimilarity*(1+twdSceneHelp[tempI].sameTageCount)/(1+threedSceneHelp[tempJ].sameTageCount); 
				}
				//if(twdSceneHelp[tempI].sameTagIndex.size() > threedSceneHelp[tempJ].sameTagIndex.size())
				//{
				//	tempObjMaxSimilarity = tempObjMaxSimilarity*(1+threedSceneHelp[tempJ].sameTagIndex.size())/(1+twdSceneHelp[tempI].sameTagIndex.size()); 
				//}
				//else
				//{
				//	tempObjMaxSimilarity = tempObjMaxSimilarity*(1+twdSceneHelp[tempI].sameTagIndex.size())/(1+threedSceneHelp[tempJ].sameTagIndex.size()); 
				//}

				//�����������ƶ�
				tempObjSimilarity = tempObjMaxSimilarity > tempObjSimilarity? tempObjMaxSimilarity:tempObjSimilarity;

				tempJ ++ ;  //ƥ����һ����֮���Ƶ�����
			}//while

			//����ǰ����tempI���ҵ���������ƶȼ��뵽�����������ƶ���
			tempSimilarity += tempObjSimilarity;

		}//for

	
		//�ٽ���ǰ�ļ��������������������
		//��ȫ���������vector��Ȼ��������ȡ����ƥ��ġ��������ƥ����ָ���ġ�
		tempResult.similarity = tempSimilarity;
		strcpy(tempResult.name, destfn);
		searchResult.push_back(tempResult);	

		//��ѭ��ʹ�õı�����գ��ָ�ԭֵ
		//1.���threedSceneHelp
		threedSceneHelp.clear();
		//2.���tempScene
		tempScene.lightSceneModels.clear();
		tempScene.RelationMap.clear();

		//�Ѿ��������ĳ����ĸ���
		TopNum++;
	}//while -- 3D����list������г���

	//�Լ��������������
	sort(searchResult.begin(),searchResult.end(),compareSearchResult);  //��������

	//���������������ļ�
	if( (fpt2 = fopen("SearchResult\\NewSceneDisplay.txt", "w")) == NULL )				
	{	
		printf("NewSceneDisplay.txt does not exist.\n", filename);	
	}
	else
	{
		for(int i = TopNum-1; i >= 0; i--)
		{
			fprintf(fpt2, "%s\n",searchResult[i].name);
		}
	}
	fclose(fpt2);
	fclose(fpt);	

	////��Ϊ����ֱ�Ӵ��ļ���ȡ�˽�����Ͳ�������������֮����
	////������������浽
	//for(int i = TopNum-1; i >= 0; i--)
	//{
	//	strcpy(pSceneMatResult[i].name,searchResult[i].name);
	//}

	//�ͷű���3D�������ڴ�
	for(int i = 0; i<MAXSEGMENTNUMBER; i++)
	{
		delete [] threeDRelatinoship[i];
		threeDRelatinoship[i] = NULL;

	}
	delete [] threeDRelatinoship ;
	threeDRelatinoship = NULL;

	return TopNum;
}

