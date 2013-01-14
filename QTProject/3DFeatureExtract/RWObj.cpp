/************************************************************************/
/* it's used by 3D feature extract                                                                     */
/************************************************************************/

//#include "stdafx.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include "Windows.h"
#include "ds.h"
#include <math.h>
//#include "trimesh_h/timestamp.h"
//#include "trimesh2_h/TriMesh.h"
//#include "TriMesh.h"
#include "RWObj.h"
//#include "../trimesh2/TriMesh.h"
#include "trimesh/TriMesh.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/imgproc_c.h"


#define		LINE_MAX_LEN	256

int ReadMeterial(char *filename, pMeterial *color)
{
	FILE		*fpt;
	char		input[LINE_MAX_LEN];
	char		token[LINE_MAX_LEN];
	char		*next;
	int			width;
	pMeterial	pNewMtl;

	// if cannot find the mtl file
	//  Advance to the first nonspace character in "input"
	if( (fpt = fopen(filename, "r")) == NULL )
		return 0;

	while( fgets(input, LINE_MAX_LEN, fpt) != NULL ) 
	{
		//  Advance to the first nonspace character in "input"
		for ( next = input; *next != '\0' && isspace(*next); next++ )
			;

		// Skip blank lines and comments
		if ( *next == '\0' || *next == '#' || *next == '$')
			continue;

		// Extract the first word in this line. 
		sscanf ( next, "%s%n", token, &width );
		next = next + width;

		if( strcmp(token, "newmtl") == 0 )
		{
			pNewMtl = (pMeterial) malloc(sizeof(Meterial));
			sscanf(next, "%s", pNewMtl->name);
			pNewMtl->pointer = (*color);
			(*color) = pNewMtl;
		}
		else if( strcmp(token, "Kd") == 0 )
		{
			sscanf(next, "%lf %lf %lf", &(pNewMtl->r), &(pNewMtl->g), &(pNewMtl->b));
		}
	}

	fclose(fpt);
	return 1;
}

int ReadObj(char *filename, pVer *vertex, pTri *triangle, int *NumVer, int *NumTri)
{
	FILE		*fpt;
	char		input[LINE_MAX_LEN];
	char		token[LINE_MAX_LEN];
	char		*next;
	int			width;
	int			numver, numtri;
	int			VerIndex, TriIndex;
	double		r0, r1, r2;
	char		value[LINE_MAX_LEN];
	// the face is triangle, but sometimes, the face has 4 vertex, split it to two triangles
	int			f[50];	// the maximum vertices of a face is defined as 50 ( 3 is normal )
	int			fn;		// vertex number of a face
	int			i;
	// for color
	pMeterial	color = NULL;
	pMeterial	pNowMtl = NULL;
	char			fname[400];
	
	//给名字加上后缀
	sprintf(fname, "%s.obj", filename);

	if( (fpt = fopen(fname, "r")) == NULL )
		return 0;	// False

	// one pass: get the number of vertex and triangle
	numver = 0;
	numtri = 0;
	color = NULL;
	while( fgets(input, LINE_MAX_LEN, fpt) != NULL ) 
	{

		//  Advance to the first nonspace character in "input"
		for ( next = input; *next != '\0' && isspace(*next); next++ )
			;

		// Skip blank lines and comments
		if ( *next == '\0' || *next == '#' || *next == '$'||*next == 'g')
			continue;

		// Extract the first word in this line. 
		sscanf ( next, "%s%n", token, &width );
		next = next + width;

		if( strcmp(token, "v") == 0 )
			numver ++;
		else if( strcmp(token, "f") == 0 )
			numtri ++;
		// read the color
		else if( strcmp(token, "mtllib") == 0 )
		{
			sscanf ( next, "%s", token );
			if( !ReadMeterial(token, &color) )
				;//return 0;
		}
	}
	*NumVer = numver;
	*NumTri = numtri;

	// allocate memory of vertex and triangle
	*vertex = (pVer) malloc( numver * sizeof(Ver));
	memset(*vertex, 0, numver * sizeof(Ver));
	*triangle = (pTri) malloc( numtri * sizeof(Tri));
	memset(*triangle, 0, numtri * sizeof(Tri));

	// two pass: get data of vertex and triangle
	fseek(fpt, 0, SEEK_SET);

	VerIndex = 0;
	TriIndex = 0;
	pNowMtl = NULL;
	while( fgets(input, LINE_MAX_LEN, fpt) != NULL )
	{

		//  Advance to the first nonspace character in INPUT
		for ( next = input; *next != '\0' && isspace(*next); next++ )
			;

		// Skip blank lines and comments
		if ( *next == '\0' || *next == '#' || *next == '$'||*next =='g')
			continue;

		// Extract the first word in this line. 
		sscanf ( next, "%s%n", token, &width );
		
		// Set NEXT to point to just after this token. 
		next = next + width;

		/*	V X Y Z W
			Geometric vertex.
			W is optional, a weight for rational curves and surfaces.
			The default for W is 1.		*/
		if( strcmp(token, "v") == 0 )
		{
			sscanf ( next, "%lf %lf %lf", &r0, &r1, &r2 );

			(*vertex)[VerIndex].coor[0] = r0;
			(*vertex)[VerIndex].coor[1] = r1;
			(*vertex)[VerIndex].coor[2] = r2;

			VerIndex ++;
		}
		/*  F V1 V2 V3
			or
			F V1/VT1/VN1 V2/VT2/VN2 ...
			or
			F V1//VN1 V2//VN2 ...
			or
			F V1/VT1/ V2/VT2/ ...

			Face.
			A face is defined by the vertices.
			Optionally, slashes may be used to include the texture vertex
			and vertex normal indices.

			OBJ line node indices are 1 based rather than 0 based.
			So we have to decrement them before loading them into FACE.			*/
		// sometimes in a obj file, there maybe use two kind mode of "f" in different group
		// so, it have to check format when starting read "f"
		else if( strcmp(token, "f") == 0 )
		{
			fn = 0;
			while( sscanf( next, "%s%n", value, &width) != EOF )
			{
				sscanf( value, "%d", f+fn);
				next = next + width;
				fn ++;
			}

			for(i=0; i<fn; i++)
				(*triangle)[TriIndex].v[i] = f[i] - 1;

			(*triangle)[TriIndex].NodeName = fn;	// record the number of vertex in this triangle

			// color
			//因为前面已经将color读过一遍了
			if( pNowMtl )
			{
				(*triangle)[TriIndex].r = pNowMtl->r;
				(*triangle)[TriIndex].g = pNowMtl->g;
				(*triangle)[TriIndex].b = pNowMtl->b;
			}

			TriIndex ++;
		}
		else if( strcmp(token, "usemtl") == 0 )
		{
			sscanf(next, "%s", token);
			for(pNowMtl=color; pNowMtl && strcmp(pNowMtl->name, token); pNowMtl=pNowMtl->pointer)
				;
		}
	}

	// free memory of meterial
	for(pNowMtl=color; pNowMtl; pNowMtl=color)
	{
		color = color->pointer;
		free(pNowMtl);
	}

	fclose(fpt);
	return 1;	// True
}

