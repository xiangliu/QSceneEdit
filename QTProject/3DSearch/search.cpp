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

#define			DataBaseModelNum    873   //模型总数
//#define         MAXOBJECTNUMBER   50      //假定场景中物体个数最多为50个

// #1
unsigned char	q8_table[256][256]; //每个位置存储的就是行和列之差

unsigned char	src_ArtCoeff[SRC_ANGLE][CAMNUM][ART_COEF];
unsigned char	src_ArtCoeff1[ART_COEF];     //new added var by liuxiang
unsigned char	dest_ArtCoeff[DataBaseModelNum][SRC_ANGLE][CAMNUM][ART_COEF];

unsigned char	align10[60][CAMNUM_2];
int				cost[SRC_ANGLE][ANGLE][CAMNUM][CAMNUM];

// for Fourier Descriptor matching
unsigned char	src_FdCoeff_q8[ANGLE][CAMNUM][FD_COEF];//一看src只有一个，所以是用来查找的模型
unsigned char	src_FdCoeff_q81[FD_COEF];    //new added var by liuxiang
unsigned char	dest_FdCoeff_q8[DataBaseModelNum][ANGLE][CAMNUM][FD_COEF];

// for Circularity
unsigned char	src_CirCoeff_q8[ANGLE][CAMNUM];
unsigned char	src_CirCoeff_q81;//new added var by liuxiang
unsigned char	dest_CirCoeff_q8[DataBaseModelNum][ANGLE][CAMNUM];

// for Eccentricity离心率
unsigned char	src_EccCoeff_q8[ANGLE][CAMNUM];
unsigned char	src_EccCoeff_q81;//new added var by liuxiang
unsigned char	dest_EccCoeff_q8[DataBaseModelNum][ANGLE][CAMNUM];


//******************保存照片描述子***********************
unsigned char	src_ArtCoeff2[MAXSEGMENTNUMBER][ART_COEF];     //用来保存一个照片中分割物体所有的描述子
unsigned char	src_FdCoeff_q82[MAXSEGMENTNUMBER][FD_COEF];    //保存照片场景的
unsigned char	src_CirCoeff_q82[MAXSEGMENTNUMBER];            //保存照片场景
unsigned char	src_EccCoeff_q82[MAXSEGMENTNUMBER];            //保存照片场景
	
//*******************保存3D场景分割结果描述子******************
unsigned char	dest_ArtCoeff2[MAXSEGMENTNUMBER][SRC_ANGLE][CAMNUM][ART_COEF];
unsigned char	dest_FdCoeff_q82[MAXSEGMENTNUMBER][ANGLE][CAMNUM][FD_COEF];
unsigned char	dest_CirCoeff_q82[MAXSEGMENTNUMBER][ANGLE][CAMNUM];
unsigned char	dest_EccCoeff_q82[MAXSEGMENTNUMBER][ANGLE][CAMNUM];


//abs（）求绝对值的方法
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

