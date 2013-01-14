/************************************************************************/
/* it's used by 3D feature extract   
��ʵ������ļ�û�б��õ��������ñ�����һ���ӿڴ��棬���Խ��䱸ע�Ƴ�
/************************************************************************/

//#include "stdafx.h"


#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <time.h>
#include <float.h>
#include <string.h>
#include <limits.h>
#include <vector>

#include "ds.h"
#include "RWObj.h"
#include "Bitmap.h"
#include "TranslateScale.h"
#include "Rotate.h"
#include "RegionShape.h"
#include "RecovAffine.h"
#include "Refine.h"
#include "Edge.h"
#include "convert.h"
#include "ColorDescriptor.h"
#include "Circularity.h"
#include "FourierDescriptor.h"
#include "Eccentricity.h"
#include "FeatureExtract.h"

#include "glut.h"
#include <gl/gl.h>
#include <gl/glu.h>

#define abs(a) (a>0)?(a):-(a)

#define	QUANT8				256		// 2^8
#define FD_SCALE			2		// *2 first, and then quantization
#define CIR_SCALE			2.318181818		// the range of circularity is [0~110], so *2.318 to be [0~255]
#define ECC_SCALE			25.5			// the range of circularity is [0~10], so *25.5 to be [0~255]

unsigned char	CamMap[CAMNUM_2]={0,1,2,3,4,5,6,7,8,9,5,6,7,8,9,2,3,4,0,1};

char srcfn[100];
char destfn[100];

int			winw = WIDTH, winh = HEIGHT;

// translate and scale of model 1 and 2
Ver				Translate1, Translate2;
double			Scale1, Scale2;

//����������Ƭ������������ȡ����ı���
extern unsigned char	src_ArtCoeff2[][ART_COEF];     //��������һ����Ƭ�зָ��������е�������
extern unsigned char	src_FdCoeff_q82[][10];         //������Ƭ������
extern unsigned char	src_CirCoeff_q82[];            //������Ƭ����
extern unsigned char	src_EccCoeff_q82[];            //������Ƭ����

//extern unsigned char	src_ArtCoeff2[MAXSEGMENTNUMBER][ART_COEF];     //��������һ����Ƭ�зָ��������е�������
//extern unsigned char	src_FdCoeff_q82[MAXSEGMENTNUMBER][FD_COEF];    //������Ƭ������
//extern unsigned char	src_CirCoeff_q82[MAXSEGMENTNUMBER];            //������Ƭ����
//extern unsigned char	src_EccCoeff_q82[MAXSEGMENTNUMBER];            //������Ƭ����

//����������Ƭ������������ȡ����ı���
extern unsigned char	src_ArtCoeff1[ART_COEF];     //��������һ����Ƭ�зָ��������е�������
extern unsigned char	src_FdCoeff_q81[10];    //������Ƭ������
extern unsigned char	src_CirCoeff_q81;            //������Ƭ����
extern unsigned char	src_EccCoeff_q81;            //������Ƭ����

void FindCenter(unsigned char *srcBuff, int width, int height, double *CenX, double *CenY)
{
	int					x, y, count;
	unsigned char		*pImage;
	int					maxX, minX, maxY, minY;
	//int					MeanX, MeanY; 

	count = 0;
	pImage = srcBuff;

	// ***********************************************************************************************
	// if use "mean" to each 2D shape independnetly, the origin will be moved a lot in 3D
	// if ues "center" to each 2D shape independnetly, the origin will be moved only a little in 3D
	// if center can be defined in 3D, the origin will not be moved any more.
	// But this will not very robust in 3D similarity transformation
	// In addition, to make center of each 2D shape more close to user drawn 2D shapes,
	// it's better to define center for each 2D shape independently

	// uee center of max and min to be center
	maxX = maxY = -1;
	minX = minY = INT_MAX;
	for (y=0 ; y<height ; y++)
	for (x=0 ; x<width; x++)
	{
		if( *pImage < 255 )
		{
			if( x > maxX ) maxX = x;
			if( x < minX ) minX = x;
			if( y > maxY ) maxY = y;
			if( y < minY ) minY = y;
		}
		pImage++;
	}

	if( maxX > 0 )
	{
		*CenX = (maxX+minX) / 2.0;
		*CenY = (maxY+minY) / 2.0;
	}
	else
		*CenX = *CenY = -1;		// nothing to be rendered

	// use mean to be center
/*	count = 0;
	MeanX = MeanY = 0;
	for (y=0 ; y<height ; y++)
	for (x=0 ; x<width; x++)
	{
		if( *pImage < 255 )
		{
			MeanX += x;
			MeanY += y;
			count ++;
		}
		pImage++;
	}

	if( count > 0 )
	{
		*CenX = (double)MeanX / (double)count;
		*CenY = (double)MeanY / (double)count;
	}
	else
		*CenX = *CenY = -1;		// nothing to be rendered
*/
}