//added by liu xiang
//基于trimesh库的读文件函数，其实不仅仅可以读obj，skf都可以
//retrun 0 代表失败
int Read3DFile(char *filename, pVer *vertex, pTri *triangle, int *NumVer, int *NumTri)
{
	int numVer, numTri;
	class TriMesh *triMesh = new TriMesh();
	//sprintf(filename, "%s.obj", filename);
	triMesh = triMesh->read(filename);
	if( triMesh != NULL)
	{
		numVer = *NumVer = triMesh->vertices.size();
		numTri = *NumTri = triMesh->faces.size();

		*vertex = (pVer) malloc( numVer * sizeof(Ver));
		if(vertex == NULL)    return 0;
		memset(*vertex, 0, numVer * sizeof(Ver));

		*triangle = (pTri) malloc(numTri * sizeof(Tri));
		if(triangle == NULL)   return 0;
		memset(*triangle, 0, numTri * sizeof(Tri));

		for(int i = 0; i<numVer; i++)
		{
			(*vertex)[i].coor[0] = triMesh->vertices[i][0];
			(*vertex)[i].coor[1] = triMesh->vertices[i][1];
			(*vertex)[i].coor[2] = triMesh->vertices[i][2];
		}

		for(int i = 0; i<numTri; i++)
		{
			(*triangle)[i].v[0] = triMesh->faces[i][0];
			(*triangle)[i].v[1] = triMesh->faces[i][1];
			(*triangle)[i].v[2] = triMesh->faces[i][2];
			(*triangle)[i].NodeName = 3;
	    }
	}
	else
	{
		return 0;
	}
}


/*############################ liu xiang added ###################*/

/*### for read picture #####*/
int DoubleToInt(double s)
{
	int result = s+0.5;
	return result;
}

//用来查找bmBits中左上角和右下角的第一个黑点
//return 1，表示成功找到；return 0表示要么没有位图，要么没有黑点
/*
	*a*********
	***********
	***********
	********b**
	***********
	pLU::a ;pRD ::b
*/
int FindTheRect(unsigned char *bmBits,int width, int height,sPOINT* pLU,sPOINT*pRD)
{
	//初始化两个顶点
	pLU->x = width-1;
	pLU->y = height-1;
	pRD->x = 0;
	pRD->y = 0;	

	//如果没有内容返回 0
	if(bmBits ==NULL)
	{
		return 0;
	}
	
	int tmepCount = 0;//用来检测黑点个数
	for(int i = 0; i < height; i++)
	{
		for(int j = 0; j < width; j++)
		{
			if( *(bmBits + width * i + j) < 255 )
			{
				tmepCount++;
				if(pLU->x > j)
				{
					pLU->x = j;
				}
				if(pLU->y> i)
				{
					pLU->y = i;
				}
				if(pRD->x< j)
				{
					pRD->x = j;
				}
				if(pRD->y< i)
				{
					pRD->y = i;
				}
			}
		}
	}

	//表明没有点
	if(tmepCount==0)
	{
		return 0;
	}

	return 1;
}