//计算和的方式，即方案二
int MatchLF1(unsigned char dest_ArtCoeff[SRC_ANGLE][CAMNUM][ART_COEF], 
	unsigned char dest_FdCoeff_q8[ANGLE][CAMNUM][FD_COEF],
	unsigned char dest_CirCoeff_q8[ANGLE][CAMNUM],
	unsigned char dest_EccCoeff_q8[ANGLE][CAMNUM])
{
	static int				cost_q81[ANGLE][CAMNUM];
	int						i, j, srcCam, destCam;
	register unsigned char	m;
	//int						err, MinErr;
	int                     err1,MinErr1;//added by liuxiang,为了计算最小值
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
			//q8_table就是用来加速的
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
			fd_distance <<= 2;//搞不懂这个为什么左移，是不是个数太少了，比重太低了，因为ART没移

			// for Circularity
			cir_distance = q8_table[dest_CirCoeff_q8[destCam][i]][src_CirCoeff_q81];
			cir_distance <<= 1;

			// for Eccentricity
			ecc_distance = q8_table[dest_EccCoeff_q8[destCam][i]][src_EccCoeff_q81];
			//ecc_distance <<= 1;

			cost_q81[destCam][i] = art_distance + fd_distance + cir_distance + ecc_distance;
		}

		// find minimum error of the two models from all camera pairs
		//此处修改为从一组角度中找出最小得，最终将所有最小的都加起来
		err1 = 0;//这个值必须设置成最大的，要不然最后比较结果都为
		MinErr1 = 0;
		//for(srcCam=0; srcCam<SRC_ANGLE; srcCam++)		// each src angle
		for(destCam=0; destCam<ANGLE; destCam++)	// each dest angle
		{
			err1 = cost_q81[destCam][0];
			for(j=1; j<CAMNUM; j++)						// each align，这里原指的是一对匹配
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


//计算和的方式，即方案二
int MatchLF2(unsigned char dest_ArtCoeff[SRC_ANGLE][CAMNUM][ART_COEF], 
	unsigned char dest_FdCoeff_q8[ANGLE][CAMNUM][FD_COEF],
	unsigned char dest_CirCoeff_q8[ANGLE][CAMNUM],
	unsigned char dest_EccCoeff_q8[ANGLE][CAMNUM])
{
	static int				cost_q81[ANGLE][CAMNUM];
	int						i, j, srcCam, destCam;
	register unsigned char	m;
	//int						err, MinErr;
	int                     err1,MinErr1;//added by liuxiang,为了计算最小值
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
			//q8_table就是用来加速的
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
			//fd_distance <<= 1;//搞不懂这个为什么左移，是不是个数太少了，比重太低了，因为ART没移
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
		//此处修改为从一组角度中找出最小得，最终将所有最小的都加起来
		err1 = 0;//这个值必须设置成最大的，要不然最后比较结果都为
		MinErr1 = 0;
		err1 = cost_q81[0][0];
		//for(srcCam=0; srcCam<SRC_ANGLE; srcCam++)		// each src angle
		for(destCam=0; destCam<ANGLE; destCam++)	// each dest angle
		{
			//err1 = cost_q81[destCam][0];
			for(j=1; j<CAMNUM; j++)						// each align，这里原指的是一对匹配
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

//计算和的方式，即方案二的改写，
//其中pNum代表照片分割结果中的第几个物体，而tNum则代表3D某个场景的第tNUM个物体
//只是返回值是值最小最像
float MatchLF3(int pNum, int tNum)
{
	static int				cost_q81[ANGLE][CAMNUM];
	int						i, j, srcCam, destCam;
	register unsigned char	m;
	//int						err, MinErr;
	int                     err1,MinErr1;//added by liuxiang,为了计算最小值
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
			//q8_table就是用来加速的
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
			//fd_distance <<= 1;//搞不懂这个为什么左移，是不是个数太少了，比重太低了，因为ART没移
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
		//此处修改为从一组角度中找出最小得，最终将所有最小的都加起来
		err1 = 0;//这个值必须设置成最大的，要不然最后比较结果都为
		MinErr1 = 0;
		err1 = cost_q81[0][0];
		//for(srcCam=0; srcCam<SRC_ANGLE; srcCam++)		// each src angle
		for(destCam=0; destCam<ANGLE; destCam++)	// each dest angle
		{
			//err1 = cost_q81[destCam][0];
			for(j=1; j<CAMNUM; j++)						// each align，这里原指的是一对匹配
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


//matchLF2的改写，因为matchLF2是通过传递参数来进行检索，参数复制需要时间，所以次方法不需要参数复制
//此方法用于匹配一张用户手绘照片与3D模型库中的模型
//其中databaseNum代表3D模型数据库中的第几个模型
int MatchLF4(int databaseNum)
{
	static int				cost_q81[ANGLE][CAMNUM];
	int						i, j, srcCam, destCam;
	register unsigned char	m;
	int                     err1,MinErr1;//added by liuxiang,为了计算最小值
	int						art_distance, fd_distance, cir_distance, ecc_distance;


	// compare each coefficients pair from the two models first
	//for(srcCam=0; srcCam<ANGLE; srcCam++)
	for(destCam=0; destCam<ANGLE; destCam++)
		for(i=0; i<CAMNUM; i++)
			//for(j=0; j<CAMNUM; j++)
		{
			// un-loop to speed-up
			// for ART (Zernike moment)
			//q8_table就是用来加速的
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
			//fd_distance <<= 1;//搞不懂这个为什么左移，是不是个数太少了，比重太低了，因为ART没移
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
		//此处修改为从一组角度中找出最小得，最终将所有最小的都加起来
		err1 = 0;//这个值必须设置成最大的，要不然最后比较结果都为
		MinErr1 = 0;
		err1 = cost_q81[0][0];
		//for(srcCam=0; srcCam<SRC_ANGLE; srcCam++)		// each src angle
		for(destCam=0; destCam<ANGLE; destCam++)	// each dest angle
		{
			//err1 = cost_q81[destCam][0];
			for(j=1; j<CAMNUM; j++)						// each align，这里原指的是一对匹配
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
	char			filename[200], cmpfn[200], srcfn[20][200], destfn[200], c1[200];//srcfn用来保存用户输入的模型
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
	//貌似在这里不需要了
	fpt = fopen("SearchConfigure\\align10.txt", "rb");
	if(fpt == NULL)
	{
		sprintf(err,"Can't open file : align10.txt! \n");
		return 0;
	}
	fread(align10, sizeof(unsigned char), 60*CAMNUM_2, fpt);
	fclose(fpt);

	//**************read feature of all models***************************
	//即模型库
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

	//如果读到的模型个数与期望值不同，则直接退出程序
	if( Count != DataBaseModelNum )
	{	
		//printf("The model count is different from ModelNum!!\n");	
		sprintf(err,"The model count is different from ModelNum!");
		return 0;	
	}
	//**************end of read feature of all models***************************
	
	//如果总共的模型库个数大于5个，那么我们只显示最像的5个
	if(DataBaseModelNum > MODELSEARCHRESULTNUMBER)
	{
		TopNum = MODELSEARCHRESULTNUMBER;
	}
	else
	{
		TopNum = DataBaseModelNum;		
	}

	//保存匹配结果的变量分配
	pmr = (pMatRes) malloc (DataBaseModelNum * sizeof(MatRes));
	pTop = (pMatRes) malloc (TopNum * sizeof(MatRes));
	if(!pmr||!pTop)
	{
		sprintf(err,"buffer allocate failed! \n"); 
		return 0;
	}

	// initialize
	AllClassNum = 0;//所有用户输入模型个数

	//compare.txt-->  每一行都作为一个文件A-->  A中每一行作为一个model
	// read filename of two models
	if( (fpt = fopen("SearchConfigure\\CompareSource.txt", "r")) == NULL )				
	{	
		sprintf(err,"Can't open the file compare.txt");	
		return 0;	
	}

	//每次读取compare.txt中的一行
	//while( fscanf(fpt, "%s", cmpfn) != EOF )
	//{
		//sprintf(filename, "SearchConfigure\\%s.txt", cmpfn);
		//fpt1 = fopen(filename, "r");
		//当再次返回while循环时，classNum已经被清零，所以%s.txt保存的是需要来查找的那个modle
		ClassNum = 0;

		//将compare.txt下的各项类容读取
		//随后再将各个文件下的文件名读取
		//其中总个数保存在ClassNum之中
		while( fgets(srcfn[ClassNum], 200, fpt) )
		{   //srcfn表示source file name
			srcfn[ClassNum][strlen(srcfn[ClassNum])-1] = 0x00;
			ClassNum++;
		}
		fclose(fpt);

		AllClassNum += ClassNum;//也就是在最后统计的时候用到了

		//随后将compare.txt中得各个文件读出来
		//读的是用户的输入
		//printf("")
		//对于compare中每一行所代表的文件夹中的每个文件进行检索.
		for(n=0; n<ClassNum; n++)
		{
			//从读的文件大小来说，这是用户输入图片的算子
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
			//这里再次打开同一个文件是为什么？--这里是为了读取那个名字，好保存
			//这里处在第三层循环里
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
			//sort之初始化
			for(i=0; i<TopNum; i++)
			{
				pTop[i].sim = INT_MAX;
				strcpy(pTop[i].name, "");
			}

			//被匹配的所有模型的个数
			for(k=0; k<DataBaseModelNum; k++)
				for(i=0; i<TopNum; i++)//就是和pTop中得每个都进行比较，如果更小则更像
					if( pmr[k].sim < pTop[i].sim )
					{
						//如果TopNum >1
						if(TopNum>1)
						{
							//将所有的pTop都往后移
							//其中TopNum-2为倒数第二个，将其移动到倒数第一个
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
					//把结果写入searchResult.txt中

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

//用来检索单个3D模型的方法,用于在后阶段用户手绘模型的检索
int search3DModel(char* err,pObjectMatRes pObjectMatResult)
{
	// for region shape descriptor
	int				i, j, k, n, m, Count, TopNum, ClassNum, AllClassNum;
	pMatRes			pmr, pTop;
	int				SIM;
	//	double			SIM;
	//	clock_t			start, finish;
	FILE			*fpt, *fpt1,*fpt2;
	char			filename[200], cmpfn[200], srcfn[20][200], destfn[200], c1[200];//srcfn用来保存用户输入的模型
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
	//貌似在这里不需要了
	fpt = fopen("SearchConfigure\\align10.txt", "rb");
	if(fpt == NULL)
	{
		sprintf(err,"Can't open file : align10.txt! \n");
		return 0;
	}
	fread(align10, sizeof(unsigned char), 60*CAMNUM_2, fpt);
	fclose(fpt);

	//**************read feature of all models***************************
	//即模型库
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

	//如果读到的模型个数与期望值不同，则直接退出程序
	if( Count != DataBaseModelNum )
	{	
		sprintf(err,"The model count is different from ModelNum!");
		return 0;	
	}
	//**************end of read feature of all models***************************
	
	//如果总共的模型库个数大于5个，那么我们只显示最像的5个
	if(DataBaseModelNum > MODELSEARCHRESULTNUMBER)
	{
		TopNum = MODELSEARCHRESULTNUMBER;
	}
	else
	{
		TopNum = DataBaseModelNum;		
	}

	//保存匹配结果的变量分配
	pmr = (pMatRes) malloc (DataBaseModelNum * sizeof(MatRes));
	pTop = (pMatRes) malloc (TopNum * sizeof(MatRes));
	if(!pmr||!pTop)
	{
		sprintf(err,"buffer allocate failed! \n"); 
		return 0;
	}


	//*************基于前阶段照片提取中保存在数组中的描述子进行检索
	//1.打开3D模型数据库列表，
	//2.每次读取一个模型，用照片模型与之匹配，保存匹配结果
	//3.给匹配结果排序，保留匹配度最高的模型

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


	// sort,给检索结果排序
	//sort之初始化
	for(i=0; i<TopNum; i++)
	{
		pTop[i].sim = INT_MAX;
		strcpy(pTop[i].name, "");
	}

	//被匹配的所有模型的个数
	for(k=0; k<DataBaseModelNum; k++)
		for(i=0; i<TopNum; i++)//就是和pTop中得每个都进行比较，如果更小则更像
			if( pmr[k].sim < pTop[i].sim )
			{
				//如果TopNum >1
				if(TopNum>1)
				{
					//将所有的pTop都往后移
					//其中TopNum-2为倒数第二个，将其移动到倒数第一个
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


	//将检索结果保存到doc的变量里头
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

//基于对照片分割结果以及提取的描述子，检索匹配的3D场景
//1.先读取照片场景中各个物体描述子
//2.读取场景列表中一个场景的描述子
//3.进行比较，将相似度保存
//4.返回2继续
//5.(释放新开辟内存)
int Find3DScene(TwoDScene twds,char* err)
{
	// for region shape descriptor
	int			    TopNum;
	pMatRes		    pTop;
	FILE			*fpt, *fpt1,*fpt2;
	char			filename[200],destfn[200];//srcfn用来保存用户输入的模型

	////******************保存照片描述子***********************
	//unsigned char	src_ArtCoeff2[MAXSEGMENTNUMBER][ART_COEF];     //用来保存一个照片中分割物体所有的描述子
	//unsigned char	src_FdCoeff_q82[MAXSEGMENTNUMBER][FD_COEF];    //保存照片场景的
	//unsigned char	src_CirCoeff_q82[MAXSEGMENTNUMBER];            //保存照片场景
	//unsigned char	src_EccCoeff_q82[MAXSEGMENTNUMBER];            //保存照片场景
	//
	////*******************保存3D场景分割结果描述子******************
	//unsigned char	dest_ArtCoeff2[MAXSEGMENTNUMBER][SRC_ANGLE][CAMNUM][ART_COEF];
	//unsigned char	dest_FdCoeff_q82[MAXSEGMENTNUMBER][ANGLE][CAMNUM][FD_COEF];
	//unsigned char	dest_CirCoeff_q82[MAXSEGMENTNUMBER][ANGLE][CAMNUM];
	//unsigned char	dest_EccCoeff_q82[MAXSEGMENTNUMBER][ANGLE][CAMNUM];


	//ThreeDSource threeds;  //用来保存读取的3D场景
	char temp2[400]; //用来保存临时读取的scene.txt的每一行内容
	int temp3,temp4,temp5; //临时变量
	char* next;
	char* next1;
	char threedTag[MAXSEGMENTNUMBER][30]; //用来保存三维场景物体tag的变量，假定物体个数不超过50，且tag长度不超过30
	int tagFlag[MAXSEGMENTNUMBER]; //用于tag比较时候的临时变量
	char relatinoship[MAXSEGMENTNUMBER][MAXSEGMENTNUMBER]; //用于临时保存三维场景中物体之间的relationship，假定物体不超过50个
	int edgeCount[10]; //用来记录某个物体所包含relationship的个数 辅助数组，因为总共就8种relationship
	float tempSimilarity; //用来临时存储两个场景之间的相似度
	float tempObjSimilarity; //用来临时存储两个物体之间的相似度
	MatResTemp matresT[MAXSEGMENTNUMBER][10]; //用来记录每个物体对应另一个场景中相同tag物体的相似度,同样假定物体不超过50个，每个物体相同标签的个数不超过10个
	int sameTagCount[MAXSEGMENTNUMBER]; //记录每个物体在被检索3D场景中有相似标签的物体个数
	int modelNumber ; //用来记录场景被分割以后物体个数
	float importance[MAXSEGMENTNUMBER]; //用于记录物体的重要程度，重要程度正比于物体大小和反比于相同tag物体个数

	//****************最终similarity中各个部分之间的权重*******************
	float a = 0.5; //用来表示两个物体geometry相似度之间的权重
	float b = 0.5; //用来表示两个物体<edge,partner>相似度之间的权重
	float c = 0.5; //用来表示两个物体<edge,partner>相似度中edge的权重
	float d = 0.5; //用来表示两个物体<edge,partner>相似度中partner的权重
	//float e = 0.5; //用来表示物体重要程度中count部分的权重
	//float f = 0.5; //用来表示物体重要程度中size部分的权重

	pTop = (pMatRes) malloc (10 * sizeof(MatRes));  //保存匹配结果的变量分配,暂定选择10个

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
	//貌似在这里不需要了
	fpt = fopen("SearchConfigure\\align10.txt", "rb");
	if(fpt == NULL)
	{
		sprintf(err,"Can't open file : align10.txt! \n");
		return 0;
	}
	fread(align10, sizeof(unsigned char), 60*CAMNUM_2, fpt);
	fclose(fpt);

	//*********************读照片的分割结果的描述子******************************/
	int temp1 = twds.modelNum;

	if(temp1>MAXSEGMENTNUMBER)
	{
		err = "The segement number is max than MAXSEGMENTNUMBER!";
	}

	for(int i=0; i<temp1; i++)
	{
		//从读的文件大小来说，这是用户输入图片的算子
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


	//**************读取3D场景中所有场景的描述子并进行匹配***************************/
    //具体顺序：
	//1.读取场景库中场景列表
	//2.按先后顺序读取场景列表中的场景，先读取场景信息scene.txt
	//3.先比较scene.txt与照片场景中tag重合度，如果重合度大于阀值，继续，否则返回2读取下一个场景
	//4.再比较两个场景中相同标签的relationship和partner（这里相同标签的定义可能需要斟酌）
	//5.将最终的相似度保存，返回2
	//6.结束

	//第一步：打开场景列表
	fpt = fopen("SearchConfigure\\SceneModelList.txt", "r");
	if(fpt == NULL)
	{
		sprintf(err,"Can't open file : SceneModelList.txt! \n");
		return 0;
	}

	//**********************第二步：循环读取场景列表中的场景信息************
	TopNum = 0; //记录已经检索到的场景的个数

	while( fgets(destfn, 200, fpt) )
	{
		//给读到的字符串加上结束符
		destfn[strlen(destfn)-1] = 0x00;

		//修改文件名，在名字最后加上.txt,用来读取场景信息
		sprintf(filename, "%s.txt", destfn);

		//如果无法读取，则读取下一个场景
		if( (fpt1 = fopen(filename, "rb")) == NULL )				
		{	continue;	}

		//**********************第二步*******************
		//读取3d场景中的信息scene.txt
		tempSimilarity = 0;
		while( fgets(temp2, LINE_MAX_LEN, fpt1) != NULL ) 
		{
			//过滤掉每行中的开头空白字符
			for ( next = temp2; *next != '\0' && isspace(*next); next++ )
				;
			
			//过滤掉注视行以及空白航
			if ( *next == '\0' || *next == '#')
				continue;

			//读到分割物体个数,即@total number开始，因此下一行应该紧跟模型个数
			if(*next =='@'&& *(next+1)=='t'&& *(next+2)=='o')
			{
				fgets(temp2, LINE_MAX_LEN, fpt);
				next = temp2;
				sscanf ( next, "%d",&modelNumber);
				continue;
			}
			
			//读到分割物体标签,即@tag开始
			//因此下一行应该紧跟标签，标签之间不能间断
			if(*next =='@'&& *(next+1)=='t'&& *(next+2)=='a')
			{
				temp4 = 0;
				while(fgets(temp2, LINE_MAX_LEN, fpt)!=NULL && *temp2 == '\0')
				{
					sscanf (temp2, "%d %s",&temp3,threedTag[temp4]);
					temp4 ++;
				}

				//如果读到的标签和物体个数不一致，说明数据有问题
				if(temp4!=modelNumber) return 0;

				//直接进入下一个循环
				continue;
			}

			//读取场景中物体的relationship矩阵
			//以@edge开头
			//因此下一行应该紧跟edge矩阵，标签之间不能间断
			if(*next =='@'&& *(next+1)=='e'&& *(next+2)=='d')
			{
				temp4 = 0; //用来记录行
				while(fgets(temp2, LINE_MAX_LEN, fpt)&& *temp2 == '\0')
				{
					next1 = temp2;
					temp5 = 0; //用来记录列
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
					
					//如果relationship数量不等于模型数，那么矩阵存在错误
					if(temp5!= modelNumber) return 0;
					temp4 ++;
				}

				//如果relationship数量不等于模型数，那么矩阵存在错误
				if(temp4!= modelNumber) return 0;

				//直接进入下一个循环
				continue;
			}//if

			//********************第三步***********************
			//比较照片场景和3D场景tag之间的相似度
			//用于查找两个场景之间相似的标签数量
			for(int i = 0; i< modelNumber; i++) //初始化变量
			{
				tagFlag[i] = 0;
			}
			temp3 = 0;
			//tag 比对
			for(int i = 0; i< twds.modelNum ;i++)
			{
				for(int j = 0; j< modelNumber ;j++)
				{
					//表示这个标签没有被使用且和照片标签一致
					if( (!tagFlag[j]) && compareTwoTag(twds.tag1[i],threedTag[j]) )
					{
						temp3 ++; //用于记录tag一致的个数
						tagFlag[j] =1; //说明第j个tag已经被使用，适用于有多个相同tag的情况
						break;
					}
				}
			}
			//计算tag相关度
			temp1 = (float)temp3/(modelNumber+twds.modelNum);
			//看看tag相关度是否低于阀值，如果低于，直接返回-1，这里讲阀值设置为0.2
			if(temp1 < 0.2)
			{
				return 0;
			}

			//********如果tag相似度超过阀值，开始读取3D场景被分割物体的描述子
			temp1 = modelNumber;
			for(int i=0; i<temp1; i++)
			{
				//从读的文件大小来说，这是用户输入图片的算子
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

			//用于将记录照片中每个物体相同标签个数的数组初始化
			for(int i = 0; i< MAXSEGMENTNUMBER; i++)
			{    sameTagCount[i] = 0;}

			//对于照片中每个物体，用来寻找并比较在3D场景中相同标签物体的geometry相似度
			for(int i = 0; i< twds.modelNum ;i++)
			{
				for(int j = 0; j< modelNumber ;j++)
				{
					//第一步找到标签
					if(compareTwoTag(twds.tag1[i],threedTag[j]) )
					{
						matresT[i][sameTagCount[i]].objectNum = j;
						matresT[i][sameTagCount[i]].similarity_G = MatchLF3(i,j);
						sameTagCount[i]++;

					}
				}
			}

			//********************第四步***********************
			//比较两个场景中相同标签的relationship和partner相似度
			//这里其实还存在一个多对多的问题，暂时先不考虑，简化处理
			//如果两个物体tag不相同，那么我们认为他们之间的相似度为0，所以先找到相似的tag，如果没有，直接跳过
			//1.对于照片中每个物体，找到其相同标签的物体
			//2.对于该物体所具有的所有<edge,partner>;先找对应物体相同的<edge,partner>
			//2.找相同关系的pair，pair之间的edge作为辅助
			//3.物体的texture和geometry
			for(int i = 0; i< modelNumber; i++) //初始化变量
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
				
				//如果该物体在被检索场景中有相同标签,假定当前物体为i
				if(!sameTagCount[i])
				{				
				    //1.遍历与i同tag的3D物体j
					for(int j=0; j<sameTagCount[i];j++)
					{
						//2.表示两个物体i,j的geometry、texture、tag相似度,其中a为权重
						tempObjSimilarity += a*matresT[i][j].similarity_G;

						//3.便利i和j所有长度为2的relationship，比较相似度
						for(int k = 0; k<twds.modelNum ;k ++)
						{
							if(twds.relationship[i][k]!='0')
							{
								for(int l = 0; l<modelNumber; l++)
								{
									if(twds.relationship[i][k]==relatinoship[j][l])
									    tempObjSimilarity +=c*1;
									//如果k和l的tag相同
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
							//将匹配结果乘以i和j的重要程度，然后累加到tempSimilarity
						tempObjSimilarity = tempObjSimilarity*twds.importance[i]*importance[j];
						tempSimilarity += tempObjSimilarity;
					}//for
				
				}//if
				
			}//for

			//如果当前检索到的结果小于10个，则++，否则topnum不变
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
				//给结果排个序，最后就用可以循序插入了
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
			else //当ptop被填满，开始插入
			{
				for(int i = 9; i>0;i--)
				{
					if(pTop[i].Precise>=tempSimilarity)
					{
						if(i==9)
							break;
						else
						{
							//整体往后移
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
				
			
		}//while -- 3D场景list里的某个场景

		//关闭用来开启读取某个2D场景的文件流
		fclose(fpt1);

	}//while -- 3D场景list里的所有场景

	//将检索结果输出到文件
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

//该函数不需要从文件读取照片的描述子
int Find3DSceneFromBuffer(TwoDScene twds,char* err,pSceneMatRes pSceneMatResult)
{
	// for region shape descriptor
	int			    TopNum;
	pMatRes		    pTop; //这是指单个物体的匹配度
	FILE			*fpt, *fpt1,*fpt2;
	char			filename[200],destfn[200];//srcfn用来保存用户输入的模型

	////******************保存照片描述子***********************
	//unsigned char	src_ArtCoeff2[MAXSEGMENTNUMBER][ART_COEF];     //用来保存一个照片中分割物体所有的描述子
	//unsigned char	src_FdCoeff_q82[MAXSEGMENTNUMBER][FD_COEF];    //保存照片场景的
	//unsigned char	src_CirCoeff_q82[MAXSEGMENTNUMBER];            //保存照片场景
	//unsigned char	src_EccCoeff_q82[MAXSEGMENTNUMBER];            //保存照片场景
	//
	////*******************保存3D场景分割结果描述子******************
	//unsigned char	dest_ArtCoeff2[MAXSEGMENTNUMBER][SRC_ANGLE][CAMNUM][ART_COEF];
	//unsigned char	dest_FdCoeff_q82[MAXSEGMENTNUMBER][ANGLE][CAMNUM][FD_COEF];
	//unsigned char	dest_CirCoeff_q82[MAXSEGMENTNUMBER][ANGLE][CAMNUM];
	//unsigned char	dest_EccCoeff_q82[MAXSEGMENTNUMBER][ANGLE][CAMNUM];

	char temp2[400]; //用来保存临时读取的scene.txt的每一行内容
	int temp3,temp5; //临时变量
	float temp4;
	char* next;
	char* next1;
	char threedTag[MAXSEGMENTNUMBER][30]; //用来保存三维场景物体tag的变量，假定物体个数不超过50，且tag长度不超过30
	int tagFlag[MAXSEGMENTNUMBER]; //用于tag比较时候的临时变量
	char relatinoship[MAXSEGMENTNUMBER][MAXSEGMENTNUMBER]; //用于临时保存三维场景中物体之间的relationship，假定物体不超过50个
	int edgeCount[10]; //用来记录某个物体所包含relationship的个数 辅助数组，因为总共就8种relationship
	float tempSimilarity; //用来临时存储两个场景之间的相似度
	float tempObjSimilarity; //用来临时存储两个物体之间的相似度
	MatResTemp matresT[MAXSEGMENTNUMBER][10]; //用来记录每个物体对应另一个场景中相同tag物体的相似度,同样假定物体不超过50个，每个物体相同标签的个数不超过10个
	int sameTagCount[MAXSEGMENTNUMBER]; //记录每个物体在被检索3D场景中有相似标签的物体个数
	int modelNumber; //用于记录被分割三维场景中物体个数
	float importance[MAXSEGMENTNUMBER]; //用于记录物体的重要程度，重要程度正比于物体大小和反比于相同tag物体个数
	vector<int> temp6; //辅助数组

	//****************最终similarity中各个部分之间的权重*******************
	float a = 0.5; //用来表示两个物体geometry相似度之间的权重
	float b = 0.5; //用来表示两个物体<edge,partner>相似度之间的权重
	float c = 0.25; //用来表示两个物体<edge,partner>相似度中edge的权重
	float d = 0.25; //用来表示两个物体<edge,partner>相似度中partner的权重
	//float e = 0.5; //用来表示物体重要程度中count部分的权重
	//float f = 0.5; //用来表示物体重要程度中size部分的权重

	pTop = (pMatRes) malloc (SCENEFORDISPLAY * sizeof(MatRes));  //保存匹配结果的变量分配,暂定选择10个

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
	//貌似在这里不需要了
	fpt = fopen("SearchConfigure\\align10.txt", "rb");
	if(fpt == NULL)
	{
		sprintf(err,"Can't open file : align10.txt! \n");
		return 0;
	}
	fread(align10, sizeof(unsigned char), 60*CAMNUM_2, fpt);
	fclose(fpt);

	//*********************读照片的分割结果的描述子******************************/
	int temp1 = twds.modelNum;

	if(temp1>MAXSEGMENTNUMBER)
	{
		err = "The segement number is max than MAXSEGMENTNUMBER!";
	}

	
	//**************读取3D场景中所有场景的描述子并进行匹配***************************/
    //具体顺序：
	//1.读取场景库中场景列表
	//2.按先后顺序读取场景列表中的场景，先读取场景信息scene.txt
	//3.先比较scene.txt与照片场景中tag重合度，如果重合度大于阀值，继续，否则返回2读取下一个场景
	//4.再比较两个场景中相同标签的relationship和partner（这里相同标签的定义可能需要斟酌）
	//5.将最终的相似度保存，返回2
	//6.结束

	//第一步：打开场景列表
	fpt = fopen("SearchConfigure\\SceneModelList.txt", "r");
	if(fpt == NULL)
	{
		sprintf(err,"Can't open file : SceneModelList.txt! \n");
		return 0;
	}

	//**********************第二步：循环读取场景列表中的场景信息************
	TopNum = 0; //记录已经检索到的场景的个数

	while( fgets(destfn, 200, fpt) )
	{
		//给读到的字符串加上结束符
		destfn[strlen(destfn)-1] = 0x00;

		//修改文件名，在名字最后加上.txt,用来读取场景信息
		sprintf(filename, "%s.txt", destfn);

		//如果无法读取，则读取下一个场景
		if( (fpt1 = fopen(filename, "rb")) == NULL )				
		{	continue;	}

		//**********************第二步*******************
		//读取3d场景中的信息scene.txt
		tempSimilarity = 0;
		while( fgets(temp2, LINE_MAX_LEN, fpt1) != NULL ) 
		{
			//过滤掉每行中的开头空白字符
			for ( next = temp2; *next != '\0' && isspace(*next); next++ )
				;
			
			//过滤掉注视行以及空白航
			if ( *next == '\0' || *next == '#')
				continue;

			//读到分割物体个数,即@total number开始，因此下一行应该紧跟模型个数
			if(*next =='@'&& *(next+1)=='t'&& *(next+2)=='o')
			{
				fgets(temp2, LINE_MAX_LEN, fpt1);
				next = temp2;
				sscanf ( next, "%d",&modelNumber);
				continue;
			}
			
			//读到分割物体标签,即@tag开始
			//因此下一行应该紧跟标签，标签之间不能间断
			if(*next =='@'&& *(next+1)=='t'&& *(next+2)=='a')
			{
	
				for(int i = 0;i< modelNumber ;i++)
				{
					if(fgets(temp2, LINE_MAX_LEN, fpt1))
					{
						sscanf (temp2, "%d %s %f",&temp3,threedTag[i],&importance[i]);
					}
					else//如果读到的标签和物体个数不一致，说明数据有问题
					{
                        return 0;
					}
				}
				//直接进入下一个循环
				continue;
			}

			//读取场景中物体的relationship矩阵
			//以@edge开头
			//因此下一行应该紧跟edge矩阵，标签之间不能间断
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
					else//如果读到的标签和物体个数不一致，说明数据有问题
					{
						return 0;
					}
					
				}

				//直接进入下一个循环
				continue;
			}//if
		}//while--读取场景的txt；

		fclose(fpt1);

		//********************第三步***********************
		//比较照片场景和3D场景tag之间的相似度
		//用于查找两个场景之间相似的标签数量
		for(int i = 0; i< modelNumber; i++) //初始化变量
		{
			tagFlag[i] = 0;
		}
		temp3 = 0;
		//tag 比对
		for(int i = 0; i< twds.modelNum ;i++)
		{
			for(int j = 0; j< modelNumber ;j++)
			{
				//表示这个标签没有被使用且和照片标签一致
				if( (!tagFlag[j]) && compareTwoTag(twds.tag1[i],threedTag[j]) )
				{
					temp3 ++; //用于记录tag一致的个数
					tagFlag[j] =1; //说明第j个tag已经被使用，适用于有多个相同tag的情况
					break;
				}
			}
		}
		//计算tag相关度
		temp4 = (float)temp3/(modelNumber+twds.modelNum);
		//看看tag相关度是否低于阀值，如果低于，直接进入下一个场景比较，这里讲阀值设置为0.2
		if(temp4 < 0.01)    
		{ continue; }

		//********如果tag相似度超过阀值，开始读取3D场景被分割物体的描述子
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

		//用于将记录照片中每个物体相同标签个数的数组初始化
		for(int i = 0; i< MAXSEGMENTNUMBER; i++)
		{    sameTagCount[i] = 0;}
		//用来计算3D场景中每个物体重要度的辅助数组
		for(int i = 0; i< modelNumber; i++) //初始化变量
		{  tagFlag[i] = 0; }

		//对于照片中每个物体，用来寻找并比较在3D场景中相同标签物体的geometry相似度
		for(int i = 0; i< twds.modelNum ;i++)
		{
			for(int j = 0; j< modelNumber ;j++)
			{
				//第一步找到标签
				if(compareTwoTag(twds.tag1[i],threedTag[j]) )
				{
					matresT[i][sameTagCount[i]].objectNum = j;
					matresT[i][sameTagCount[i]].similarity_G = MatchLF3(i,j);
					sameTagCount[i]++;
					temp6.push_back(j); //把每一个有相同标签的物体都记录下来
				}
			}

			//处理importance
			if(sameTagCount[i]) temp4 = 0.7f/sameTagCount[i] ;
		
			for(int k = 0; k<temp6.size()&&!tagFlag[temp6.at(k)];k++)
			{
					importance[temp6.at(k)] = importance[temp6.at(k)] *0.3;
				    importance[temp6.at(k)] += temp4;
				    tagFlag[temp6.at(k)] = 1;
			}
		}

		//********************第四步***********************
		//比较两个场景中相同标签的relationship和partner相似度
		//这里其实还存在一个多对多的问题，暂时先不考虑，简化处理
		//如果两个物体tag不相同，那么我们认为他们之间的相似度为0，所以先找到相似的tag，如果没有，直接跳过
		//1.对于照片中每个物体，找到其相同标签的物体
		//2.对于该物体所具有的所有<edge,partner>;先找对应物体相同的<edge,partner>
		//2.找相同关系的pair，pair之间的edge作为辅助
		//3.物体的texture和geometry
		for(int i = 0; i< modelNumber; i++) //初始化变量
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
				
			//如果该物体在被检索场景中有相同标签,假定当前物体为i
			if(sameTagCount[i])
			{				
				//1.遍历与i同tag的3D物体j
				for(int j=0; j<sameTagCount[i];j++)
				{
					//2.表示两个物体i,j的geometry、texture、tag相似度,其中a为权重
					tempObjSimilarity += a*matresT[i][j].similarity_G;

					//3.便利i和j所有长度为2的relationship，比较相似度
					for(int k = 0; k<twds.modelNum ;k ++)
					{
						if(twds.relationship[i][k]!='0')
						{
							for(int l = 0; l<modelNumber; l++)
							{
								if(twds.relationship[i][k]==relatinoship[j][l])
									tempObjSimilarity +=c*1;
								//如果k和l的tag相同
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
						//将匹配结果乘以i和j的重要程度，然后累加到tempSimilarity
					tempObjSimilarity = tempObjSimilarity*twds.importance[i]*importance[j];
					tempSimilarity += tempObjSimilarity;
				}//for
				
			}//if
				
		}//for

		//如果当前检索到的结果小于SCENEFORDISPLAY个，则++，否则topnum不变
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
			//给结果排个序，最后就用可以循序插入了
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
		else //当ptop被填满，开始插入
		{
			for(int i = SCENEFORDISPLAY-1; i>0;i--)
			{
				if(pTop[i].Precise>=tempSimilarity)
				{
					if(i==SCENEFORDISPLAY-1)
						break;
					else
					{
						//整体往后移
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
							

	}//while -- 3D场景list里的所有场景

	//将检索结果输出到文件
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

	//将检索结果保存到doc里头
	for(int i = 0; i< TopNum;i++)
	{
		strcpy(pSceneMatResult[i].name,pTop[i].name);
	}

	fclose(fpt);	
	free(pTop);

	return 1;
}

//用于检索时vector比较的辅助函数
bool compareTwoSearchHelp(const ThreeDSceneSearchHelp &h1, const ThreeDSceneSearchHelp &h2)
{
	return h1.modelTag <  h2.modelTag;
}


//用于场景检索最终检索结果的排序比较函数
bool compareSearchResult(const SceneMatRes &s1, const SceneMatRes &s2)
{
	return s1.similarity <  s2.similarity;
}


/************************************************************************/
/* 此函数和Find3DSceneFromBuffer的区别在于，
Find3DSceneFromBuffer使用的是第一版本的3D场景数据库 
而Search3DSceneFromBuffer使用的是第二版本的数据库
*/
/************************************************************************/
int Search3DSceneFromBuffer(TwoDScene twds,int **TwoDRelationship,char* err,pSceneMatRes pSceneMatResult)
{
	// for region shape descriptor
	int			    TopNum;
	pMatRes		    pTop; //这是指单个物体的匹配度
	FILE			*fpt, *fpt1,*fpt2;
	char			filename[200],destfn[200];//srcfn用来保存用户输入的模型

	////******************保存照片描述子***********************
	Scene tempScene;  //用于保存检索时数据库中的3D场景的临时变量
	vector<ThreeDSceneSearchHelp> twdSceneHelp;  //进行场景的描述子等比较时的辅助变量
	vector<ThreeDSceneSearchHelp> threedSceneHelp;
	char temp2[400]; //用来保存临时读取的scene.txt的每一行内容
	int temp3,temp5; //临时变量
	float temp4;
	char* next;
	char* next1;
	char threedTag[MAXSEGMENTNUMBER][30]; //用来保存三维场景物体tag的变量，假定物体个数不超过50，且tag长度不超过30
	int tagFlag[MAXSEGMENTNUMBER]; //用于tag比较时候的临时变量
	int **threeDRelatinoship; //用于临时保存三维场景中物体之间的relationship，假定物体不超过50个
	int edgeCount[10]; //用来记录某个物体所包含relationship的个数 辅助数组，因为总共就8种relationship
	float tempSimilarity; //用来临时存储两个场景之间的相似度
	float tempObjSimilarity; //用来临时存储两个物体之间的相似度
	MatResTemp matresT[MAXSEGMENTNUMBER][20]; //用来记录每个物体对应另一个场景中相同tag物体的相似度,同样假定物体不超过50个，每个物体相同标签的个数不超过10个
	int sameTagCount[MAXSEGMENTNUMBER]; //记录每个物体在被检索3D场景中有相似标签的物体个数
	int modelNumber; //用于记录被分割三维场景中物体个数
	float importance[MAXSEGMENTNUMBER]; //用于记录物体的重要程度，重要程度正比于物体大小和反比于相同tag物体个数
	vector<int> temp6; //辅助数组

	//****************最终similarity中各个部分之间的权重*******************
	float a = 5.0; //用来表示两个物体relationship中tag相似度权重
	float b = 3.0; //用来表示两个物体relationship中的relationship相似度权重
	float c = 2.0; //用来表示两个物体relationship中相同tag和relationship组的奖励值
	//float d = 0.25; //用来表示两个物体<edge,partner>相似度中partner的权重
	float e = 6.0; //用来表示两个物体geometry相似度部分的权重
	float f = 4.0; //用来表示两个物体relationship相似度部分的权重

	//pTop = (pMatRes) malloc (SCENEFORDISPLAY * sizeof(MatRes));  //保存匹配结果的变量分配,暂定选择10个
	vector<SceneMatRes> searchResult;          //保存检索结果
	SceneMatRes tempResult;

	//开辟保存3D场景relationship的内存
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
	//貌似在这里不需要了
	fpt = fopen("SearchConfigure\\align10.txt", "rb");
	if(fpt == NULL)
	{
		sprintf(err,"Can't open file : align10.txt! \n");
		return 0;
	}
	fread(align10, sizeof(unsigned char), 60*CAMNUM_2, fpt);
	fclose(fpt);

	//*********************读照片的分割结果的描述子******************************/
	int temp1 = twds.modelNum;

	if(temp1>MAXSEGMENTNUMBER)  //默认被分割场景中物体个数不超过50个
	{
		err = "The segement number is max than MAXSEGMENTNUMBER!";
	}

	//数据准备
	ThreeDSceneSearchHelp tt;  //场景数据转换的辅助变量
	for(int i = 0; i< twds.modelNum ;i++) //用于获取2D场景中的所有tag
	{	
		tt.modelIndex = i;
		tt.modelTag = string(twds.tag1[i]);
		tt.sameTageCount = 1;  //用于初始化

		//处理每个物体拥有的relationship以及relationship另外一头的tag
		for(int j = 0; j<twds.modelNum ; j++)
		{
			if(TwoDRelationship[i][j]!= 0)  //'0'默认为无关系
			{
				tt.relationships.push_back( TwoDRelationship[i][j]);
				tt.relationshipTag.push_back( twds.tag1[j]);
			}
		}
		twdSceneHelp.push_back(tt);
	}
	sort(twdSceneHelp.begin(),twdSceneHelp.end(),compareTwoSearchHelp);  //升序排序

	//**************读取新的3D场景中所有场景的描述子并进行匹配***************************/
	//具体顺序：
	//1.读取场景库中场景列表,其中场景列表保存在NewSceneModelList.txt（对于后续分类型的检索可以分文件夹检索）
	//2.按先后顺序读取场景列表中的场景，《--先读取场景信息scene.txt；--》此处改成读取场景的conference_room26.fbx
	//3.先比较scene.txt与照片场景中tag重合度，如果重合度大于阀值，继续，否则返回2读取下一个场景
	//4.再比较两个场景中相同标签的relationship和partner（这里相同标签的定义可能需要斟酌）
	//5.将最终的相似度保存，返回2
	//6.结束

	//第一步：打开场景列表
	fpt = fopen("SearchConfigure\\NewSceneModelList.txt", "r");
	if(fpt == NULL)
	{
		sprintf(err,"Can't open file : NewSceneModelList.txt! \n");
		return 0;
	}

	//**********************第二步：循环读取场景列表中的场景信息************
	TopNum = 0; //记录已经检索到的场景的个数
	while( fgets(destfn, 200, fpt) )
	{
		//给读到的字符串加上结束符
		destfn[strlen(destfn)-1] = 0x00;

		//修改文件名，在名字最后加上Relationship.cng,用来读取场景tag和relationship信息
		sprintf(filename, "%sRelationship.cng", destfn);
		string filePath(filename); //将char* 转string
		tempScene.LightReadRelationFile(filePath);//通过调用Scene中的函数来读取3D场景信息
		//为threeDRelatinoship初始化
		for(int i = 0; i< MAXSEGMENTNUMBER; i++)
		{
			memset(threeDRelatinoship[i],0,MAXSEGMENTNUMBER*4);
		}
		tempScene.BuildRelationTable1(threeDRelatinoship);   //为3D场景建立relationship table

		//********************第三步***********************
		/************************************************************************/
		/*比较照片场景和3D场景tag之间的相似度,方法将2D场景和3D场景中的标签都存入一个vector<string>中
		  随后对两个vector进行排序，然后从头比到结束就可以了*/
		/************************************************************************/

		//*****************因为该部分只需要针对2D场景提取一次，所以应该放到while循环外面
		//ThreeDSceneSearchHelp tt;  //场景数据转换的辅助变量
		//for(int i = 0; i< twds.modelNum ;i++) //用于获取2D场景中的所有tag
		//{	
		//	tt.modelIndex = i;
		//	tt.modelTag = string(twds.tag1[i]);
		//	twdSceneHelp.push_back(tt);
		//}
		//sort(twdSceneHelp.begin(),twdSceneHelp.end(),compareTwoSearchHelp);  //升序排序
		   
		for(int i = 0; i< tempScene.modelSize ;i++)//用于获取3D场景中的所有tag
		{
			importance[i] = 1; //暂时不对importance最处理，后面会根据tag进行处理
			tt.modelIndex = i;
			tt.modelTag = tempScene.lightSceneModels[i].modelTag;
			tt.sameTageCount = 1; //用于初始化

			//处理每个物体拥有的relationship以及relationship另外一头的tag
			for(int j = 0; j<tempScene.modelSize ; j++)
			{
				if(threeDRelatinoship[i][j]!= '0')  //'0'默认为无关系
				{
					tt.relationships.push_back( threeDRelatinoship[i][j] - '0');
					tt.relationshipTag.push_back( tempScene.lightSceneModels[j].modelTag);
				}
			}

			threedSceneHelp.push_back(tt);
		}
		sort(threedSceneHelp.begin(),threedSceneHelp.end(),compareTwoSearchHelp);  //升序排序

		temp3 = 0; //用来记录两个场景中相同标签的个数
		int tempI = 0; //用来标识twdSceneTag中元素的序号临时变量
		int tempJ = 0; //用来标识threedSceneTag中元素的序号临时变量
		while(tempI< twds.modelNum && tempJ< tempScene.modelSize) //进行比较
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

		//计算tag相关度（此处值得商榷，因为目标场景中的物体可能被剔除，所以可能影响不大）
		temp4 = (float)temp3/(tempScene.modelSize + twds.modelNum);
		//看看tag相关度是否低于阀值，如果低于，直接进入下一个场景比较，这里讲阀值设置为0.2(测试的时候用0.01)
		if(temp4 < 0.1)    
		{ 
			//将循环使用的变量清空，恢复原值
			//1.清空threedSceneHelp
			threedSceneHelp.clear();
			//2.清空tempScene
			tempScene.lightSceneModels.clear();
			tempScene.RelationMap.clear();
			continue;
		}

		//分别计算2D和3D场景中相同tag的模型的个数
		tempI = 1;
		while (tempI< twds.modelNum)
		{
			if(twdSceneHelp[tempI].modelTag ==twdSceneHelp[tempI-1].modelTag)
			{
				//如果与前一个tag相同，则数目增1
				twdSceneHelp[tempI].sameTageCount = twdSceneHelp[tempI-1].sameTageCount +1;
			}
			else
			{
				//逆序去给每个模型都赋值相同tag个数
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
				//逆序去给每个模型都赋值相同tag个数
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
				//如果与前一个tag相同，则数目增1
				threedSceneHelp[tempI].sameTageCount = threedSceneHelp[tempI-1].sameTageCount +1;
			}
			else
			{
				//逆序去给每个模型都赋值相同tag个数
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
				//逆序去给每个模型都赋值相同tag个数
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

		////记录下与每个2D场景中物体tag相同的3D场景中的物体的index
		//tempI = 0;
		//tempJ = 0;
		//while(tempI< twds.modelNum && tempJ< tempScene.modelSize)
		//{
		//	//如果2D场景中当前物体和前一个物体的tag相同，则可以直接复制前一个物体的sameTagIndex
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
		//	//如果当前2D场景中的物体和3D场景中物体tag相同，则讲对方添加到自己的sameTagIndex中，并且在3D场景的物体列表中继续往后搜索
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


		////记录下与每个3D场景中物体tag相同的2D场景中的物体的index
		//tempI = 0;
		//tempJ = 0;
		//while(tempJ< twds.modelNum && tempI< tempScene.modelSize)
		//{
		//	//如果3D场景中当前物体和前一个物体的tag相同，则可以直接复制前一个物体的sameTagIndex
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
		//	//如果当前2D场景中的物体和3D场景中物体tag相同，则讲对方添加到自己的sameTagIndex中，并且在3D场景的物体列表中继续往后搜索
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


		//记录每个2D场景中物体和3D场景中相同标签物体的相似度以及3D场景中每个物体的importance
		//随后再获取每个2D场景中的物体所能从3D场景中相同tag物体处获取的最大相似度
		//最终累积到两个场景的相似度之中
		int tempIndexi = 0;
		int tmepIndexj = 0;
		tempSimilarity = 0.0f; //保存2D场景和3D场景的总体相似度
		float tempObjMaxSimilarity ; //用于保存2D场景中某个物体和3D场景中相同tag物体能获得的当前最大相似度
		float tempRelationshipSimilarity; //用于临时保存两个物体的relationship相似度
		int sameTagCount;
		int sameRelationshipCount;
		int sameTagAndRelCount;
		int tempSum;
		tempI = 0;
		tempJ = 0;
		bool firstStepInTag = true;    //用来记录是否是第一次碰到相同tag的物体，是就是true， 不是就是false；
		int sameTagBegineIndex = 0;     //保存相同tag的3D场景的物体的index开始位置
		
		//计算每一个2D场景中的物体与3D场景中物体的最大相似度，并累加到总体的场景相似度中
		for(tempI = 0; tempI < twds.modelNum; tempI++)
		{
			//首先寻找相同tag的3D场景中的物体位置
			tempJ = 0;
			while( tempJ < tempScene.modelSize && twdSceneHelp[tempI].modelTag > threedSceneHelp[tempJ].modelTag )
			{
				tempJ ++;
			}
			//说明index为tempI的2D场景中的物体在3D场景中没有与之tag相同的物体
			if(tempJ < tempScene.modelSize && twdSceneHelp[tempI].modelTag < threedSceneHelp[tempJ].modelTag)
			{
				continue;
			}

			//计算与index为tempI的2D场景中的物体相同tag的所有物体相似度，并取最大值累加到场景相似度
			tempObjSimilarity = 0.0f;  //保存2D场景中物体能与3D场景中相同tag物体所获得的最大相似度
			tempIndexi = twdSceneHelp[tempI].modelIndex;  //记录当前2D场景中物体序号
			while(tempJ< tempScene.modelSize && twdSceneHelp[tempI].modelTag == threedSceneHelp[tempJ].modelTag)
			{
				tempObjMaxSimilarity = 0.0f;
				tempRelationshipSimilarity = 0.0f;

				//获取月tempI相同tag的3D场景中的物体的index
				tmepIndexj = threedSceneHelp[tempJ].modelIndex;

				//1.几何相似度
				tempObjMaxSimilarity += e*MatchLF3(tempIndexi,tmepIndexj); 

				//2.relationship相似度
				//2.1 tag相似度 和 2.2 relationshipTag相似度 和2.3 奖励值
				sameTagCount = 0;
				sameRelationshipCount = 0;
				sameTagAndRelCount = 0;
				bool relationBool = false;   //以检验2D场景中的某个物体的relationship对应的relationshipTag是否已经在3D场景中对应物体tempJ的relationship中找到了相同的relationshipTag
				bool tagBool = false;  //以检验2D场景中的某个物体的relationship对应的tag是否已经在3D场景中对应物体tempJ的relationship中找到了相同的tag
				//统计相同的tag以及relationshiptag
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

				//两个场景中相同tag的物体的个数之和
				tempSum = twdSceneHelp[tempI].relationshipTag.size() + threedSceneHelp[tempJ].relationshipTag.size();

				//保证被检索的物体有relationship
				if(tempSum > 0)
				{
					tempRelationshipSimilarity +=  a*(sameTagCount*2/ (float)tempSum) ;  //2.1 tag相同值
					tempRelationshipSimilarity +=  b*(sameRelationshipCount*2/ (float)tempSum) ;  //2.2 relationship相同值
					tempRelationshipSimilarity +=  c*(sameTagAndRelCount*2/ (float)tempSum) ;  //2.2  奖励值
				}

				tempObjMaxSimilarity += f*tempRelationshipSimilarity;
				//importance
				tempObjMaxSimilarity = tempObjMaxSimilarity*twds.importance[tempIndexi]*importance[tmepIndexj]; 
				//惩罚值，即相似模型个数，重要程度越低
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

				//保存最大的相似度
				tempObjSimilarity = tempObjMaxSimilarity > tempObjSimilarity? tempObjMaxSimilarity:tempObjSimilarity;

				tempJ ++ ;  //匹配下一个与之相似的物体
			}//while

			//将当前物体tempI能找到的最大相似度加入到场景的总相似度中
			tempSimilarity += tempObjSimilarity;

		}//for

	
		//再将当前的检索结果插入检索结果队列
		//将全部结果插入vector，然后再排序，取出最匹配的《这里的最匹配是指最大的》
		tempResult.similarity = tempSimilarity;
		strcpy(tempResult.name, destfn);
		searchResult.push_back(tempResult);	

		//将循环使用的变量清空，恢复原值
		//1.清空threedSceneHelp
		threedSceneHelp.clear();
		//2.清空tempScene
		tempScene.lightSceneModels.clear();
		tempScene.RelationMap.clear();

		//已经检索到的场景的个数
		TopNum++;
	}//while -- 3D场景list里的所有场景

	//对检索结果进行排序
	sort(searchResult.begin(),searchResult.end(),compareSearchResult);  //升序排序

	//将检索结果输出到文件
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

	////因为后面直接从文件读取了结果，就不在这里存入变量之中了
	////将检索结果保存到
	//for(int i = TopNum-1; i >= 0; i--)
	//{
	//	strcpy(pSceneMatResult[i].name,searchResult[i].name);
	//}

	//释放保存3D场景的内存
	for(int i = 0; i<MAXSEGMENTNUMBER; i++)
	{
		delete [] threeDRelatinoship[i];
		threeDRelatinoship[i] = NULL;

	}
	delete [] threeDRelatinoship ;
	threeDRelatinoship = NULL;

	return TopNum;
}