int FeatureExtractByFile(char* err, CSegObject &object)
{
	unsigned char   *EdgeBuff, *YuvBuff;
	unsigned char	*srcBuffer;
	char			filename[400];
	FILE			*fpt, *fpt1, *fpt_art_q8, *fpt_art_q4, *fpt_fd_q8,  *fpt_cir_q8, *fpt_ecc_q8;//, *fpt_ccd;
	int				i, j, k, p, r, a, itmp;
	static int		UseCam = 2;
	// for region shape descriptor
	//double			src_ArtCoeff[ANGLE][CAMNUM][ART_ANGULAR][ART_RADIAL];
	double			src_ArtCoeff[ART_ANGULAR][ART_RADIAL];
	//double			dest_ArtCoeff[ANGLE][CAMNUM][ART_ANGULAR][ART_RADIAL];
	//unsigned char	q8_ArtCoeff[ANGLE][CAMNUM][ART_COEF];
	unsigned char	q8_ArtCoeff[ART_COEF];
	//unsigned char	q4_ArtCoeff[ANGLE][CAMNUM][ART_COEF_2];//ART_COEF_2==18
	unsigned char	q4_ArtCoeff[ART_COEF_2];
	// for color decsriptor
	//unsigned __int64 CompactColor[ANGLE][CAMNUM];	// 63 bits for each image
	//unsigned __int64 dest_CompactColor[ANGLE][CAMNUM];	// 63 bits for each image
	// for circularity
	//double			cir_Coeff[ANGLE][CAMNUM];
	double			cir_Coeff;
	//unsigned char	q8_cirCoeff[ANGLE][CAMNUM];
	unsigned char	q8_cirCoeff;
	// for fourier descriptor
	//double			src_FdCoeff[ANGLE][CAMNUM][FD_COEFF_NO], dest_FdCoeff[ANGLE][CAMNUM][FD_COEFF_NO];
	double			src_FdCoeff[FD_COEFF_NO];
	//unsigned char	q8_FdCoeff[ANGLE][CAMNUM][FD_COEFF_NO];
	unsigned char	q8_FdCoeff[FD_COEFF_NO]; //src_FdCoeff��q8_FdCoeff֮��Ĺ�ϵ�ǣ�ǰ��Ϊ����õ���ֵ������Ϊǰ�߽��й�һ���ֵ
	sPOINT			*Contour;
	unsigned char	*ContourMask;
	// for eccentricity
	//double			ecc_Coeff[ANGLE][CAMNUM];
	double			ecc_Coeff;
	//unsigned char	q8_eccCoeff[ANGLE][CAMNUM], dest_eccCoeff[ANGLE][CAMNUM];
	unsigned char	q8_eccCoeff;
	// for compare
	int				Count;
	// quantization version
	char			fname[400];
	int				high, low, middle;
	double			QuantTable[17] = {	0.000000000, 0.003585473, 0.007418411, 0.011535520, 
										0.015982337, 0.020816302, 0.026111312, 0.031964674, 
										0.038508176, 0.045926586, 0.054490513, 0.064619488, 
										0.077016351, 0.092998687, 0.115524524, 0.154032694, 1.000000000};
	double			CenX, CenY;
	int				total;


	// initialize ART
	GenerateBasisLUT();
	
	//Ϊ��Ƭ�����ڴ�ռ�
	srcBuffer = (unsigned char *) malloc (winw * winh * sizeof(unsigned char));

	YuvBuff = (unsigned char *) malloc (3 * winw * winh * sizeof(unsigned char));
	
	// add edge to test retrieval
	//ò�Ʋ�Ҫ�õ�
	EdgeBuff = (unsigned char *) malloc (winw * winh * sizeof(unsigned char));
	
	
	// for Fourier Descriptor
	total = winw * winh;
	Contour = (sPOINT *) malloc( total * sizeof(sPOINT));
	ContourMask = (unsigned char *) malloc( total * sizeof(unsigned char));
	
	//�������buffer�����Ƿ�ɹ�
	if(!srcBuffer||!YuvBuff||!EdgeBuff ||!Contour ||!ContourMask )
	{
		sprintf(err,"buffer allocate failed! \n"); 
	     return 0;
	}

	//������Ƭ�ĵ�ַ�б�
	fpt1 = fopen("SearchConfigure\\CompareSource.txt", "r");//���ﲻ��ʹ��rb����֪����ʲôԭ��
	
	//ÿ�δ�list���һ���ļ���
	while( fgets(fname, 400, fpt1) )
	{
		// record execute time --- start
		//start = clock();
	
		//�����1����Ϊÿһ�к�������������
		//fname[strlen(fname)-1] = '\0';
		fname[strlen(fname)-1] = '\0';
		//sprintf(fname, "%s", fname);

		// get the translatation and scale of the two model
		//if(!ReadPicture(fname,srcBuff[0],winw,winh,CenX,CenY))
		if(!ReadPictureByOpencv(srcBuffer,winw,winh,object))
		{
			sprintf(err,"Picture read failed! \n"); 
			return 0;
		}	
		// find center for each shape
		FindCenter(srcBuffer, winw, winh, &CenX, &CenY);
	
		// get Zernike moment
		FindRadius2(srcBuffer, &CenX, &CenY);
	
		//ExtractCoefficients(srcBuffer, src_ArtCoeff[0][0], EdgeBuff, CenX, CenY);
		ExtractCoefficients(srcBuffer, src_ArtCoeff, EdgeBuff, CenX, CenY);

		FourierDescriptor(src_FdCoeff, srcBuffer, winw, winh, Contour, ContourMask, CenX, CenY);
	
		// get eccentricity
		//ecc_Coeff[0][0] = Eccentricity(srcBuffer, winw, winh, CenX, CenY);
	    ecc_Coeff = Eccentricity(srcBuffer, winw, winh, CenX, CenY);

		// get circularity
		EdgeDetectSil(EdgeBuff, srcBuffer, winw, winh);
		cir_Coeff = Circularity(srcBuffer, winw, winh, EdgeBuff);

	
	
	
		//for(i=0; i<1; i++)
			//for(j=0; j<1; j++)
			//{
				// the order is the same with that defined in MPEG-7, total 35 coefficients
				k = 0;
				p = 0;
	
				//��k��0�䵽��2���൱�ڽ�q8_ArtCoeff[i][j][k] �е�ǰ�����������˸�ֵ
				//��֪���������
				for(r=1 ; r<ART_RADIAL ; r++, k++)
				{
					//itmp = (int)(QUANT8 *  src_ArtCoeff[i][j][p][r]);
					itmp = (int)(QUANT8 *  src_ArtCoeff[p][r]);
					if(itmp>255)
						//q8_ArtCoeff[i][j][k] = 255;
						q8_ArtCoeff[k] = 255;
					else
						//q8_ArtCoeff[i][j][k] = itmp;
						q8_ArtCoeff[k] = itmp;
				}
	
				//����k��2��ʼ�ˣ�p�Ǵ�1��ʼ��
				for(p=1; p<ART_ANGULAR ; p++)
					for(r=0 ; r<ART_RADIAL ; r++, k++)
					{
						//itmp = (int)(QUANT8 *  src_ArtCoeff[i][j][p][r]);
						itmp = (int)(QUANT8 *  src_ArtCoeff[p][r]);
						if(itmp>255)
							//q8_ArtCoeff[i][j][k] = 255;
							q8_ArtCoeff[k] = 255;
						else
							q8_ArtCoeff[k] = itmp;
					}
	//}
			// save to disk
			//fwrite(q8_ArtCoeff, sizeof(unsigned char), ANGLE * CAMNUM * ART_COEF, fpt_art_q8);
			//sprintf(filename, "%s_q8_v1.8.art", fname);
			sprintf((char*)object.filename.c_str(), "%s_q8_v1.8.art", fname);

			if( (fpt = fopen(filename, "wb")) == NULL )	{	sprintf(err,"Write %s error!", filename);	return 0;	}
			//fwrite(q8_ArtCoeff, sizeof(unsigned char), ANGLE * CAMNUM * ART_COEF, fpt);
			fwrite(q8_ArtCoeff, sizeof(unsigned char), ART_COEF, fpt);
			fclose(fpt);
	
			// non-linear Quantization to 4 bits for each coefficient using MPEG-7 quantization table
			//for(i=0; i<ANGLE; i++)
			//	for(j=0; j<CAMNUM; j++)
			//for(i=0; i<1; i++)
				//for(j=0; j<1; j++)
				//{
					// the order is the same with that defined in MPEG-7, total 35 coefficients
					k = 0;
					p = 0;
					for(r=1 ; r<ART_RADIAL ; r++, k++)
					{
						high = 17;
						low = 0;
						while(high-low > 1)
						{
							middle = (high+low) / 2;
	
							//if(QuantTable[middle] < src_ArtCoeff[i][j][p][r])
							if(QuantTable[middle] < src_ArtCoeff[p][r])
								low = middle;
							else
								high = middle;
						}
						q8_ArtCoeff[k] = low;
					}
					for(p=1; p<ART_ANGULAR ; p++)
						for(r=0 ; r<ART_RADIAL ; r++, k++)
						{
							high = 17;
							low = 0;
							while(high-low > 1)
							{
								middle = (high+low) / 2;
	
								//if(QuantTable[middle] < src_ArtCoeff[i][j][p][r])
								if(QuantTable[middle] < src_ArtCoeff[p][r])
									low = middle;
								else
									high = middle;
							}
							//q8_ArtCoeff[i][j][k] = low;
							q8_ArtCoeff[k] = low;
						}
				//} 
	
				//for(i=0; i<ANGLE; i++)
				//	for(j=0; j<CAMNUM; j++)
				//for(i=0; i<1; i++)
					//for(j=0; j<1; j++)
						for(k=0, a=0; k<ART_COEF; k+=2, a++) //k-36 -a-18
							if( k+1 < ART_COEF )
								//q4_ArtCoeff[i][j][a] = ( (q8_ArtCoeff[i][j][k] << 4) & 0xf0 ) | 
								//( q8_ArtCoeff[i][j][k+1] & 0x0f );
								q4_ArtCoeff[a] = ( (q8_ArtCoeff[k] << 4) & 0xf0 ) | 
								( q8_ArtCoeff[k+1] & 0x0f );
							else
								//q4_ArtCoeff[i][j][a] = ( (q8_ArtCoeff[i][j][k] << 4) & 0xf0 );
								q4_ArtCoeff[a] = ( (q8_ArtCoeff[k] << 4) & 0xf0 );
	
				// save to disk
				//fwrite(q4_ArtCoeff, sizeof(unsigned char), ANGLE * CAMNUM * ART_COEF_2, fpt_art_q4);
				sprintf(filename, "%s_q4_v1.8.art", fname);
				if( (fpt = fopen(filename, "wb")) == NULL )	{	sprintf(err,"Write %s error!", filename);	return 0;	}
				fwrite(q4_ArtCoeff, sizeof(unsigned char), ART_COEF_2, fpt);
				fclose(fpt);
	
				// **********************************************************************
				// save color descriptor to disk
				//			fwrite(CompactColor, sizeof(unsigned char), ANGLE * CAMNUM * sizeof(unsigned __int64), fpt_ccd);
				//			sprintf(filename, "%s.ccd", fname);
				//			if( (fpt = fopen(filename, "wb")) == NULL )	{	printf("Write %s error!!\n", filename);	return;	}
				//			fwrite(CompactColor, sizeof(unsigned char), ANGLE * CAMNUM * sizeof(unsigned __int64), fpt);
				//			fclose(fpt);
	
				// **********************************************************************
				// save circularity feature to file
				//			sprintf(filename, "%s.cir", fname);
				//			fpt = fopen(filename, "wb");
				//			fwrite(cir_Coeff, ANGLE * CAMNUM, sizeof(double), fpt);
				//			fclose(fpt);
	
				// linear Quantization to 8 bits for each coefficient
				//for(i=0; i<ANGLE; i++)
				//	for(j=0; j<CAMNUM; j++)
				//for(i=0; i<1; i++)
					//for(j=0; j<1; j++)
					//{
						itmp = (int)(QUANT8 *  cir_Coeff);
						//if(itmp>255)		q8_cirCoeff[i][j] = 255;
						//else				q8_cirCoeff[i][j] = itmp;
						if(itmp>255)		q8_cirCoeff = 255;
						else				q8_cirCoeff = itmp;
					//}
					// save to disk
					//fwrite(q8_cirCoeff, sizeof(unsigned char), ANGLE * CAMNUM, fpt_cir_q8);
					sprintf(filename, "%s_q8_v1.8.cir", fname);
					if( (fpt = fopen(filename, "wb")) == NULL )	{	sprintf(err,"Write %s error!", filename);	return 0;	}
					fwrite(&q8_cirCoeff, sizeof(unsigned char), 1, fpt);
					fclose(fpt);
	
					// **********************************************************************
					// save eccentricity feature to file
					// linear Quantization to 8 bits for each coefficient
					//for(i=0; i<ANGLE; i++)
					//	for(j=0; j<CAMNUM; j++)
					//for(i=0; i<1; i++)
						//for(j=0; j<1; j++)
						//{
							//itmp = (int)(QUANT8 * ecc_Coeff[i][j]);
							itmp = (int)(QUANT8 * ecc_Coeff);
							//if(itmp>255)		q8_eccCoeff[i][j] = 255;
							//else				q8_eccCoeff[i][j] = itmp;
							if(itmp>255)		q8_eccCoeff = 255;
							else				q8_eccCoeff = itmp;
						//}
						// save to disk
						//fwrite(&q8_eccCoeff, sizeof(unsigned char), 1, fpt_ecc_q8);
						sprintf(filename, "%s_q8_v1.8.ecc", fname);
						if( (fpt = fopen(filename, "wb")) == NULL )	{	sprintf(err,"Write %s error!", filename);	return 0;	}
						fwrite(&q8_eccCoeff, sizeof(unsigned char), 1, fpt);
						fclose(fpt);
	
						// **********************************************************************
						// save Fourier descriptor feature to file
						//			fwrite(src_FdCoeff, ANGLE * CAMNUM * FD_COEFF_NO, sizeof(double), fpt_fd);
						//			sprintf(filename, "%s.fd", fname);
						//			fpt = fopen(filename, "wb");
						//			fwrite(src_FdCoeff, ANGLE * CAMNUM * FD_COEFF_NO, sizeof(double), fpt);
						//			fclose(fpt);
	
						//for(i=0; i<ANGLE; i++)
						//	for(j=0; j<CAMNUM; j++)
						//for(i=0; i<1; i++)
							//for(j=0; j<1; j++)
							//{
								for(k=0; k<FD_COEFF_NO; k++)
								{
									//itmp = (int)(QUANT8 * FD_SCALE * src_FdCoeff[i][j][k]);
									itmp = (int)(QUANT8 * FD_SCALE * src_FdCoeff[k]);
									if(itmp>255)
										//q8_FdCoeff[i][j][k] = 255;
										q8_FdCoeff[k] = 255;
									else
										//q8_FdCoeff[i][j][k] = itmp;
										q8_FdCoeff[k] = itmp;
								}
							//}
							//һ���ǽ�������������д��һ��
							//fwrite(q8_FdCoeff,  FD_COEFF_NO, sizeof(unsigned char), fpt_fd_q8);
							//һ���ǽ�������������д�������ļ�
							sprintf(filename, "%s_q8_v1.8.fd", fname);
							if((fpt = fopen(filename, "wb"))==NULL){sprintf(err,"Write %s error!", filename);	return 0;	};
							fwrite(q8_FdCoeff, FD_COEFF_NO, sizeof(unsigned char), fpt);
							fclose(fpt);

						    //printf("%d:The process of %s is finished! ", Count++,fname);
	}
	
	free(srcBuffer);
	free(YuvBuff);
	free(EdgeBuff);
	free(Contour);
	free(ContourMask);
	fclose(fpt1);

	return 1;
}