int ScalePicture(unsigned char *bmBits,unsigned char *NewbmBits,int ow,int oh,int nw,int nh,double scale,double *centx,double *centy)
{
	sPOINT pLU,pRD;//两个对角坐标
	int i,j,k;
	int count;
	int newPosition;
	//true代表255即白色，false代表黑色
	int signal;
	unsigned char *bmBitTemp;
	double size;//用来计算新的内存中点个数

	size = 0.0;
	signal = TRUE;
	bmBitTemp = (unsigned char *)malloc(sizeof(unsigned char)*nw*oh);
	memset(bmBitTemp,255,sizeof(unsigned char)*nw*oh);

	for(i=0; i<oh;i++)
	{
		if(*(bmBits+i*ow) == 255)
		{
			signal =1;
		}
		else
		{
			signal = 0;
		}
		newPosition =0;
		count = 1;

		for(j = 1; j<ow;j++)
		{

			if(*(bmBits+i*ow+j) == 255 )
			{
				if(signal)
				{
					count ++;
					if(j == (ow -1))
					{
						size = count*scale;
						count = DoubleToInt(size);
						if(count>0)
						{						
							memset(bmBitTemp+i*nw+newPosition,255,count);
						}
					}
				}
				else
				{
					size = count*scale;
					count = DoubleToInt(size);

					if(count>0)
					{
						memset(bmBitTemp+i*nw+newPosition,0,count);
						newPosition = count+newPosition;
					}
					count = 1;

					signal = 1;
				}
			}
			else
			{
				if(!signal)
				{
					count ++;
					if(j == (ow -1))
					{
						size = count*scale;
						count = DoubleToInt(size);
						if(count>0)
						{						
							memset(bmBitTemp+i*nw+newPosition,0,count);
						}
					}
				}
				else
				{
					size = count*scale;
					count = DoubleToInt(size);

					if(count>0)
					{						
						memset(bmBitTemp+i*nw+newPosition,255,count);
						newPosition = count+newPosition;
					}
					count = 1;
					signal = 0;
				}
			}
		}
	}

	for(i=0; i<nw;i++)
	{
		if(*(bmBitTemp+i*nw) == 255)
		{
			signal =1;
		}
		else
		{
			signal = 0;
		}
		newPosition =0;
		count = 1;

		for(j = 1; j<oh;j++)
		{

			if(*(bmBitTemp+j*nw+i) == 255 )
			{
				if(signal)
				{
					count ++;
					if(j ==(oh-1))
					{
						size = count*scale;
						count = DoubleToInt(size);
						if(count>0)
						{
							for(k =0;k<count;k++ )
							{
								*(NewbmBits+(newPosition+k)*nw+i) = 255;
							}
						}
					}
				}
				else
				{
					size = count*scale;
					count = DoubleToInt(size);

					if(count>0)
					{
						for(k =0;k<count;k++ )
						{
							*(NewbmBits+(newPosition+k)*nw+i) = 0;
						}
						newPosition = count+newPosition - 1;
					}
					count = 1;

					signal = 1;
				}
			}
			else
			{
				if(!signal)
				{
					count ++;
					if(j ==(oh-1))
					{
						size = count*scale;
						count = DoubleToInt(size);
						if(count>0)
						{
							for(k =0;k<count;k++ )
							{
								*(NewbmBits+(newPosition+k)*nw+i) = 0;
							}
						}
					}
				}
				else
				{
					size = count*scale;
					count = DoubleToInt(size);

					if(count>0)
					{						
						for(k =0;k<count;k++ )
						{
							*(NewbmBits+(newPosition+k)*nw+i) = 255;
						}
						newPosition = count+newPosition-1;
					}
					count = 1;
					signal = 0;
				}
			}
		}
	}

	k = 0;
	//初始化最大的两个对角坐标点
	pLU.x = nw;
	pLU.y = nh;

	pRD.x = 0;
	pRD.y = 0;

	for(i = 0; i < nh; i++)
	{
		for(j = 0; j < nw; j++)
		{
			if( *(NewbmBits + nw * i + j) < 255 )
			{
				k++;
				if(pLU.x> j)
				{
					pLU.x = j;
				}
				if(pLU.y> i)
				{
					pLU.y = i;
				}
				if(pRD.x< j)
				{
					pRD.x = j;
				}
				if(pRD.y< i)
				{
					pRD.y = i;
				}
			}
		}
	}

	//for(i = 0;i<nh;i++)
	//{
	//	for(j = 0;j<nw;j++)
	//	{
	//		if(*(NewbmBits+i*nw+j)==255)
	//		{
	//			printf("#");
	//		}
	//		else
	//		{
	//			printf("@"); 
	//		}

	//	}
	//	printf("\n");
	//}

	*centx = (pRD.x - pLU.x)/2;
	*centy = (pRD.y - pLU.y)/2;

	free(bmBitTemp);
	return 1;
}