int FeatureExtract(char* err, CSegObject &object)
{
	unsigned char   *EdgeBuff, *YuvBuff;
	unsigned char	*srcBuffer;
	char			filename[400];
	FILE			*fpt, *fpt1, *fpt_art_q8, *fpt_art_q4, *fpt_fd_q8,  *fpt_cir_q8, *fpt_ecc_q8;//, *fpt_ccd;
	int				i, j, k, p, r, a, itmp;
	static int		UseCam = 2;
	// for region shape descriptor
	//double			src_ArtCoeff[ANGLE][CAMNUM][ART_ANGULAR][ART_RADIAL];
	double			src_ArtCoeff[ART_ANGULAR][ART_RADIAL];
	//double			dest_ArtCoeff[ANGLE][CAMNUM][ART_ANGULAR][ART_RADIAL];
	//unsigned char	q8_ArtCoeff[ANGLE][CAMNUM][ART_COEF];
	unsigned char	q8_ArtCoeff[ART_COEF];
	//unsigned char	q4_ArtCoeff[ANGLE][CAMNUM][ART_COEF_2];//ART_COEF_2==18
	unsigned char	q4_ArtCoeff[ART_COEF_2];
	// for color decsriptor
	//unsigned __int64 CompactColor[ANGLE][CAMNUM];	// 63 bits for each image
	//unsigned __int64 dest_CompactColor[ANGLE][CAMNUM];	// 63 bits for each image
	// for circularity
	//double			cir_Coeff[ANGLE][CAMNUM];
	double			cir_Coeff;
	//unsigned char	q8_cirCoeff[ANGLE][CAMNUM];
	unsigned char	q8_cirCoeff;
	// for fourier descriptor
	//double			src_FdCoeff[ANGLE][CAMNUM][FD_COEFF_NO], dest_FdCoeff[ANGLE][CAMNUM][FD_COEFF_NO];
	double			src_FdCoeff[FD_COEFF_NO];
	//unsigned char	q8_FdCoeff[ANGLE][CAMNUM][FD_COEFF_NO];
	unsigned char	q8_FdCoeff[FD_COEFF_NO]; //src_FdCoeff��q8_FdCoeff֮��Ĺ�ϵ�ǣ�ǰ��Ϊ����õ���ֵ������Ϊǰ�߽��й�һ���ֵ
	sPOINT			*Contour;
	unsigned char	*ContourMask;
	// for eccentricity
	//double			ecc_Coeff[ANGLE][CAMNUM];
	double			ecc_Coeff;
	//unsigned char	q8_eccCoeff[ANGLE][CAMNUM], dest_eccCoeff[ANGLE][CAMNUM];
	unsigned char	q8_eccCoeff;
	// for compare
	int				Count;
	// quantization version
	char			fname[400];
	int				high, low, middle;
	double			QuantTable[17] = {	0.000000000, 0.003585473, 0.007418411, 0.011535520, 
		0.015982337, 0.020816302, 0.026111312, 0.031964674, 
		0.038508176, 0.045926586, 0.054490513, 0.064619488, 
		0.077016351, 0.092998687, 0.115524524, 0.154032694, 1.000000000};
	double			CenX, CenY;
	int				total;


	// initialize ART
	GenerateBasisLUT();

	//Ϊ��Ƭ�����ڴ�ռ�
	srcBuffer = (unsigned char *) malloc (winw * winh * sizeof(unsigned char));

	YuvBuff = (unsigned char *) malloc (3 * winw * winh * sizeof(unsigned char));

	// add edge to test retrieval
	//ò�Ʋ�Ҫ�õ�
	EdgeBuff = (unsigned char *) malloc (winw * winh * sizeof(unsigned char));


	// for Fourier Descriptor
	total = winw * winh;
	Contour = (sPOINT *) malloc( total * sizeof(sPOINT));
	ContourMask = (unsigned char *) malloc( total * sizeof(unsigned char));

	//�������buffer�����Ƿ�ɹ�
	if(!srcBuffer||!YuvBuff||!EdgeBuff ||!Contour ||!ContourMask )
	{
		sprintf(err,"buffer allocate failed! \n"); 
		return 0;
	}

	//������Ƭ�ĵ�ַ�б�
	//fpt1 = fopen("SearchConfigure\\CompareSource.txt", "r");//���ﲻ��ʹ��rb����֪����ʲôԭ��

	//���ָ����屣����CompareSource.txt����Ϊ������ʱ����Ҫ����ļ���
	fpt = fopen("SearchConfigure\\CompareSource.txt", "w");
	if(fpt==NULL) 
	{
		printf("can't open the file: SearchConfigure\\CompareSource.txt\n");
		return 0;
	}
	fprintf(fpt, "%s%s\n","SearchSource\\\\",(char*)object.filename.c_str());
	fclose(fpt);

	
	//ÿ�δ�list���һ���ļ���
	//while( fgets(fname, 400, fpt1) )
	//{
		// record execute time --- start
		//start = clock();

		//�����1����Ϊÿһ�к�������������
		//fname[strlen(fname)-1] = '\0';
		//fname[strlen(fname)-1] = '\0';
		//sprintf(fname, "%s", fname);

		// get the translatation and scale of the two model
		//if(!ReadPicture(fname,srcBuff[0],winw,winh,CenX,CenY))
		if(!ReadPictureByOpencv(srcBuffer,winw,winh,object))
		{
			sprintf(err,"Picture read failed! \n"); 
			return 0;
		}	
		// find center for each shape
		FindCenter(srcBuffer, winw, winh, &CenX, &CenY);

		// get Zernike moment
		FindRadius2(srcBuffer, &CenX, &CenY);

		//ExtractCoefficients(srcBuffer, src_ArtCoeff[0][0], EdgeBuff, CenX, CenY);
		ExtractCoefficients(srcBuffer, src_ArtCoeff, EdgeBuff, CenX, CenY);

		FourierDescriptor(src_FdCoeff, srcBuffer, winw, winh, Contour, ContourMask, CenX, CenY);

		// get eccentricity
		//ecc_Coeff[0][0] = Eccentricity(srcBuffer, winw, winh, CenX, CenY);
		ecc_Coeff = Eccentricity(srcBuffer, winw, winh, CenX, CenY);

		// get circularity
		EdgeDetectSil(EdgeBuff, srcBuffer, winw, winh);
		cir_Coeff = Circularity(srcBuffer, winw, winh, EdgeBuff);




		//for(i=0; i<1; i++)
		//for(j=0; j<1; j++)
		//{
		// the order is the same with that defined in MPEG-7, total 35 coefficients
		k = 0;
		p = 0;

		//��k��0�䵽��2���൱�ڽ�q8_ArtCoeff[i][j][k] �е�ǰ�����������˸�ֵ
		//��֪���������
		for(r=1 ; r<ART_RADIAL ; r++, k++)
		{
			//itmp = (int)(QUANT8 *  src_ArtCoeff[i][j][p][r]);
			itmp = (int)(QUANT8 *  src_ArtCoeff[p][r]);
			if(itmp>255)
				//q8_ArtCoeff[i][j][k] = 255;
				q8_ArtCoeff[k] = 255;
			else
				//q8_ArtCoeff[i][j][k] = itmp;
				q8_ArtCoeff[k] = itmp;
		}

		//����k��2��ʼ�ˣ�p�Ǵ�1��ʼ��
		for(p=1; p<ART_ANGULAR ; p++)
			for(r=0 ; r<ART_RADIAL ; r++, k++)
			{
				//itmp = (int)(QUANT8 *  src_ArtCoeff[i][j][p][r]);
				itmp = (int)(QUANT8 *  src_ArtCoeff[p][r]);
				if(itmp>255)
					//q8_ArtCoeff[i][j][k] = 255;
					q8_ArtCoeff[k] = 255;
				else
					q8_ArtCoeff[k] = itmp;
			}
			//}
			// save to disk
			//fwrite(q8_ArtCoeff, sizeof(unsigned char), ANGLE * CAMNUM * ART_COEF, fpt_art_q8);
		//	sprintf(filename, "%s_q8_v1.8.art", fname);
			
			sprintf(filename, "SearchSource/%s_q8_v1.8.art", (char*)object.filename.c_str());
			if( (fpt = fopen(filename, "wb")) == NULL )	{	sprintf(err,"Write %s error!", filename);	return 0;	}
			//fwrite(q8_ArtCoeff, sizeof(unsigned char), ANGLE * CAMNUM * ART_COEF, fpt);
			fwrite(q8_ArtCoeff, sizeof(unsigned char), ART_COEF, fpt);
			fclose(fpt);

			// non-linear Quantization to 4 bits for each coefficient using MPEG-7 quantization table
			//for(i=0; i<ANGLE; i++)
			//	for(j=0; j<CAMNUM; j++)
			//for(i=0; i<1; i++)
			//for(j=0; j<1; j++)
			//{
			// the order is the same with that defined in MPEG-7, total 35 coefficients
			k = 0;
			p = 0;
			for(r=1 ; r<ART_RADIAL ; r++, k++)
			{
				high = 17;
				low = 0;
				while(high-low > 1)
				{
					middle = (high+low) / 2;

					//if(QuantTable[middle] < src_ArtCoeff[i][j][p][r])
					if(QuantTable[middle] < src_ArtCoeff[p][r])
						low = middle;
					else
						high = middle;
				}
				q8_ArtCoeff[k] = low;
			}
			for(p=1; p<ART_ANGULAR ; p++)
				for(r=0 ; r<ART_RADIAL ; r++, k++)
				{
					high = 17;
					low = 0;
					while(high-low > 1)
					{
						middle = (high+low) / 2;

						//if(QuantTable[middle] < src_ArtCoeff[i][j][p][r])
						if(QuantTable[middle] < src_ArtCoeff[p][r])
							low = middle;
						else
							high = middle;
					}
					//q8_ArtCoeff[i][j][k] = low;
					q8_ArtCoeff[k] = low;
				}
				//} 

				//for(i=0; i<ANGLE; i++)
				//	for(j=0; j<CAMNUM; j++)
				//for(i=0; i<1; i++)
				//for(j=0; j<1; j++)
				for(k=0, a=0; k<ART_COEF; k+=2, a++) //k-36 -a-18
					if( k+1 < ART_COEF )
						//q4_ArtCoeff[i][j][a] = ( (q8_ArtCoeff[i][j][k] << 4) & 0xf0 ) | 
						//( q8_ArtCoeff[i][j][k+1] & 0x0f );
						q4_ArtCoeff[a] = ( (q8_ArtCoeff[k] << 4) & 0xf0 ) | 
						( q8_ArtCoeff[k+1] & 0x0f );
					else
						//q4_ArtCoeff[i][j][a] = ( (q8_ArtCoeff[i][j][k] << 4) & 0xf0 );
						q4_ArtCoeff[a] = ( (q8_ArtCoeff[k] << 4) & 0xf0 );

				// save to disk
				//fwrite(q4_ArtCoeff, sizeof(unsigned char), ANGLE * CAMNUM * ART_COEF_2, fpt_art_q4);
				//StrCpy(fname,(char*)object.filename.c_str());
				sprintf(filename, "SearchSource/%s_q4_v1.8.art", (char*)object.filename.c_str());
				if( (fpt = fopen(filename, "wb")) == NULL )	{	sprintf(err,"Write %s error!", filename);	return 0;	}
				fwrite(q4_ArtCoeff, sizeof(unsigned char), ART_COEF_2, fpt);
				fclose(fpt);

				// **********************************************************************
				// save color descriptor to disk
				//			fwrite(CompactColor, sizeof(unsigned char), ANGLE * CAMNUM * sizeof(unsigned __int64), fpt_ccd);
				//			sprintf(filename, "%s.ccd", fname);
				//			if( (fpt = fopen(filename, "wb")) == NULL )	{	printf("Write %s error!!\n", filename);	return;	}
				//			fwrite(CompactColor, sizeof(unsigned char), ANGLE * CAMNUM * sizeof(unsigned __int64), fpt);
				//			fclose(fpt);

				// **********************************************************************
				// save circularity feature to file
				//			sprintf(filename, "%s.cir", fname);
				//			fpt = fopen(filename, "wb");
				//			fwrite(cir_Coeff, ANGLE * CAMNUM, sizeof(double), fpt);
				//			fclose(fpt);

				// linear Quantization to 8 bits for each coefficient
				//for(i=0; i<ANGLE; i++)
				//	for(j=0; j<CAMNUM; j++)
				//for(i=0; i<1; i++)
				//for(j=0; j<1; j++)
				//{
				itmp = (int)(QUANT8 *  cir_Coeff);
				//if(itmp>255)		q8_cirCoeff[i][j] = 255;
				//else				q8_cirCoeff[i][j] = itmp;
				if(itmp>255)		q8_cirCoeff = 255;
				else				q8_cirCoeff = itmp;
				//}
				// save to disk
				//fwrite(q8_cirCoeff, sizeof(unsigned char), ANGLE * CAMNUM, fpt_cir_q8);
				sprintf(filename, "SearchSource/%s_q8_v1.8.cir", (char*)object.filename.c_str());
				if( (fpt = fopen(filename, "wb")) == NULL )	{	sprintf(err,"Write %s error!", filename);	return 0;	}
				fwrite(&q8_cirCoeff, sizeof(unsigned char), 1, fpt);
				fclose(fpt);

				// **********************************************************************
				// save eccentricity feature to file
				// linear Quantization to 8 bits for each coefficient
				//for(i=0; i<ANGLE; i++)
				//	for(j=0; j<CAMNUM; j++)
				//for(i=0; i<1; i++)
				//for(j=0; j<1; j++)
				//{
				//itmp = (int)(QUANT8 * ecc_Coeff[i][j]);
				itmp = (int)(QUANT8 * ecc_Coeff);
				//if(itmp>255)		q8_eccCoeff[i][j] = 255;
				//else				q8_eccCoeff[i][j] = itmp;
				if(itmp>255)		q8_eccCoeff = 255;
				else				q8_eccCoeff = itmp;
				//}
				// save to disk
				//fwrite(&q8_eccCoeff, sizeof(unsigned char), 1, fpt_ecc_q8);
				//sprintf(filename, "%s_q8_v1.8.ecc", fname);
				sprintf(filename, "SearchSource/%s_q8_v1.8.ecc", (char*)object.filename.c_str());
				if( (fpt = fopen(filename, "wb")) == NULL )	{	sprintf(err,"Write %s error!", filename);	return 0;	}
				fwrite(&q8_eccCoeff, sizeof(unsigned char), 1, fpt);
				fclose(fpt);

				// **********************************************************************
				// save Fourier descriptor feature to file
				//			fwrite(src_FdCoeff, ANGLE * CAMNUM * FD_COEFF_NO, sizeof(double), fpt_fd);
				//			sprintf(filename, "%s.fd", fname);
				//			fpt = fopen(filename, "wb");
				//			fwrite(src_FdCoeff, ANGLE * CAMNUM * FD_COEFF_NO, sizeof(double), fpt);
				//			fclose(fpt);

				//for(i=0; i<ANGLE; i++)
				//	for(j=0; j<CAMNUM; j++)
				//for(i=0; i<1; i++)
				//for(j=0; j<1; j++)
				//{
				for(k=0; k<FD_COEFF_NO; k++)
				{
					//itmp = (int)(QUANT8 * FD_SCALE * src_FdCoeff[i][j][k]);
					itmp = (int)(QUANT8 * FD_SCALE * src_FdCoeff[k]);
					if(itmp>255)
						//q8_FdCoeff[i][j][k] = 255;
						q8_FdCoeff[k] = 255;
					else
						//q8_FdCoeff[i][j][k] = itmp;
						q8_FdCoeff[k] = itmp;
				}
				//}
				//һ���ǽ�������������д��һ��
				//fwrite(q8_FdCoeff,  FD_COEFF_NO, sizeof(unsigned char), fpt_fd_q8);
				//һ���ǽ�������������д�������ļ�
				sprintf(filename, "SearchSource/%s_q8_v1.8.fd", (char*)object.filename.c_str());
				if((fpt = fopen(filename, "wb"))==NULL){sprintf(err,"Write %s error!", filename);	return 0;	};
				fwrite(q8_FdCoeff, FD_COEFF_NO, sizeof(unsigned char), fpt);
				fclose(fpt);

				//printf("%d:The process of %s is finished! ", Count++,fname);
	//}

	free(srcBuffer);
	free(YuvBuff);
	free(EdgeBuff);
	free(Contour);
	free(ContourMask);
	//fclose(fpt1);

	return 1;
}


//�˺���Ϊ��ά��Ƭ�ָ�������ά������ȡ������
//1.�˺�����ȡ������Ϊ��Ƭ�ָ�����������
//2.�ú�������ȡ�Ľ��ֱ�ӱ��浽���飬�Ա�ֱ�����ڼ�����˳��Ҳ����һ�ݵ��ļ����ˣ�
//3. err��ʾ��ȡ�����ӹ����еĳ�����Ϣ�� object��ʾ����ȡ�����ӵķָ����壬numb��ʾ��numb���ָ����壨i��0��ʼ��
int FeatureExtractForPictureScene(char* err, CSegObject &object,int numb,char* pFilename)
{
	unsigned char   *EdgeBuff, *YuvBuff;
	unsigned char	*srcBuffer;
	char			filename[400];
	FILE			*fpt, *fpt1, *fpt_art_q8, *fpt_art_q4, *fpt_fd_q8,  *fpt_cir_q8, *fpt_ecc_q8;//, *fpt_ccd;
	int				i, j, k, p, r, a, itmp;
	static int		UseCam = 2;

	// for region shape descriptor
	double			src_ArtCoeff[ART_ANGULAR][ART_RADIAL];
	//unsigned char	q8_ArtCoeff[ART_COEF]; //�������滻��src_ArtCoeff2
	//unsigned char	q4_ArtCoeff[ART_COEF_2];

	// for color decsriptor
	//unsigned __int64 CompactColor[ANGLE][CAMNUM];	// 63 bits for each image
	//unsigned __int64 dest_CompactColor[ANGLE][CAMNUM];	// 63 bits for each image
	
	// for circularity
	double			cir_Coeff;
	//unsigned char	q8_cirCoeff;

	// for fourier descriptor
	double			src_FdCoeff[FD_COEFF_NO]; //��������Ǹ����õ�
	 //src_FdCoeff��q8_FdCoeff֮��Ĺ�ϵ�ǣ�ǰ��Ϊ����õ���ֵ������Ϊǰ�߽��й�һ���ֵ
	//����������滻��src_FdCoeff_q82
	//unsigned char	q8_FdCoeff[FD_COEFF_NO];
	
	sPOINT			*Contour;
	unsigned char	*ContourMask;

	// for eccentricity
	double			ecc_Coeff; //��������Ǹ����õ�
	//unsigned char	q8_eccCoeff; //����������滻��src_EccCoeff_q82
	// for compare
	int				Count;
	// quantization version
	char			fname[400];
	int				high, low, middle;
	double			QuantTable[17] = {	0.000000000, 0.003585473, 0.007418411, 0.011535520, 
		0.015982337, 0.020816302, 0.026111312, 0.031964674, 
		0.038508176, 0.045926586, 0.054490513, 0.064619488, 
		0.077016351, 0.092998687, 0.115524524, 0.154032694, 1.000000000};
	double			CenX, CenY;
	int				total;


	// initialize ART
	GenerateBasisLUT();

	//Ϊ��Ƭ�����ڴ�ռ�
	srcBuffer = (unsigned char *) malloc (winw * winh * sizeof(unsigned char));

	YuvBuff = (unsigned char *) malloc (3 * winw * winh * sizeof(unsigned char));

	// add edge to test retrieval
	//ò�Ʋ�Ҫ�õ�
	EdgeBuff = (unsigned char *) malloc (winw * winh * sizeof(unsigned char));


	// for Fourier Descriptor
	total = winw * winh;
	Contour = (sPOINT *) malloc( total * sizeof(sPOINT));
	ContourMask = (unsigned char *) malloc( total * sizeof(unsigned char));

	//�������buffer�����Ƿ�ɹ�
	if(!srcBuffer||!YuvBuff||!EdgeBuff ||!Contour ||!ContourMask )
	{
		sprintf(err,"buffer allocate failed! \n"); 
		return 0;
	}

	/*
	//���ָ����屣����CompareSource.txt����Ϊ������ʱ����Ҫ����ļ���
	fpt = fopen("SearchConfigure\\CompareSource.txt", "w");
	if(fpt==NULL) 
	{
		printf("can't open the file: SearchConfigure\\CompareSource.txt\n");
		return 0;
	}
	fprintf(fpt, "%s%s\n","SearchSource\\\\",(char*)object.filename.c_str());
	fclose(fpt);
	*/

	//ÿ�δ�list���һ���ļ���
	//while( fgets(fname, 400, fpt1) )
	//{
	// record execute time --- start
	//start = clock();

	//�����1����Ϊÿһ�к�������������
	//fname[strlen(fname)-1] = '\0';
	//fname[strlen(fname)-1] = '\0';
	//sprintf(fname, "%s", fname);

	// get the translatation and scale of the two model
	//if(!ReadPicture(fname,srcBuff[0],winw,winh,CenX,CenY))
	if(!ReadPictureByOpencv(srcBuffer,winw,winh,object))
	{
		sprintf(err,"Picture read failed! \n"); 
		return 0;
	}	
	// find center for each shape
	FindCenter(srcBuffer, winw, winh, &CenX, &CenY);

	// get Zernike moment
	FindRadius2(srcBuffer, &CenX, &CenY);

	//ExtractCoefficients(srcBuffer, src_ArtCoeff[0][0], EdgeBuff, CenX, CenY);
	ExtractCoefficients(srcBuffer, src_ArtCoeff, EdgeBuff, CenX, CenY);

	FourierDescriptor(src_FdCoeff, srcBuffer, winw, winh, Contour, ContourMask, CenX, CenY);

	// get eccentricity
	//ecc_Coeff[0][0] = Eccentricity(srcBuffer, winw, winh, CenX, CenY);
	ecc_Coeff = Eccentricity(srcBuffer, winw, winh, CenX, CenY);

	// get circularity
	EdgeDetectSil(EdgeBuff, srcBuffer, winw, winh);
	cir_Coeff = Circularity(srcBuffer, winw, winh, EdgeBuff);




	//for(i=0; i<1; i++)
	//for(j=0; j<1; j++)
	//{
	// the order is the same with that defined in MPEG-7, total 35 coefficients
	k = 0;
	p = 0;

	//��k��0�䵽��2���൱�ڽ�q8_ArtCoeff[i][j][k] �е�ǰ�����������˸�ֵ
	//��֪���������
	for(r=1 ; r<ART_RADIAL ; r++, k++)
	{
		//itmp = (int)(QUANT8 *  src_ArtCoeff[i][j][p][r]);
		itmp = (int)(QUANT8 *  src_ArtCoeff[p][r]);
		if(itmp>255)
			//q8_ArtCoeff[i][j][k] = 255;
			//q8_ArtCoeff[k] = 255;
			src_ArtCoeff2[numb][k] = 255;
		else
			//q8_ArtCoeff[i][j][k] = itmp;
			//q8_ArtCoeff[k] = itmp;
			src_ArtCoeff2[numb][k] = itmp;
	}

	//����k��2��ʼ�ˣ�p�Ǵ�1��ʼ��
	for(p=1; p<ART_ANGULAR ; p++)
		for(r=0 ; r<ART_RADIAL ; r++, k++)
		{
			//itmp = (int)(QUANT8 *  src_ArtCoeff[i][j][p][r]);
			itmp = (int)(QUANT8 *  src_ArtCoeff[p][r]);
			if(itmp>255)
				//q8_ArtCoeff[i][j][k] = 255;
				//q8_ArtCoeff[k] = 255;
				src_ArtCoeff2[numb][k] = 255;
			else
				//q8_ArtCoeff[k] = itmp;
				src_ArtCoeff2[numb][k] = itmp;
		}
		//}
		// save to disk
		//fwrite(q8_ArtCoeff, sizeof(unsigned char), ANGLE * CAMNUM * ART_COEF, fpt_art_q8);
		//	sprintf(filename, "%s_q8_v1.8.art", fname);

		sprintf(filename, "SearchSource/%s_%d_q8_v1.8.art",pFilename,numb);
		if( (fpt = fopen(filename, "wb")) == NULL )	{	sprintf(err,"Write %s error!", filename);	return 0;	}
		//fwrite(q8_ArtCoeff, sizeof(unsigned char), ANGLE * CAMNUM * ART_COEF, fpt);
		fwrite(src_ArtCoeff2[numb], sizeof(unsigned char), ART_COEF, fpt);
		fclose(fpt);

		//q4û�õ���������ʱ�Ͳ�����
		/*
		// non-linear Quantization to 4 bits for each coefficient using MPEG-7 quantization table
		//for(i=0; i<ANGLE; i++)
		//	for(j=0; j<CAMNUM; j++)
		//for(i=0; i<1; i++)
		//for(j=0; j<1; j++)
		//{
		// the order is the same with that defined in MPEG-7, total 35 coefficients
		k = 0;
		p = 0;
		for(r=1 ; r<ART_RADIAL ; r++, k++)
		{
			high = 17;
			low = 0;
			while(high-low > 1)
			{
				middle = (high+low) / 2;

				//if(QuantTable[middle] < src_ArtCoeff[i][j][p][r])
				if(QuantTable[middle] < src_ArtCoeff[p][r])
					low = middle;
				else
					high = middle;
			}
			q8_ArtCoeff[k] = low;
		}
		for(p=1; p<ART_ANGULAR ; p++)
			for(r=0 ; r<ART_RADIAL ; r++, k++)
			{
				high = 17;
				low = 0;
				while(high-low > 1)
				{
					middle = (high+low) / 2;

					//if(QuantTable[middle] < src_ArtCoeff[i][j][p][r])
					if(QuantTable[middle] < src_ArtCoeff[p][r])
						low = middle;
					else
						high = middle;
				}
				//q8_ArtCoeff[i][j][k] = low;
				q8_ArtCoeff[k] = low;
			}
			//} 

			//for(i=0; i<ANGLE; i++)
			//	for(j=0; j<CAMNUM; j++)
			//for(i=0; i<1; i++)
			//for(j=0; j<1; j++)
			for(k=0, a=0; k<ART_COEF; k+=2, a++) //k-36 -a-18
				if( k+1 < ART_COEF )
					//q4_ArtCoeff[i][j][a] = ( (q8_ArtCoeff[i][j][k] << 4) & 0xf0 ) | 
					//( q8_ArtCoeff[i][j][k+1] & 0x0f );
					q4_ArtCoeff[a] = ( (q8_ArtCoeff[k] << 4) & 0xf0 ) | 
					( q8_ArtCoeff[k+1] & 0x0f );
				else
					//q4_ArtCoeff[i][j][a] = ( (q8_ArtCoeff[i][j][k] << 4) & 0xf0 );
					q4_ArtCoeff[a] = ( (q8_ArtCoeff[k] << 4) & 0xf0 );

			// save to disk
			//fwrite(q4_ArtCoeff, sizeof(unsigned char), ANGLE * CAMNUM * ART_COEF_2, fpt_art_q4);
			//StrCpy(fname,(char*)object.filename.c_str());
			sprintf(filename, "SearchSource/%s_q4_v1.8.art", (char*)object.filename.c_str());
			if( (fpt = fopen(filename, "wb")) == NULL )	{	sprintf(err,"Write %s error!", filename);	return 0;	}
			fwrite(q4_ArtCoeff, sizeof(unsigned char), ART_COEF_2, fpt);
			fclose(fpt);
			*/

			// **********************************************************************
			// save color descriptor to disk
			//			fwrite(CompactColor, sizeof(unsigned char), ANGLE * CAMNUM * sizeof(unsigned __int64), fpt_ccd);
			//			sprintf(filename, "%s.ccd", fname);
			//			if( (fpt = fopen(filename, "wb")) == NULL )	{	printf("Write %s error!!\n", filename);	return;	}
			//			fwrite(CompactColor, sizeof(unsigned char), ANGLE * CAMNUM * sizeof(unsigned __int64), fpt);
			//			fclose(fpt);

			// **********************************************************************
			// save circularity feature to file
			//			sprintf(filename, "%s.cir", fname);
			//			fpt = fopen(filename, "wb");
			//			fwrite(cir_Coeff, ANGLE * CAMNUM, sizeof(double), fpt);
			//			fclose(fpt);

			// linear Quantization to 8 bits for each coefficient
			//for(i=0; i<ANGLE; i++)
			//	for(j=0; j<CAMNUM; j++)
			//for(i=0; i<1; i++)
			//for(j=0; j<1; j++)
			//{
			itmp = (int)(QUANT8 *  cir_Coeff);
			//if(itmp>255)		q8_cirCoeff[i][j] = 255;
			//else				q8_cirCoeff[i][j] = itmp;
			//if(itmp>255)		q8_cirCoeff = 255;
			//else				q8_cirCoeff = itmp;
			if(itmp>255)		src_CirCoeff_q82[numb] = 255;
			else				src_CirCoeff_q82[numb] = itmp;
			//}
			// save to disk
			//fwrite(q8_cirCoeff, sizeof(unsigned char), ANGLE * CAMNUM, fpt_cir_q8);
			sprintf(filename, "SearchSource/%s_%d_q8_v1.8.cir", pFilename,numb);
			if( (fpt = fopen(filename, "wb")) == NULL )	{	sprintf(err,"Write %s error!", filename);	return 0;	}
			fwrite(&src_CirCoeff_q82[numb], sizeof(unsigned char), 1, fpt);
			fclose(fpt);

			// **********************************************************************
			// save eccentricity feature to file
			// linear Quantization to 8 bits for each coefficient
			//for(i=0; i<ANGLE; i++)
			//	for(j=0; j<CAMNUM; j++)
			//for(i=0; i<1; i++)
			//for(j=0; j<1; j++)
			//{
			//itmp = (int)(QUANT8 * ecc_Coeff[i][j]);
			itmp = (int)(QUANT8 * ecc_Coeff);
			//if(itmp>255)		q8_eccCoeff[i][j] = 255;
			//else				q8_eccCoeff[i][j] = itmp;
			//if(itmp>255)		q8_eccCoeff = 255;
			//else				q8_eccCoeff = itmp;
			if(itmp>255)		src_EccCoeff_q82[numb] = 255;
			else				src_EccCoeff_q82[numb] = itmp;

			//}
			// save to disk
			//fwrite(&q8_eccCoeff, sizeof(unsigned char), 1, fpt_ecc_q8);
			//sprintf(filename, "%s_q8_v1.8.ecc", fname);
			sprintf(filename, "SearchSource/%s_%d_q8_v1.8.ecc",pFilename,numb);
			if( (fpt = fopen(filename, "wb")) == NULL )	{	sprintf(err,"Write %s error!", filename);	return 0;	}
			fwrite(&src_EccCoeff_q82[numb], sizeof(unsigned char), 1, fpt);
			fclose(fpt);

			// **********************************************************************
			// save Fourier descriptor feature to file
			//			fwrite(src_FdCoeff, ANGLE * CAMNUM * FD_COEFF_NO, sizeof(double), fpt_fd);
			//			sprintf(filename, "%s.fd", fname);
			//			fpt = fopen(filename, "wb");
			//			fwrite(src_FdCoeff, ANGLE * CAMNUM * FD_COEFF_NO, sizeof(double), fpt);
			//			fclose(fpt);

			//for(i=0; i<ANGLE; i++)
			//	for(j=0; j<CAMNUM; j++)
			//for(i=0; i<1; i++)
			//for(j=0; j<1; j++)
			//{
			for(k=0; k<FD_COEFF_NO; k++)
			{
				//itmp = (int)(QUANT8 * FD_SCALE * src_FdCoeff[i][j][k]);
				itmp = (int)(QUANT8 * FD_SCALE * src_FdCoeff[k]);
				if(itmp>255)
					//q8_FdCoeff[i][j][k] = 255;
					//q8_FdCoeff[k] = 255;
				    src_FdCoeff_q82[numb][k] = 255;
				else
					//q8_FdCoeff[i][j][k] = itmp;
					//q8_FdCoeff[k] = itmp;
					src_FdCoeff_q82[numb][k] = itmp;
			}
			//}
			//һ���ǽ�������������д��һ��
			//fwrite(q8_FdCoeff,  FD_COEFF_NO, sizeof(unsigned char), fpt_fd_q8);
			//һ���ǽ�������������д�������ļ�
			sprintf(filename, "SearchSource/%s_%d_q8_v1.8.fd",pFilename,numb);
			if((fpt = fopen(filename, "wb"))==NULL){sprintf(err,"Write %s error!", filename);	return 0;	};
			fwrite(src_FdCoeff_q82[numb], FD_COEFF_NO, sizeof(unsigned char), fpt);
			fclose(fpt);

			//printf("%d:The process of %s is finished! ", Count++,fname);
			//}

			free(srcBuffer);
			free(YuvBuff);
			free(EdgeBuff);
			free(Contour);
			free(ContourMask);
			//fclose(fpt1);

			return 1;
}