void TranslateImgBits(unsigned char *src,unsigned char *dest,int srcW ,int srcH, int destW,int destH, sPOINT* pLU,sPOINT*pRD)
{
	for(int i = 0; i <destH; i++)
	{
		memcpy((dest+i*destW), (src+(i+(int)pLU->y)*srcW+(int)pLU->x),destW);
	}
}
//要注意照片全是空白的情况
//对图片进行放大缩小可能会造成图像的变形，尤其是宽和高不等比例的放大缩小
//因此平移过程应该保持高和宽一样大小
//因此对于新的宽和高，如果两者大小不同，则取其小着，按照小者比例缩放，其他部分填充空白
int ProccessPicture(unsigned char *bmBits,unsigned char *NewbmBits,int ow,int oh, int nw,int nh,double *centx,double *centy)
{
	sPOINT pLU,pRD;//两个对角坐标
	int i,j,ii;//用作循环变量
	//int centerx,centery;
	int width1, high1;//用来表示平移后的宽和高
	double scale;  //用来表示缩放比例
	unsigned char *bmBits1; //用来表示新的内存地址
	unsigned char *NewbmBits1; //用来表示新的内存地址
	int temp;

	if( FindTheRect(bmBits,ow, oh,&pLU,&pRD)==0 )
	{
		return 0;
	}
	
	width1 = pRD.x - pLU.x+1;
	high1 = pRD.y - pLU.y+1;

	//这里说明是横向扩充了，那还得跟原来比
	if(high1 > width1)
	{
		//width1 = high1 ;
		bmBits1 = (unsigned char *)malloc(sizeof(unsigned char)*high1*high1);
		if(!bmBits1) return 0;
		memset(bmBits1,255,sizeof(unsigned char)*high1*high1);
		//新的高度比原来小
		for(i = 0; i <high1; i++)
		{
			memcpy((bmBits1+i*width1), (bmBits+(i+(int)pLU.y)*ow+(int)pLU.x),width1);
		}

	}
	else
	{
		//high1 = width1   ;
		bmBits1 = (unsigned char *)malloc(sizeof(unsigned char)*width1*width1);
		if(!bmBits1) return 0;
		memset(bmBits1,255,sizeof(unsigned char)*width1*width1);

		temp = (int)(width1 - high1)/2;
		for(i = temp; i <high1-temp; i++)
		{
			memcpy((bmBits1+i*width1), (bmBits+(i+(int)pLU.y)*ow+(int)pLU.x),width1);
		}

	}

	

	//实现平移
	//算法思想就是要将物体置于正中间+
	
	//for(i = 0; i <high1; i++)
	//{
	//	memcpy((bmBits1+i*width1), (bmBits+(i+(int)pLU.y)*ow+(int)pLU.x),width1);
	//}

  /*  for(i = 0;i<high1;i++)
	{
	    for(j = 0;j<width1;j++)
		{
			if(*(bmBits1+i*width1+j)==255)
			{
				printf("#");
			}
			else
			{
			   printf("@"); 
			}

		}
		printf("\n");
	}*/

	//实现缩放
	if(nw>nh)
	{
		ii = nh ;
	}
	else
	{
		ii = nw ;
	}
	scale = (double)(ii-2)/(double)width1;

	NewbmBits1 =  (unsigned char *)malloc(sizeof(unsigned char)*(ii-2)*(ii-2));
	memset(NewbmBits1,255,sizeof(unsigned char)*(ii-2)*(ii-2));
    
	//目标高度或者宽度大于原始宽度或者高度，放大
	ScalePicture(bmBits1,NewbmBits1,width1,high1,ii-2,ii-2,scale,centx,centy);
    free(bmBits1);

	//仅仅用来初始化
	memset(NewbmBits,255,sizeof(unsigned char)*(nw)*(nh));

	
// for test
	//for(i = 0;i<ii-2;i++)
	//{
	//	for(j = 0;j<ii-2;j++)
	//	{
	//		if(*(NewbmBits1+i*(ii-2)+j)==255)
	//		{
	//			printf("#");
	//		}
	//		else
	//		{
	//			printf("@"); 
	//		}

	//	}
	//	printf("\n");
	//}

	//printf("%d",nw);

	if(nw == nh)
	{
		for(i = 0;i<ii-2; i++)
		{
			memcpy((NewbmBits+(i+1)*nw+1),(NewbmBits1+i*(ii-2)),ii-2);
		}
	}
	else if(nw <nh)
	{
		j = (nh-nw)/2;
		for(i = 0;i<ii-2; i++)
		{
			memcpy((NewbmBits+(i+j+1)*ii+1),(NewbmBits1+i*(ii-2)),ii-2);
		}
	}
	else
	{
		j = (nw-nh)/2;
		for(i = 0;i<ii-2; i++)
		{
			memcpy((NewbmBits+(i+1)*ii+1+j),(NewbmBits1+i*(ii-2)),ii-2);
		}

	}
	
//for test
	//for(i = 0;i<nh;i++)
	//{
	//	for(j = 0;j<nw;j++)
	//	{
	//		if(*(NewbmBits+i*nw+j)==255)
	//		{
	//			printf("#");
	//		}
	//		else
	//		{
	//			printf("@"); 
	//		}
	//	}
	//	printf("\n");
	//}

	free(NewbmBits1);
	
	return 1;

}


int ReadPicture(char *filename,unsigned char *bmBits, /*unsigned char *bmColor,*/int w,int h,double *centx,double *centy)
{

	//for test
	int i, j;

	FILE *fp;
	int bmpWidth;//图像的宽
	int bmpHeight;//图像的高
	RGBQUAD *pColorTable;//颜色表指针
	int biBitCount;//图像类型，每像素位数
	int lineByte; //图像每行像素所占的字节数
	BITMAPFILEHEADER fileHeader;
	BITMAPINFOHEADER infoHead;
    unsigned char *bmBitsTemp ;//最后使用的图像像素数据
	unsigned char *bmBitsTemp1 ;//临时存储的图像像素数据

	if( (fp = fopen(filename,"rb")) == NULL )
	{	
        printf("Can't open the file %s!",filename);
		return 0;	// False
	}

	fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1,fp);

    //判断文件的类型是否为bmp文件
    if (fileHeader.bfType != ((WORD)('M' <<8) | 'B'))
	{
        //AfxMessageBox("该文件不是BMP文件");
		printf("This is not a BMP file !");
		fclose(fp);
		return 0;
	}
	  
	fread(&infoHead, sizeof(BITMAPINFOHEADER), 1,fp);
	
	 //获取图像宽、高、每像素所占位数等信息
	 bmpWidth = infoHead.biWidth;
	 bmpHeight = infoHead.biHeight;
	 biBitCount = infoHead.biBitCount;

	 printf("The bitCount is %d!",biBitCount);

	 //bmBitsTemp是存储读取bmp文件，然后处理之后的像素数据
	 bmBitsTemp=(unsigned char*) malloc(sizeof(unsigned char)* bmpWidth * bmpHeight);

	 //定义变量，计算图像每行像素所占的字节数（必须是4的倍数）
	  lineByte=(bmpWidth * biBitCount/8+3)/4*4;
	 //灰度图像有颜色表，且颜色表表项为256
	 if( biBitCount==8)
	 {
		 //申请颜色表所需要的空间，读颜色表进内存
		 //pColorTable=new RGBQUAD[256];
         pColorTable = (RGBQUAD *)malloc(sizeof(RGBQUAD)*256);
		 fread(pColorTable,sizeof(RGBQUAD),256,fp);

		 bmBitsTemp1=(unsigned char*) malloc(sizeof(unsigned char)* lineByte * bmpHeight);
		 fread(bmBitsTemp1,biBitCount/8,lineByte * bmpHeight,fp);

		 //将bmBitsTemp1格式转换成后续可以处理的bmBitsTemp
		 for(i=0;i<bmpHeight;i++)
			 for(j=0;j<bmpWidth;j++)
			 {
				 *(bmBitsTemp+bmpWidth*i+j) = *(bmBitsTemp1+lineByte*i+j);
			 }

		  free(pColorTable);
		  pColorTable = NULL;
		  free(bmBitsTemp1);
		  bmBitsTemp1 =NULL;	 
	 }
	 else if( biBitCount==24)
	 {
		 bmBitsTemp1=(unsigned char*) malloc(sizeof(unsigned char)* lineByte * bmpHeight);
		 fread(bmBitsTemp1,biBitCount/8,lineByte * bmpHeight,fp);

		 //将bmBitsTemp1格式转换成后续可以处理的bmBitsTemp
		 for(i=0;i<bmpHeight;i++)
			 for(j=0;j<bmpWidth;j++)
			 {
				 *(bmBitsTemp+bmpWidth*i+j) = *(bmBitsTemp1+lineByte*i+j*(biBitCount/8));
			 }

			 free(bmBitsTemp1);
			 bmBitsTemp1 =NULL;
	 }
	 else
	 {
		 printf("The file's BitCount is not 8 、24 or 32 !");
		 return 0;
	 }

	 fclose(fp);

	 //for test
	 //for(i = 0;i<bmpHeight;i++)
	 //{
	 //	for(j = 0;j<lineByte;j++)
	 //	{
  //         printf("%d ",*(bmBitsTemp+i*lineByte+j));
	 //	}
	 //	printf("\n");
	 //}

	 ProccessPicture(bmBitsTemp,bmBits,bmpWidth,bmpHeight, w,h,centx,centy);
	 
	 //for test
	 //bmBitsTemp=(unsigned char*)malloc(sizeof(unsigned char)* 5 * 5);
	 //memset(bmBitsTemp,255,sizeof(unsigned char)* 5 * 5);
	 //for( i =1;i<3;i++)
		// for( j =1;j<3;j++)
		// {
		//    *(bmBitsTemp+i*5+j) = 0;
		// }
  //   ProccessPicture(bmBitsTemp,bmBits,5,5, 4,4,centx,centy);
	
	free(bmBitsTemp);
	bmBitsTemp=NULL;

	return 1;
}