//�˺���Ϊ�û��ֻ��ά����������ȡ������
//1.�ú�������ȡ�Ľ��ֱ�ӱ��浽���飬�Ա�ֱ�����ڼ�����˳��Ҳ����һ�ݵ��ļ����ˣ�
//3. err��ʾ��ȡ�����ӹ����еĳ�����Ϣ��object��ʾ����ȡ�����ӵķָ����壬numb��ʾ��numb���ָ����壨i��0��ʼ��
int FeatureExtractForPictureObject(char* err, CvvImage& picture)
{
	unsigned char   *EdgeBuff, *YuvBuff;
	unsigned char	*srcBuffer;
	char			filename[400];
	FILE			*fpt, *fpt1, *fpt_art_q8, *fpt_art_q4, *fpt_fd_q8,  *fpt_cir_q8, *fpt_ecc_q8;//, *fpt_ccd;
	int				i, j, k, p, r, a, itmp;
	static int		UseCam = 2;

	// for region shape descriptor
	double			src_ArtCoeff[ART_ANGULAR][ART_RADIAL];
	//unsigned char	q8_ArtCoeff[ART_COEF]; //�������滻��src_ArtCoeff1
	//unsigned char	q4_ArtCoeff[ART_COEF_2];

	// for color decsriptor
	//unsigned __int64 CompactColor[ANGLE][CAMNUM];	// 63 bits for each image
	//unsigned __int64 dest_CompactColor[ANGLE][CAMNUM];	// 63 bits for each image
	
	// for circularity
	double			cir_Coeff;
	//unsigned char	q8_cirCoeff; //�������滻��src_cirCoeff1

	// for fourier descriptor
	double			src_FdCoeff[FD_COEFF_NO]; //��������Ǹ����õ�
	 //src_FdCoeff��q8_FdCoeff֮��Ĺ�ϵ�ǣ�ǰ��Ϊ����õ���ֵ������Ϊǰ�߽��й�һ���ֵ
	//����������滻��src_FdCoeff_q81
	//unsigned char	q8_FdCoeff[FD_COEFF_NO];
	
	sPOINT			*Contour;
	unsigned char	*ContourMask;

	// for eccentricity
	double			ecc_Coeff; //��������Ǹ����õ�
	//unsigned char	q8_eccCoeff; //����������滻��src_EccCoeff_q81

	// for compare
	int				Count;
	// quantization version
	char			fname[400];
	int				high, low, middle;
	double			QuantTable[17] = {	0.000000000, 0.003585473, 0.007418411, 0.011535520, 
		0.015982337, 0.020816302, 0.026111312, 0.031964674, 
		0.038508176, 0.045926586, 0.054490513, 0.064619488, 
		0.077016351, 0.092998687, 0.115524524, 0.154032694, 1.000000000};
	double			CenX, CenY;
	int				total;


	// initialize ART
	GenerateBasisLUT();

	//Ϊ��Ƭ�����ڴ�ռ�
	srcBuffer = (unsigned char *) malloc (winw * winh * sizeof(unsigned char));

	YuvBuff = (unsigned char *) malloc (3 * winw * winh * sizeof(unsigned char));

	// add edge to test retrieval
	//ò�Ʋ�Ҫ�õ�
	EdgeBuff = (unsigned char *) malloc (winw * winh * sizeof(unsigned char));


	// for Fourier Descriptor
	total = winw * winh;
	Contour = (sPOINT *) malloc( total * sizeof(sPOINT));
	ContourMask = (unsigned char *) malloc( total * sizeof(unsigned char));

	//�������buffer�����Ƿ�ɹ�
	if(!srcBuffer||!YuvBuff||!EdgeBuff ||!Contour ||!ContourMask )
	{
		sprintf(err,"buffer allocate failed! \n"); 
		return 0;
	}

	//��ȡ��Ƭ��Ϣ
	if(!LoadPicture(srcBuffer,winw,winh,picture))
	{
		sprintf(err,"Picture read failed! \n"); 
		return 0;
	}	
	// find center for each shape
	FindCenter(srcBuffer, winw, winh, &CenX, &CenY);

	// get Zernike moment
	FindRadius2(srcBuffer, &CenX, &CenY);

	//ExtractCoefficients(srcBuffer, src_ArtCoeff[0][0], EdgeBuff, CenX, CenY);
	ExtractCoefficients(srcBuffer, src_ArtCoeff, EdgeBuff, CenX, CenY);

	FourierDescriptor(src_FdCoeff, srcBuffer, winw, winh, Contour, ContourMask, CenX, CenY);

	// get eccentricity
	//ecc_Coeff[0][0] = Eccentricity(srcBuffer, winw, winh, CenX, CenY);
	ecc_Coeff = Eccentricity(srcBuffer, winw, winh, CenX, CenY);

	// get circularity
	EdgeDetectSil(EdgeBuff, srcBuffer, winw, winh);
	cir_Coeff = Circularity(srcBuffer, winw, winh, EdgeBuff);

	//for(i=0; i<1; i++)
	//for(j=0; j<1; j++)
	//{
	// the order is the same with that defined in MPEG-7, total 35 coefficients
	k = 0;
	p = 0;

	//��k��0�䵽��2���൱�ڽ�q8_ArtCoeff[i][j][k] �е�ǰ�����������˸�ֵ
	//��֪���������
	for(r=1 ; r<ART_RADIAL ; r++, k++)
	{
		//itmp = (int)(QUANT8 *  src_ArtCoeff[i][j][p][r]);
		itmp = (int)(QUANT8 *  src_ArtCoeff[p][r]);
		if(itmp>255)
			//q8_ArtCoeff[i][j][k] = 255;
			//q8_ArtCoeff[k] = 255;
			src_ArtCoeff1[k] = 255;
		else
			//q8_ArtCoeff[i][j][k] = itmp;
			//q8_ArtCoeff[k] = itmp;
			src_ArtCoeff1[k] = itmp;
	}

	//����k��2��ʼ�ˣ�p�Ǵ�1��ʼ��
	for(p=1; p<ART_ANGULAR ; p++)
		for(r=0 ; r<ART_RADIAL ; r++, k++)
		{
			//itmp = (int)(QUANT8 *  src_ArtCoeff[i][j][p][r]);
			itmp = (int)(QUANT8 *  src_ArtCoeff[p][r]);
			if(itmp>255)
				//q8_ArtCoeff[i][j][k] = 255;
				//q8_ArtCoeff[k] = 255;
				src_ArtCoeff1[k] = 255;
			else
				//q8_ArtCoeff[k] = itmp;
				src_ArtCoeff1[k] = itmp;
		}


			itmp = (int)(QUANT8 *  cir_Coeff);
			//if(itmp>255)		q8_cirCoeff[i][j] = 255;
			//else				q8_cirCoeff[i][j] = itmp;
			//if(itmp>255)		q8_cirCoeff = 255;
			//else				q8_cirCoeff = itmp;
			if(itmp>255)		src_CirCoeff_q81 = 255;
			else				src_CirCoeff_q81 = itmp;
			//}

			itmp = (int)(QUANT8 * ecc_Coeff);
			//if(itmp>255)		q8_eccCoeff[i][j] = 255;
			//else				q8_eccCoeff[i][j] = itmp;
			//if(itmp>255)		q8_eccCoeff = 255;
			//else				q8_eccCoeff = itmp;
			if(itmp>255)		src_EccCoeff_q81 = 255;
			else				src_EccCoeff_q81 = itmp;
			//}


			for(k=0; k<FD_COEFF_NO; k++)
			{
				//itmp = (int)(QUANT8 * FD_SCALE * src_FdCoeff[i][j][k]);
				itmp = (int)(QUANT8 * FD_SCALE * src_FdCoeff[k]);
				if(itmp>255)
					//q8_FdCoeff[i][j][k] = 255;
					//q8_FdCoeff[k] = 255;
				    src_FdCoeff_q81[k] = 255;
				else
					//q8_FdCoeff[i][j][k] = itmp;
					//q8_FdCoeff[k] = itmp;
					src_FdCoeff_q81[k] = itmp;
			}

			free(srcBuffer);
			free(YuvBuff);
			free(EdgeBuff);
			free(Contour);
			free(ContourMask);
			//fclose(fpt1);

			return 1;
}