//bmBits 是在外边开辟外边释放
int ReadPictureByOpencv(unsigned char *bmBits, /*unsigned char *bmColor,*/int w,int h,CSegObject &object)
{
	//IplImage* imgP = cvLoadImage(filename );
	IplImage* imgP = object.grayimg.GetImage();
	if(!imgP )
	{
		fprintf(stderr, "Can not load image \n");
		return 0;
	}

	CvSize img_cvsize ; 

	//IplImage* imgChaged;
	//img_cvsize.width =imgP->width;
	//img_cvsize.height = imgP->height;
	//imgChaged = cvCreateImage( img_cvsize,IPL_DEPTH_8U,1); 
	//cvConvertImage( imgP, imgChaged);
	////cvSaveImage( "D:\\9.bmp", imgChaged );
	//cvReleaseImage(&imgP); 

	////bmBitsTemp是存储读取bmp文件，然后处理之后的像素数据
	//unsigned char *bmBitsTemp=(unsigned char *)malloc(sizeof(unsigned char)*imgP->imageSize);
	//if(!bmBitsTemp) return 0;
	//memcpy(bmBitsTemp,imgP->imageData,sizeof(unsigned char) * imgChaged->imageSize);
	
	sPOINT pLU, pRD;
	pLU.x = imgP->width-1;
	pLU.y = imgP->height-1;
	pRD.x = 0;
	pRD.y = 0;	
	for (int i=0;i<imgP->height;i++)
	{
		for (int j=0;j<imgP->width;j++)
		{
			BYTE tmp=(BYTE)(imgP->imageData+i*imgP->widthStep)[j*imgP->nChannels+0];
			if(tmp==0)
			{
				if(pLU.x > j)
				{
					pLU.x = j;
				}
				if(pLU.y> i)
				{
					pLU.y = i;
				}
				if(pRD.x< j)
				{
					pRD.x = j;
				}
				if(pRD.y< i)
				{
					pRD.y = i;
				}
			}
		}
	}

	//if( !FindTheRect(bmBitsTemp,img_cvsize.width, img_cvsize.height,&pLU,&pRD) )
	//{
	//	return 0;
	//}

	//int ow = img_cvsize.width;

	int width1 = pRD.x - pLU.x+1;
	int height1 = pRD.y - pLU.y+1;

	//unsigned char *dest=(unsigned char *)malloc(sizeof(unsigned char)*width1*height1);
	//if(!dest) return 0;
	//memset(dest,255,sizeof(unsigned char)*width1*height1);
	//for(int i = 0; i <height1; i++)
	//{
	//	memcpy((dest+i*width1), (bmBitsTemp+(i+(int)pLU.y)*ow+(int)pLU.x),width1);
	//}


	IplImage *convertResult ;
	img_cvsize.width =width1;
	img_cvsize.height = height1;
	convertResult = cvCreateImage( img_cvsize,IPL_DEPTH_8U, 1);
	memset(convertResult->imageData,255,convertResult->imageSize);
	//cvResize(imgChaged, convertResult1, CV_INTER_LINEAR);

	//memcpy(convertResult1->imageData, dest,sizeof(unsigned char) * width1*height1);
	for (int i=0;i<convertResult->height;i++)
	{
		for (int j=0;j<convertResult->width;j++)
		{
			(convertResult->imageData+i*convertResult->widthStep)[j*convertResult->nChannels]=(imgP->imageData+(i+pLU.y)*imgP->widthStep)[(j+pLU.x)*imgP->nChannels+0];
		}
	}

	double wRate,hRate;

	wRate = (double)(w-2)/width1;
	hRate = (double)(h-2)/height1;

	if(wRate>hRate)
	{
		img_cvsize.width =width1*hRate;
		img_cvsize.height = h-2;
	}
	else
	{
		img_cvsize.width =w-2;
		img_cvsize.height = height1*wRate;
	}

	
	//img_cvsize.width =w-2;
	//img_cvsize.height = h-2;

	IplImage *Result ;
	Result = cvCreateImage( img_cvsize,IPL_DEPTH_8U,1); 
	cvResize(convertResult, Result, CV_INTER_LINEAR);
	//cvConvertImage(convertResult1, convertResult);
	//cvSaveImage( "D:\\8.bmp", Result );

	//bmBits = (unsigned char *)malloc(sizeof(unsigned char)*w*h);
	//if(!bmBits) return 0;
	memset(bmBits,255,sizeof(unsigned char)*w*h);

	int temp ;
	if(img_cvsize.width>img_cvsize.height)
	{
		temp = (h - img_cvsize.height)/2;
		for (int i=temp;i<img_cvsize.height+temp;i++)
		{
			memcpy((bmBits+i*w+1),Result->imageData+(i-temp)*Result->widthStep, w-2);
		}
	}
	else
	{
		temp = (w - img_cvsize.width)/2;
		for (int i=1;i<h-1;i++)
		{
			memcpy((bmBits+i*w+temp),Result->imageData+(i-1)*Result->widthStep,img_cvsize.width);
		}
	}
	//for (int i=1;i<h-1;i++)
	//{
	//	memcpy((bmBits+i*w+1),Result->imageData+(i-1)*Result->widthStep, w-2);
	//}

	//memcpy(bmBits,convertResult->imageData,sizeof(unsigned char) * w*h);
	
	//cvReleaseImage(&imgP); 
	cvReleaseImage(&convertResult); 
	cvReleaseImage(&Result); 

	return 1;
}

//bmBits 是在外边开辟外边释放
//这个函数用在为用户手绘的模型提取描述子当中
int LoadPicture(unsigned char *bmBits,int w,int h,CvvImage &picture)
{
	//IplImage* imgP = cvLoadImage(filename );
	IplImage* imgP = picture.GetImage();
	if(!imgP )
	{
		fprintf(stderr, "Can not load image \n");
		return 0;
	}

	CvSize img_cvsize ; 

	sPOINT pLU, pRD;
	pLU.x = imgP->width-1;
	pLU.y = imgP->height-1;
	pRD.x = 0;
	pRD.y = 0;	
	for (int i=0;i<imgP->height;i++)
	{
		for (int j=0;j<imgP->width;j++)
		{
			BYTE tmp=(BYTE)(imgP->imageData+i*imgP->widthStep)[j*imgP->nChannels+0];
			if(tmp==0)
			{
				if(pLU.x > j)
				{
					pLU.x = j;
				}
				if(pLU.y> i)
				{
					pLU.y = i;
				}
				if(pRD.x< j)
				{
					pRD.x = j;
				}
				if(pRD.y< i)
				{
					pRD.y = i;
				}
			}
		}
	}

	//if( !FindTheRect(bmBitsTemp,img_cvsize.width, img_cvsize.height,&pLU,&pRD) )
	//{
	//	return 0;
	//}

	//int ow = img_cvsize.width;

	int width1 = pRD.x - pLU.x+1;
	int height1 = pRD.y - pLU.y+1;

	//unsigned char *dest=(unsigned char *)malloc(sizeof(unsigned char)*width1*height1);
	//if(!dest) return 0;
	//memset(dest,255,sizeof(unsigned char)*width1*height1);
	//for(int i = 0; i <height1; i++)
	//{
	//	memcpy((dest+i*width1), (bmBitsTemp+(i+(int)pLU.y)*ow+(int)pLU.x),width1);
	//}


	IplImage *convertResult ;
	img_cvsize.width =width1;
	img_cvsize.height = height1;
	convertResult = cvCreateImage( img_cvsize,IPL_DEPTH_8U, 1);
	memset(convertResult->imageData,255,convertResult->imageSize);
	//cvResize(imgChaged, convertResult1, CV_INTER_LINEAR);

	//memcpy(convertResult1->imageData, dest,sizeof(unsigned char) * width1*height1);
	for (int i=0;i<convertResult->height;i++)
	{
		for (int j=0;j<convertResult->width;j++)
		{
			(convertResult->imageData+i*convertResult->widthStep)[j*convertResult->nChannels]=(imgP->imageData+(i+pLU.y)*imgP->widthStep)[(j+pLU.x)*imgP->nChannels+0];
		}
	}

	double wRate,hRate;

	wRate = (double)(w-2)/width1;
	hRate = (double)(h-2)/height1;

	if(wRate>hRate)
	{
		img_cvsize.width =width1*hRate;
		img_cvsize.height = h-2;
	}
	else
	{
		img_cvsize.width =w-2;
		img_cvsize.height = height1*wRate;
	}


	//img_cvsize.width =w-2;
	//img_cvsize.height = h-2;

	IplImage *Result ;
	Result = cvCreateImage( img_cvsize,IPL_DEPTH_8U,1); 
	cvResize(convertResult, Result, CV_INTER_LINEAR);
	//cvConvertImage(convertResult1, convertResult);
	//cvSaveImage( "D:\\8.bmp", Result );

	//bmBits = (unsigned char *)malloc(sizeof(unsigned char)*w*h);
	//if(!bmBits) return 0;
	memset(bmBits,255,sizeof(unsigned char)*w*h);

	int temp ;
	if(img_cvsize.width>img_cvsize.height)
	{
		temp = (h - img_cvsize.height)/2;
		for (int i=temp;i<img_cvsize.height+temp;i++)
		{
			memcpy((bmBits+i*w+1),Result->imageData+(i-temp)*Result->widthStep, w-2);
		}
	}
	else
	{
		temp = (w - img_cvsize.width)/2;
		for (int i=1;i<h-1;i++)
		{
			memcpy((bmBits+i*w+temp),Result->imageData+(i-1)*Result->widthStep,img_cvsize.width);
		}
	}

	//cvReleaseImage(&imgP); 
	cvReleaseImage(&convertResult); 
	cvReleaseImage(&Result); 

	return 1;
}


//bmBits 是在外边开辟外边释放
int ReadPictureByOpencvOld(unsigned char *bmBits, /*unsigned char *bmColor,*/int w,int h,CSegObject &object)
{
	//IplImage* imgP = cvLoadImage(filename );
	IplImage* imgP = object.grayimg.GetImage();
	if(!imgP )
	{
		fprintf(stderr, "Can not load image \n");
		return 0;
	}

	CvSize img_cvsize ; 

	//IplImage* imgChaged;
	//img_cvsize.width =imgP->width;
	//img_cvsize.height = imgP->height;
	//imgChaged = cvCreateImage( img_cvsize,IPL_DEPTH_8U,1); 
	//cvConvertImage( imgP, imgChaged);
	////cvSaveImage( "D:\\9.bmp", imgChaged );
	//cvReleaseImage(&imgP); 

	////bmBitsTemp是存储读取bmp文件，然后处理之后的像素数据
	//unsigned char *bmBitsTemp=(unsigned char *)malloc(sizeof(unsigned char)*imgP->imageSize);
	//if(!bmBitsTemp) return 0;
	//memcpy(bmBitsTemp,imgP->imageData,sizeof(unsigned char) * imgChaged->imageSize);


	sPOINT pLU, pRD;
	pLU.x = imgP->width-1;
	pLU.y = imgP->height-1;
	pRD.x = 0;
	pRD.y = 0;	
	for (int i=0;i<imgP->height;i++)
	{
		for (int j=0;j<imgP->width;j++)
		{
			BYTE tmp=(BYTE)(imgP->imageData+i*imgP->widthStep)[j*imgP->nChannels+0];
			if(tmp==0)
			{
				if(pLU.x > j)
				{
					pLU.x = j;
				}
				if(pLU.y> i)
				{
					pLU.y = i;
				}
				if(pRD.x< j)
				{
					pRD.x = j;
				}
				if(pRD.y< i)
				{
					pRD.y = i;
				}
			}
		}
	}

	//if( !FindTheRect(bmBitsTemp,img_cvsize.width, img_cvsize.height,&pLU,&pRD) )
	//{
	//	return 0;
	//}

	//int ow = img_cvsize.width;

	int width1 = pRD.x - pLU.x+1;
	int height1 = pRD.y - pLU.y+1;

	//unsigned char *dest=(unsigned char *)malloc(sizeof(unsigned char)*width1*height1);
	//if(!dest) return 0;
	//memset(dest,255,sizeof(unsigned char)*width1*height1);
	//for(int i = 0; i <height1; i++)
	//{
	//	memcpy((dest+i*width1), (bmBitsTemp+(i+(int)pLU.y)*ow+(int)pLU.x),width1);
	//}


	IplImage *convertResult ;
	img_cvsize.width =width1;
	img_cvsize.height = height1;
	convertResult = cvCreateImage( img_cvsize,IPL_DEPTH_8U, 1);
	memset(convertResult->imageData,255,convertResult->imageSize);
	//cvResize(imgChaged, convertResult1, CV_INTER_LINEAR);

	//memcpy(convertResult1->imageData, dest,sizeof(unsigned char) * width1*height1);
	for (int i=0;i<convertResult->height;i++)
	{
		for (int j=0;j<convertResult->width;j++)
		{
			(convertResult->imageData+i*convertResult->widthStep)[j*convertResult->nChannels]=(imgP->imageData+(i+pLU.y)*imgP->widthStep)[(j+pLU.x)*imgP->nChannels+0];
		}
	}

	double wRate,hRate;

	wRate = (double)(w-2)/width1;
	hRate = (double)(h-2)/height1;

	if(wRate>hRate)
	{
		img_cvsize.width =width1*hRate;
		img_cvsize.height = h-2;
	}
	else
	{
		img_cvsize.width =w-2;
		img_cvsize.height = height1*wRate;
	}


	//img_cvsize.width =w-2;
	//img_cvsize.height = h-2;

	IplImage *Result ;
	Result = cvCreateImage( img_cvsize,IPL_DEPTH_8U,1); 
	cvResize(convertResult, Result, CV_INTER_LINEAR);
	//cvConvertImage(convertResult1, convertResult);
	cvSaveImage( "D:\\8.bmp", Result );

	//bmBits = (unsigned char *)malloc(sizeof(unsigned char)*w*h);
	//if(!bmBits) return 0;
	memset(bmBits,255,sizeof(unsigned char)*w*h);

	int temp ;
	if(img_cvsize.width>img_cvsize.height)
	{
		temp = (h - img_cvsize.height)/2;
		for (int i=temp;i<img_cvsize.height+temp;i++)
		{
			memcpy((bmBits+i*w+1),Result->imageData+(i-temp)*Result->widthStep, w-2);
		}
	}
	else
	{
		temp = (w - img_cvsize.width)/2;
		for (int i=1;i<h-1;i++)
		{
			memcpy((bmBits+i*w+temp),Result->imageData+(i-1)*Result->widthStep,img_cvsize.width);
		}
	}
	//for (int i=1;i<h-1;i++)
	//{
	//	memcpy((bmBits+i*w+1),Result->imageData+(i-1)*Result->widthStep, w-2);
	//}

	//memcpy(bmBits,convertResult->imageData,sizeof(unsigned char) * w*h);

	//cvReleaseImage(&imgP); 
	cvReleaseImage(&convertResult); 
	cvReleaseImage(&Result); 

	return 1;
}
/*############################ the end ###################*/

void SaveObj(char *filename, pVer vertex, pTri triangle, int NumVer, int NumTri)
{
	int		i, j;
	FILE	*fpt;

	fpt = fopen(filename, "w");

	// save each vertex to .obj file
	for(i=0; i<NumVer; i++)
		fprintf(fpt, "v %.6f %.6f %.6f\n", vertex[i].coor[0], vertex[i].coor[1], vertex[i].coor[2]);

	// save each triangle (face)
	for(i=0; i<NumTri; i++)
	{
		fprintf(fpt, "f");
		for(j=0; j<triangle[i].NodeName; j++)	// record the number of vertex in this triangle
			fprintf(fpt, " %d", triangle[i].v[j]+1);
		fprintf(fpt, "\n");
	}

	fclose(fpt);
}


void SaveMergeObj(char *filename, pVer vertex1, pTri triangle1, int NumVer1, int NumTri1, 
								  pVer vertex2, pTri triangle2, int NumVer2, int NumTri2)
{
	int		i, j;
	FILE	*fpt;

	fpt = fopen(filename, "w");

	fprintf(fpt, "# src model: %d vertices; %d triangles\n", NumVer1, NumTri1);
	fprintf(fpt, "\nmtllib color.mtl\n\n");

	fprintf(fpt, "g src\n");
	// save each vertex to .obj file
	for(i=0; i<NumVer1; i++)
		fprintf(fpt, "v %.6f %.6f %.6f\n", vertex1[i].coor[0], vertex1[i].coor[1], vertex1[i].coor[2]);

	fprintf(fpt, "\nusemtl red\n\n");
	// save each triangle (face)
	for(i=0; i<NumTri1; i++)
	{
		fprintf(fpt, "f");
		for(j=0; j<triangle1[i].NodeName; j++)	// record the number of vertex in this triangle
			fprintf(fpt, " %d", triangle1[i].v[j]+1);
		fprintf(fpt, "\n");
	}

	fprintf(fpt, "\n# dest model: %d vertices; %d triangles\n", NumVer2, NumTri2);
	fprintf(fpt, "g dest\n");
	// save each vertex to .obj file
	for(i=0; i<NumVer2; i++)
		fprintf(fpt, "v %.6f %.6f %.6f\n", vertex2[i].coor[0], vertex2[i].coor[1], vertex2[i].coor[2]);

	fprintf(fpt, "\nusemtl bule\n\n");
	// save each triangle (face)
	for(i=0; i<NumTri2; i++)
	{
		fprintf(fpt, "f");
		for(j=0; j<triangle2[i].NodeName; j++)	// record the number of vertex in this triangle
			fprintf(fpt, " %d", triangle2[i].v[j]+1+NumVer1);
		fprintf(fpt, "\n");
	}

	fclose(fpt);
}



