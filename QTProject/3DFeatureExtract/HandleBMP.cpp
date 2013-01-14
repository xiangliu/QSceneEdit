/************************************************************************/
/* it's used by 3D feature extract                                                                     */
/************************************************************************/

//#include "stdafx.h"
//#include <afx.h>
#include "HandleBMP.h" 

#define PI 3.14159265358979

HandleBMP::HandleBMP()
{
	this->pBmpBuf = NULL;
	this->biBitCount = 0;
	this->filePathName = "";
}

bool HandleBMP::readBMP(char* filePath)
{
	//不知道这个tmp在这里是干什么用的
	//char *tmp = filePath.GetBuffer(filePath.GetLength());
    FILE *fp=fopen(filePath,"rb");

	BITMAPFILEHEADER fileHeader;
	fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1,fp);

    //判断文件的类型是否为bmp文件
    if (fileHeader.bfType != ((WORD)('M' <<8) | 'B'))
	{
       // AfxMessageBox("该文件不是BMP文件");
		fclose(fp);
	}

	/*
	if(fileheader.bfType!=0x4D42)
	{//不是BMP位图文件
		pDC->TextOut(100,200,"无位图文件 请选择位图文件");
		fclose(fp);
		return ;
	}*/


	BITMAPINFOHEADER infoHead;  
	fread(&infoHead, sizeof(BITMAPINFOHEADER), 1,fp);

	
	 //获取图像宽、高、每像素所占位数等信息
	 this->bmpWidth = infoHead.biWidth;
	 this->bmpHeight = infoHead.biHeight;
	 this->biBitCount = infoHead.biBitCount;

	 //定义变量，计算图像每行像素所占的字节数（必须是4的倍数）
	  lineByte=(bmpWidth * biBitCount/8+3)/4*4;
	 //灰度图像有颜色表，且颜色表表项为256
	 if( this->biBitCount==8)
	 {
		 //申请颜色表所需要的空间，读颜色表进内存
		 this->pColorTable=new RGBQUAD[256];
		 fread(pColorTable,sizeof(RGBQUAD),256,fp);
	 }
	 this->pBmpBuf=new unsigned char[lineByte * bmpHeight];

     fread(pBmpBuf,1,lineByte * bmpHeight,fp);

	 fclose(fp);

	return 1;
}

//
//bool HandleBMP::presentBMP(CDC *pDC,CPoint leftTop)
//{
//		int i, j;
//        RGBQUAD temp1;
//		for(i = 0; i < this->bmpHeight; i ++)
//		{
//			for(j = 0; j < this->bmpWidth;j ++)
//			{
//				if(this->biBitCount ==24)
//				{
//				  pDC->SetPixel(j + leftTop.x, this->bmpHeight - i - 1 + leftTop.y, 
//					RGB(
//					this->pBmpBuf[i * this->lineByte + 3*j+2],
//					this->pBmpBuf[i * this->lineByte + 3*j+1],
//					this->pBmpBuf[i * this->lineByte + 3*j+0])
//					);
//				}
//				else //if(this->biBitCount ==1)
//				{
//					temp1 = this->pColorTable[this->pBmpBuf[i * this->lineByte + j]];
//                  
//					pDC->SetPixel(j + leftTop.x, this->bmpHeight - i - 1 + leftTop.y, 
//						RGB(temp1.rgbRed,temp1.rgbGreen,temp1.rgbBlue));
//				}
//				
//			}
//		}
//	
//	return 1;
//
//}

bool HandleBMP::saveBMP(char *bmpName, unsigned char *imgBuf, 
					  int width, int height, int biBitCount, 
					  RGBQUAD *pColorTable)
{
	//如果位图数据指针为0，则没有数据传入，函数返回
	if(!imgBuf)
		return 0;

	//颜色表大小，以字节为单位，灰度图像颜色表为1024字节，彩色图像颜色表大小为0
	int colorTablesize=0;
	if(biBitCount==8)
		colorTablesize=1024;
	//待存储图像数据每行字节数为4的倍数
	int lineByte=(width * biBitCount/8+3)/4*4;
	//以二进制写的方式打开文件
	FILE *fp=fopen(bmpName,"wb");
	if(fp==0) 
		return 0;
	//申请位图文件头结构变量，填写文件头信息
	BITMAPFILEHEADER fileHead;
	fileHead.bfType = 0x4D42;
	//bmp类型
	//bfSize是图像文件4个组成部分之和
	fileHead.bfSize= sizeof(BITMAPFILEHEADER)+ 
		sizeof(BITMAPINFOHEADER)+ colorTablesize + lineByte*height;
	fileHead.bfReserved1 = 0;
	fileHead.bfReserved2 = 0;
	//bfOffBits是图像文件前3个部分所需空间之和
	fileHead.bfOffBits=54+colorTablesize;
	//写文件头进文件
	fwrite(&fileHead, sizeof(BITMAPFILEHEADER),1, fp);
	//申请位图信息头结构变量，填写信息头信息
	BITMAPINFOHEADER head;
	head.biBitCount=biBitCount;
	head.biClrImportant=0;
	head.biClrUsed=0;
	head.biCompression=0;
	head.biHeight=height;
	head.biPlanes=1;
	head.biSize=40;
	head.biSizeImage=lineByte*height;
	head.biWidth=width;
	head.biXPelsPerMeter=0;
	head.biYPelsPerMeter=0;
	//写位图信息头进内存
	fwrite(&head, sizeof(BITMAPINFOHEADER),1, fp);
	//如果灰度图像，有颜色表，写入文件 
	if(biBitCount==8)fwrite(pColorTable, sizeof(RGBQUAD),256, fp);
	//写位图数据进文件
	fwrite(imgBuf, height*lineByte, 1, fp);
	//关闭文件
	fclose(fp);
	delete [] pBmpBuf;
	return 1;
}


//bool HandleBMP::drawBitmapFormFile(CString   BitmapFilePath,CDC   *pDC,CRect   rect)     
//{     
//	HANDLE   filehandle=::LoadImage(NULL,BitmapFilePath,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);     
//	if(filehandle!=NULL)     
//	{     
//		CBitmap   bmp;     
//		if(bmp.Attach(filehandle))     
//		{     
//			BITMAP   bmpInfo;     
//			bmp.GetBitmap(&bmpInfo);     
//			CDC   dcMemory;     
//			dcMemory.CreateCompatibleDC(pDC);     
//			dcMemory.SelectObject(&bmp);     
//			pDC->SetStretchBltMode(HALFTONE);     
//			pDC->StretchBlt(rect.left,rect.top,rect.Width(),rect.Height(),&dcMemory,0,0,bmpInfo.bmWidth,bmpInfo.bmHeight,SRCCOPY);     
//			bmp.Detach();     
//		}     
//	}   
//	return 1;
//} 
//
bool HandleBMP::TurnGrey()
{
	if(this->pBmpBuf ==NULL)
	{
		//AfxMessageBox(_T("未读入数据!"),MB_OK);
		return 0;
	}

	int grey;
	BYTE *p;
	int bytePerLine =(((this->bmpWidth*this->biBitCount+31)/32)*4);

	if (this->biBitCount==8)
	{
		// 灰度映射表
		BYTE grayMap[256];
		// 计算灰度映射表（保存各个颜色的灰度值），并更新调色板
		int	i,j;
		for (i = 0; i < 256; i ++)
		{
			// 计算该颜色对应的灰度值
			grayMap[i] = (BYTE)(0.299 * this->pColorTable[i].rgbRed +

				0.587 * this->pColorTable[i].rgbGreen +

				0.114 * this->pColorTable[i].rgbBlue + 0.5);			
			// 更新调色板红色分量
			this->pColorTable[i].rgbRed = i;	

			// 更新调色板绿色分量
			this->pColorTable[i].rgbGreen = i;	

			// 更新调色板蓝色分量
			this->pColorTable[i].rgbBlue = i;

			// 更新调色板保留位
			this->pColorTable[i].rgbReserved = 0;

		}

		//逐行扫描，转为灰度图
		for(i = 0; i < this->bmpHeight; i++)
		{

			//逐列扫描
			for(j = 0; j < this->bmpWidth; j++)
			{
				// 指向第i行，第j个象素的指针
				p = (unsigned char*)this->pBmpBuf + bytePerLine * (this->bmpHeight - 1 - i) + j;
				// 变换
				*p = grayMap[*p];
			}
		}
	}
}
//
//	//bitcount=24 彩色图像
//	else if(this->biBitCount==24)
//	{
//		for(int x=0;x<this->bmpHeight;x++)
//		{
//			for(int y=0;y<this->bmpWidth;y++)
//			{
//				p = this->pBmpBuf + bytePerLine * x + y * 3;				
//				grey = (BYTE)(0.299 * p[2] + 0.587 * p[1] + 0.114 * p[0]);
//				p[0] = grey;
//				p[1] = grey;
//				p[2] = grey;
//			}
//		}
//	}
//	return true;
//}
//
//bool HandleBMP::BMPToHSI()
//{
//	if(this->pBmpBuf ==NULL)
//	{
//		AfxMessageBox(_T("未读入数据!"),MB_OK);
//		return 0;
//	}
//	//临时存储变量
//	this->Hue = new double[this->bmpHeight*this->bmpWidth];
//	this->Saturation = new double[this->bmpHeight*this->bmpWidth];
//	this->Intensity = new double[this->bmpHeight*this->bmpWidth];
//
//	unsigned char R,G,B;
//	double I,S,H;
//	int bytePerLine =(((this->bmpWidth*this->biBitCount+31)/32)*4);
//
//	for(int i=0; i<this->bmpHeight; i++)
//	{
//		for(int j=0; j<this->bmpWidth; j++)
//		{	
//			//获取RGB值 
//			B = this->pBmpBuf[i * this->lineByte + 3*j+0];
//			G = this->pBmpBuf[i * this->lineByte + 3*j+1];
//			R = this->pBmpBuf[i * this->lineByte + 3*j+2];
//
//			unsigned char a = (R<G)?R:G; 
//			unsigned char min = (a<B)?a:B;
//
//			double sqr = sqrt((double)( (R-G)*(R-G) + (R-B)*(G-B) ));
//			if(sqr != 0)
//				H = acos(((2*R-G-B)/2*1.0)/sqr);
//			else
//				H = acos(0.0);
//
//			if(B>G)
//			{				
//				H = 2*PI - H;
//			}
//
//			S = 1-(3*min)/((R+G+B)*1.0);
//
//			I =  double(R + G + B)/3;
//
//			Hue[this->bmpWidth*i+j] = H;
//			Saturation[this->bmpWidth*i+j] = S;
//			Intensity[this->bmpWidth*i+j] = I;
//		}
//	}
//	return 1;
//	/********************************************************************
//	/*写的原来的方法
//    int k ,j;
//	if(this->pBmpBuf ==NULL)
//	{
//		AfxMessageBox(_T("未读入数据!"),MB_OK);
//		return 0;
//	}
//	//申请保存HSI的临时保存空间
//	this->pHsiBuf = new unsigned char[this->bmpHeight*this->lineByte];
//
//	//临时变量
//    unsigned int r,g,b,h,s,i,xita;
//    double temp1,temp2;
//	for(k = 0; k < this->bmpHeight; k ++)
//	{
//		for(j = 0; j < this->bmpWidth;j ++)
//		{
//			
//				r=this->pBmpBuf[k * this->lineByte + 3*j+2];
//				g=this->pBmpBuf[k * this->lineByte + 3*j+1];
//				b=this->pBmpBuf[k * this->lineByte + 3*j+0];
//				
//                i = (r+g+b)/3;
//				//xita = acos((double)(r*2-g-b)/(2*sqrt((r-g)*(r-b))));
//				
//                temp1 = (r-g)*(r-b);				
//				temp2 = 2*sqrt(temp1);
//                temp1 = (r*2-g-b);
//				temp2 = temp1/temp2;
//				xita= acos(temp2);
//				
//                //xita = acos(0.3);
//				if(b<=g)
//				{
//					if(r<=b)
//					{
//					   s = 1-3*r/(r+b+g);
//					}else
//					{
//					   s = 1-3*b/(r+b+g);
//					}
//				   h = xita;
//
//				}//if
//				else
//				{
//					if(r<=g)
//					{
//						s = 1-3*r/(r+b+g);
//					}else
//					{
//						s = 1-3*g/(r+b+g);
//					}
//						h = 360-xita;
//				}//else
//				this->pHsiBuf[k * this->lineByte + 3*j+2]=h;
//				this->pHsiBuf[k * this->lineByte + 3*j+2]=s;
//				this->pHsiBuf[k * this->lineByte + 3*j+2]=i;
//		}
//	}
//   delete [] pHsiBuf;
//   this->pHsiBuf = NULL;
//   return 1;
//   /********************************************************************/
//   
//}
//
//
//bool HandleBMP::HSIToBMP()
//{
//	unsigned char R,G,B;
//
//	double H,S,I;
//	double valueR,valueG,valueB;
//
//	int bytePerLine = (((this->bmpWidth*this->biBitCount+31)/32)*4);
//
//	for(int i=0; i<this->bmpHeight; i++)
//	{
//		for(int j=0; j<this->bmpWidth;j++)
//		{
//			I = Intensity[i*this->bmpWidth+j];
//			H = Hue[i*this->bmpWidth+j];
//			S = Saturation[i*this->bmpWidth+j];
//
//			if((H>=0)&&(H<=2*PI/3))
//			{
//				valueB = I*(1-S);
//				valueR = I*(1+(S*cos(H)*1.0)/(cos((PI/3)-H)));
//				valueG = 3*I-(valueB+valueR);
//			}	
//
//			else if((H>2*PI/3)&&(H<=4*PI/3))
//			{
//				valueR = I*(1-S);
//				valueG = I*(1+(S*(cos(H-2*PI/3))*1.0)/(cos(PI - H)));
//				valueB = 3*I-(valueR+valueG);
//			}
//
//			else if((H>4*PI/3)&&(H<=2*PI))
//			{
//				valueG = I*(1-S);
//				valueB = I*(1+(S*cos(H-4*PI/3)*1.0)/(cos(5*PI/3-H)));
//				valueR = 3.0*I-valueG-valueB;
//			}
//			if(valueR>255) 
//				R=255;
//			else 
//				R=(unsigned char)(valueR+0.5);
//			if(valueG>255) 
//				G=255;
//			else 
//				G = (unsigned char)(valueG+0.5);
//			if(valueB>255) 
//				B=255;
//			else 
//				B = (unsigned char)(valueB+0.5);				
//
//			
//			this->pBmpBuf[i * this->lineByte + 3*j+0] = B;
//		    this->pBmpBuf[i * this->lineByte + 3*j+1] = G;
//			this->pBmpBuf[i * this->lineByte + 3*j+2] = R;
//		}
//	}
//	delete [] Hue;
//	delete [] Saturation;
//	delete [] Intensity;
//     Hue = NULL;
//	 Saturation = NULL;
//	 Intensity = NULL;
//   return 1;
//}
//
//
////进行直方图变换
//bool HandleBMP::zhiFangTuStren()
//{
//
//	if(this->pBmpBuf ==NULL)
//	{
//		AfxMessageBox(_T("未读入数据!"),MB_OK);
//		return 0;
//	}
//	if(this->biBitCount==8)
//	{
//		//int k = pow(2,(double)this->biBitCount);//计算灰度图像有多少个像素 
//		int k = 256;
//		unsigned double* digitalSum = new unsigned double[k];
//		for(int i = 0;i<k;i++)
//		{
//			digitalSum[i] = 0;	
//		}//初始化为0
//	    
//		//统计
//		for(int i = 0; i < this->bmpHeight; i ++)
//		{
//			for(int j = 0; j < this->bmpWidth;j ++)
//			{
//				digitalSum[this->pBmpBuf[i * this->lineByte + j]]++;					
//			}
//		}
//		//汇总
//		for(int i = 1;i<k;i++)
//		{
//			digitalSum[i] += digitalSum[i-1];
//		}
//		//确定修改后的像素值
//		for(int i = 0;i<k;i++)
//		{
//			double temp1;
//			temp1 = digitalSum[i]/(double)digitalSum[k-1];
//			digitalSum[i] = temp1*(k-1);
//		}
//		//直方图均衡化
//		for(int i = 0; i < this->bmpHeight; i ++)
//		{
//			for(int j = 0; j < this->bmpWidth;j ++)
//			{
//				this->pBmpBuf[i * this->lineByte + j] = digitalSum[this->pBmpBuf[i * this->lineByte + j]] ;					
//			}
//		}
//		delete [] digitalSum;		
//	}
//	else if(this->biBitCount ==24)
//	{
//		this->BMPToHSI();
//        int temp = this->bmpHeight*this->bmpWidth;
//		int k = 256;//计算灰度图像有多少个像素 
//		unsigned double* digitalSum = new unsigned double[k];
//		for(int i = 0;i<k;i++)
//		{
//			digitalSum[i] = 0;	
//		}//初始化为0
//
//		//统计
//		for(int i = 0; i < temp; i ++)
//		{
//             digitalSum[(int)this->Intensity[i]]++;	
//		}
//		//汇总
//		for(int i = 1;i<k;i++)
//		{
//			digitalSum[i] += digitalSum[i-1];
//		}
//		//确定修改后的像素值
//		for(int i = 0;i<k;i++)
//		{
//			double temp1;
//			temp1 = digitalSum[i]/(double)digitalSum[k-1];
//			digitalSum[i] = temp1*(k-1);
//		}
//		//直方图均衡化
//		for(int i = 0; i < temp; i ++)
//		{
//			this->Intensity[i] = digitalSum[(int)this->Intensity[i]];
//		}
//
//		this->HSIToBMP();
//		delete [] digitalSum;	
//
//	  
//	}
//
//	AfxMessageBox(_T("直方图均衡化完成!"),MB_OK);
//	return 1;
//}
////默认采用3*3模板，系数分别是1,2,1,2,4,2,1,2,1
//bool HandleBMP::linearSmoothing(int size)
//{
//	if(this->pBmpBuf ==NULL)
//	{
//		AfxMessageBox(_T("未读入数据!"),MB_OK);
//		return 0;
//	}
//	if(this->biBitCount==8)
//	{
//		for(int i = 1; i < this->bmpHeight-1; i ++)
//		{
//			for(int j = 1; j < this->bmpWidth-1;j ++)
//			{
//				this->pBmpBuf[i * this->lineByte + j] =
//					((int)this->pBmpBuf[(i-1) * this->lineByte + j-1]*1+
//					this->pBmpBuf[(i-1) * this->lineByte + j]*2+
//					this->pBmpBuf[(i-1) * this->lineByte + j+1]*1+
//					this->pBmpBuf[i * this->lineByte + j-1]*2+
//					this->pBmpBuf[i * this->lineByte + j]*4+
//					this->pBmpBuf[i * this->lineByte + j+1]*2+
//					this->pBmpBuf[(i+1) * this->lineByte + j-1]*1+
//					this->pBmpBuf[(i+1) * this->lineByte + j]*2+
//					this->pBmpBuf[(i+1) * this->lineByte + j+1]*1)/16;					
//			}
//		}
//	}
//	else if(this->biBitCount == 24)
//	{
//		this->BMPToHSI();
//		for(int i = 1; i < this->bmpHeight-1; i ++)
//		{
//			for(int j = 1; j < this->bmpWidth-1;j ++)
//			{
//				this->Intensity[i * this->bmpWidth + j] =
//					((int)this->Intensity[(i-1) * this->bmpWidth + j-1]*1+
//					this->Intensity[(i-1) * this->bmpWidth + j]*2+
//					this->Intensity[(i-1) * this->bmpWidth + j+1]*1+
//					this->Intensity[i * this->bmpWidth + j-1]*2+
//					this->Intensity[i * this->bmpWidth + j]*4+
//					this->Intensity[i * this->bmpWidth + j+1]*2+
//					this->Intensity[(i+1) * this->bmpWidth + j-1]*1+
//					this->Intensity[(i+1) * this->bmpWidth + j]*2+
//					this->Intensity[(i+1) * this->bmpWidth + j+1]*1)/16;					
//			}
//		}
//		this->HSIToBMP();
//	}
//    return 1;
//}
////非线性滤波器，实现的是中值滤波器
//bool HandleBMP::UNLinearSmoothing()
//{
//	if(this->pBmpBuf ==NULL)
//	{
//		AfxMessageBox(_T("未读入数据!"),MB_OK);
//		return 0;
//	}
//	if(this->biBitCount == 8)
//	{
//		unsigned char* temp = new unsigned char[9];//开辟用来排序的临时空间
//
//		for(int i = 1; i < this->bmpHeight-1; i ++)
//		{
//			for(int j = 1; j < this->bmpWidth-1;j ++)
//			{
//				//this->pBmpBuf[i * this->lineByte + j] =
//					temp[0]=this->pBmpBuf[(i-1) * this->lineByte + j-1];
//					temp[1]=this->pBmpBuf[(i-1) * this->lineByte + j];
//					temp[2]=this->pBmpBuf[(i-1) * this->lineByte + j+1];
//					temp[3]=this->pBmpBuf[i * this->lineByte + j-1];
//					temp[4]=this->pBmpBuf[i * this->lineByte + j];
//					temp[5]=this->pBmpBuf[i * this->lineByte + j+1];
//					temp[6]=this->pBmpBuf[(i+1) * this->lineByte + j-1];
//					temp[7]=this->pBmpBuf[(i+1) * this->lineByte + j];
//					temp[8]=this->pBmpBuf[(i+1) * this->lineByte + j+1];	
//					//插入排序
//					for(int y=1; y<9; y++)
//					{
//						unsigned char temp1 = *(temp+y);
//						for(int f = y-1;f>=0;f--)
//						{
//							if(f==0)
//							{
//							}
//							if(*(temp+f)>temp1)
//							{ 
//								if(f==0)
//								{
//									*(temp+f+1) = *(temp+f);
//									*(temp+f)=temp1;
//								}else
//								{
//									*(temp+f+1) = *(temp+f);
//								}
//							}
//							else
//							{
//								*(temp+f+1) = temp1;
//								break;		
//							}
//						}//for f
//					}//for y
//					//选择中值进行赋值
//					this->pBmpBuf[i * this->lineByte + j] = temp[4];
//			}//for j
//		}//for i
//		delete [] temp;
//	}
//	else if(this->biBitCount == 24 )
//	{
//		unsigned char* temp = new unsigned char[9];//开辟用来排序的临时空间
//
//		this->BMPToHSI();
//		for(int i = 1; i < this->bmpHeight-1; i ++)
//		{
//			for(int j = 1; j < this->bmpWidth-1;j ++)
//			{
//				//this->pBmpBuf[i * this->lineByte + j] =
//				temp[0]=this->Intensity[(i-1) * this->bmpWidth + j-1];
//				temp[1]=this->Intensity[(i-1) * this->bmpWidth + j];
//				temp[2]=this->Intensity[(i-1) * this->bmpWidth + j+1];
//				temp[3]=this->Intensity[i * this->bmpWidth + j-1];
//				temp[4]=this->Intensity[i * this->bmpWidth + j];
//				temp[5]=this->Intensity[i * this->bmpWidth + j+1];
//				temp[6]=this->Intensity[(i+1) * this->bmpWidth + j-1];
//				temp[7]=this->Intensity[(i+1) * this->bmpWidth + j];
//				temp[8]=this->Intensity[(i+1) * this->bmpWidth + j+1];	
//				//插入排序
//				for(int y=1; y<9; y++)
//				{
//					unsigned char temp1 = *(temp+y);
//					for(int f = y-1;f>=0;f--)
//					{
//						if(f==0)
//						{
//						}
//						if(*(temp+f)>temp1)
//						{ 
//							if(f==0)
//							{
//								*(temp+f+1) = *(temp+f);
//								*(temp+f)=temp1;
//							}else
//							{
//								*(temp+f+1) = *(temp+f);
//							}
//						}
//						else
//						{
//							*(temp+f+1) = temp1;
//							break;		
//						}
//					}//for f
//				}//for y
//				//选择中值进行赋值
//				this->Intensity[i * this->bmpWidth + j] = temp[4];
//			}//for j
//		}//for i
//		delete [] temp;
//		this->HSIToBMP();
//	}
//    return 1;
//}
//
//bool HandleBMP::firstOrderDifferential()
//{
//	if(this->pBmpBuf ==NULL)
//	{
//		AfxMessageBox(_T("未读入数据!"),MB_OK);
//		return 0;
//	}   
//    if(this->biBitCount == 8 )
//	{
//		int temp1,temp2;//临时变量
//
//		for(int i = 1; i < this->bmpHeight-1; i ++)
//		{
//			for(int j = 1; j < this->bmpWidth-1;j ++)
//			{
//				temp1 = (this->pBmpBuf[(i+1) * this->lineByte + j-1]*1+
//					this->pBmpBuf[(i+1) * this->lineByte + j]*2+
//					this->pBmpBuf[(i+1) * this->lineByte + j+1]*1-	
//					this->pBmpBuf[(i-1) * this->lineByte + j-1]*1-
//					this->pBmpBuf[(i-1) * this->lineByte + j]*2-
//					this->pBmpBuf[(i-1) * this->lineByte + j+1]*1);
//				//取绝对值
//				if(temp1<0)
//				{
//					temp1 = 0-temp1;
//				}
//				temp2 = (this->pBmpBuf[(i-1) * this->lineByte + j+1]*1+
//					this->pBmpBuf[i * this->lineByte + j+1]*2+
//					this->pBmpBuf[(i+1) * this->lineByte + j+1]*1-
//					this->pBmpBuf[(i-1) * this->lineByte + j-1]*1-
//					this->pBmpBuf[i * this->lineByte + j-1]*2-
//					this->pBmpBuf[(i+1) * this->lineByte + j-1]*1);
//
//				//取绝对值
//				if(temp2<0)
//				{
//					temp2 = 0-temp2;
//				}
//                if(temp1+temp2>150)
//					this->pBmpBuf[i * this->lineByte + j] = 0;
//				else 
//					this->pBmpBuf[i * this->lineByte + j] = 255;
//				//this->pBmpBuf[i * this->lineByte + j] =temp1+temp2;	
//			}
//		}
//	/*
//			temp1 = this->pBmpBuf[0];
//			temp2 = this->pBmpBuf[0];
//
//			//处理大于255和小于0的情况
//			for(int i = 0; i < this->bmpHeight-1; i ++)
//			{
//				for(int j = 0; j < this->bmpWidth-1;j ++)
//				{
//					if(temp1>this->pBmpBuf[i * this->lineByte + j])
//					{
//						 temp1 = this->pBmpBuf[i * this->lineByte + j];
//					}
//			
//					if(temp2<this->pBmpBuf[i * this->lineByte + j])
//					{
//						temp2 = this->pBmpBuf[i * this->lineByte + j];
//					}
//				}
//		   }
//
//			if(temp1<0)
//			{
//			   temp2 = temp2-temp1;
//			}
//			if(temp2>255)
//			{
//				for(int i = 0; i < this->bmpHeight-1; i ++)
//				{
//					for(int j = 0; j < this->bmpWidth-1;j ++)
//					{
//						this->pBmpBuf[i * this->lineByte + j]=
//							255*this->pBmpBuf[i * this->lineByte + j]/temp2;
//					}
//				}		
//			}
//	*/	
//	}
//	else if(this->biBitCount ==24)
//	{
//		int temp1,temp2;//临时变量
//         this->BMPToHSI();
//		for(int i = 1; i < this->bmpHeight-1; i ++)
//		{
//			for(int j = 1; j < this->bmpWidth-1;j ++)
//			{
//				temp1 = (this->Intensity[(i+1) * this->bmpWidth + j-1]*1+
//					this->Intensity[(i+1) * this->bmpWidth + j]*2+
//					this->Intensity[(i+1) * this->bmpWidth + j+1]*1-	
//					this->Intensity[(i-1) * this->bmpWidth + j-1]*1-
//					this->Intensity[(i-1) * this->bmpWidth + j]*2-
//					this->Intensity[(i-1) * this->bmpWidth + j+1]*1);
//				//取绝对值
//				if(temp1<0)
//				{
//					temp1 = 0-temp1;
//				}
//				temp2 = (this->Intensity[(i-1) * this->bmpWidth + j+1]*1+
//					this->Intensity[i * this->bmpWidth + j+1]*2+
//					this->Intensity[(i+1) * this->bmpWidth + j+1]*1-
//					this->Intensity[(i-1) * this->bmpWidth + j-1]*1-
//					this->Intensity[i * this->bmpWidth + j-1]*2-
//					this->Intensity[(i+1) * this->bmpWidth + j-1]*1);
//
//				//取绝对值
//				if(temp2<0)
//				{
//					temp2 = 0-temp2;
//				}
//
//				this->Intensity[i * this->bmpWidth + j] =temp1+temp2;	
//			}
//		}
//		this->HSIToBMP();
//		/*
//		temp1 = this->pBmpBuf[0];
//		temp2 = this->pBmpBuf[0];
//
//		//处理大于255和小于0的情况
//		for(int i = 0; i < this->bmpHeight-1; i ++)
//		{
//		for(int j = 0; j < this->bmpWidth-1;j ++)
//		{
//		if(temp1>this->pBmpBuf[i * this->lineByte + j])
//		{
//		temp1 = this->pBmpBuf[i * this->lineByte + j];
//		}
//
//		if(temp2<this->pBmpBuf[i * this->lineByte + j])
//		{
//		temp2 = this->pBmpBuf[i * this->lineByte + j];
//		}
//		}
//		}
//
//		if(temp1<0)
//		{
//		temp2 = temp2-temp1;
//		}
//		if(temp2>255)
//		{
//		for(int i = 0; i < this->bmpHeight-1; i ++)
//		{
//		for(int j = 0; j < this->bmpWidth-1;j ++)
//		{
//		this->pBmpBuf[i * this->lineByte + j]=
//		255*this->pBmpBuf[i * this->lineByte + j]/temp2;
//		}
//		}		
//		}
//		*/	
//	}
//    return 1;
//}
//
////二阶微分 0,1，0,1，-4,1,0,1,1
//bool HandleBMP::secondOrderDifferential()
//{
//	if(this->pBmpBuf ==NULL)
//	{
//		AfxMessageBox(_T("未读入数据!"),MB_OK);
//		return 0;
//	}
//	if(this->biBitCount==8)
//	{
//		for(int i = 1; i < this->bmpHeight-1; i ++)
//		{
//			for(int j = 1; j < this->bmpWidth-1;j ++)
//			{
//				this->pBmpBuf[i * this->lineByte + j] =
//					(this->pBmpBuf[(i-1) * this->lineByte + j-1]*0+
//					this->pBmpBuf[(i-1) * this->lineByte + j]*1+
//					this->pBmpBuf[(i-1) * this->lineByte + j+1]*0+
//					this->pBmpBuf[i * this->lineByte + j-1]*1-
//					this->pBmpBuf[i * this->lineByte + j]*4+
//					this->pBmpBuf[i * this->lineByte + j+1]*1+
//					this->pBmpBuf[(i+1) * this->lineByte + j-1]*0+
//					this->pBmpBuf[(i+1) * this->lineByte + j]*1+
//					this->pBmpBuf[(i+1) * this->lineByte + j+1]*0);	
//				if(this->pBmpBuf[i * this->lineByte + j] >255)
//				{
//					this->pBmpBuf[i * this->lineByte + j] =255;
//				}
//				else if(this->pBmpBuf[i * this->lineByte + j] < 0)
//				{
//					this->pBmpBuf[i * this->lineByte + j] =0;
//				}
//			}
//		}
//	}
//	else if(this->biBitCount == 24)
//	{
//		this->BMPToHSI();
//		for(int i = 1; i < this->bmpHeight-1; i ++)
//		{
//			for(int j = 1; j < this->bmpWidth-1;j ++)
//			{
//				this->Intensity[i * this->bmpWidth + j] =
//					(this->Intensity[(i-1) * this->bmpWidth + j-1]*0+
//					this->Intensity[(i-1) * this->bmpWidth + j]*1+
//					this->Intensity[(i-1) * this->bmpWidth + j+1]*0+
//					this->Intensity[i * this->bmpWidth + j-1]*1-
//					this->Intensity[i * this->bmpWidth + j]*4+
//					this->Intensity[i * this->bmpWidth + j+1]*1+
//					this->Intensity[(i+1) * this->bmpWidth + j-1]*0+
//					this->Intensity[(i+1) * this->bmpWidth + j]*1+
//					this->Intensity[(i+1) * this->bmpWidth + j+1]*0);	
//				if(this->Intensity[i * this->bmpWidth + j] >255)
//				{
//					this->Intensity[i * this->bmpWidth + j] =255;
//				}
//				else if(this->Intensity[i * this->bmpWidth + j] < 0)
//				{
//					this->Intensity[i * this->bmpWidth + j] =0;
//				}
//			}
//		}
//		this->HSIToBMP();
//	}
//    return 1;
//}
//
////傅里叶正变换
//bool HandleBMP::FourierTrans(IplImage *src, IplImage *dst)
//{   
//	IplImage * image_Re = cvCreateImage(cvGetSize(src), IPL_DEPTH_64F, 1);  //实部
//	IplImage * image_Im = cvCreateImage(cvGetSize(src), IPL_DEPTH_64F, 1);  //虚部
//
//	IplImage * Fourier = cvCreateImage(cvGetSize(src), IPL_DEPTH_64F, 2);
//	//实部变换
//	cvConvertScale(src, image_Re, 1, 0);
//	//虚部变换
//	cvZero(image_Im);
//	//合并
//	cvMerge(image_Re, image_Im, 0, 0, Fourier);
//
//	cvDFT(Fourier, dst, CV_DXT_FORWARD);
//	cvReleaseImage(&image_Re);
//	cvReleaseImage(&image_Im);
//	cvReleaseImage(&Fourier);
//
//	return 1;
//}
//
//bool HandleBMP::FourierTransShift(IplImage *src, IplImage *dst)
//{
//	double scale, shift, tmp1and3, tmp2and4;
//
//	IplImage* image_Re = cvCreateImage(cvGetSize(src), IPL_DEPTH_64F, 1);
//	IplImage* image_Im = cvCreateImage(cvGetSize(src), IPL_DEPTH_64F, 1);
//	cvSplit( src, image_Re, image_Im, 0, 0 );
//
//	//计算傅里叶谱
//	cvPow( image_Re, image_Re, 2.0);
//	cvPow( image_Im, image_Im, 2.0);
//	cvAdd( image_Re, image_Im, image_Re);
//	cvPow( image_Re, image_Re, 0.5 );
//
//	//对数变换以增强灰度级细节
//	cvAddS( image_Re, cvScalar(1.0), image_Re ); // 1 + Mag
//	cvLog( image_Re, image_Re ); // log(1 + Mag)
//
//	int nRow = src->height;
//	int nCol = src->width;
//	int cy = nRow/2;
//	int cx = nCol/2;
//
//	//中心变换
//	for(int j = 0; j < cy; j++ )
//	{
//		for(int i = 0; i < cx; i++ )
//		{
//			//中心化，将整体份成四块进行对角交换
//			tmp1and3 = CV_IMAGE_ELEM( image_Re, double, j, i);
//			CV_IMAGE_ELEM( image_Re, double, j, i) = CV_IMAGE_ELEM(image_Re, double, j+cy, i+cx);
//			CV_IMAGE_ELEM( image_Re, double, j+cy, i+cx) = tmp1and3;
//
//			tmp2and4 = CV_IMAGE_ELEM( image_Re, double, j, i+cx);
//			CV_IMAGE_ELEM( image_Re, double, j, i+cx) =	CV_IMAGE_ELEM( image_Re, double, j+cy, i);
//			CV_IMAGE_ELEM( image_Re, double, j+cy, i) = tmp2and4;
//		}
//	}
//
//	double minVal = 0, maxVal = 0;
//
//	cvMinMaxLoc( image_Re, &minVal, &maxVal );
//
//	scale = 255/(maxVal - minVal);
//	shift = -minVal * scale;
//
//	cvConvertScale(image_Re, dst, scale, shift);
//	cvReleaseImage(&image_Re);
//	cvReleaseImage(&image_Im);
//    
//	return 1;
//}
//
//
//bool HandleBMP::Fourier()
//{
//	double m,M;
//	double scale;
//	double shift;
//
//	//加载源图像，第二个参数表示将输入的图片转为单信道 
//	IplImage* src = cvLoadImage(this->filePathName,0);   
//	if (src==NULL)
//	{
//		return false;
//	}
//
//	//傅里叶系数
//	IplImage* Fourier = cvCreateImage(cvGetSize(src),IPL_DEPTH_64F,2);
//
//	IplImage* dst = cvCreateImage(cvGetSize(src),IPL_DEPTH_64F,2);
//
//	IplImage* ImageRe = cvCreateImage(cvGetSize(src),IPL_DEPTH_64F,1);
//	IplImage* ImageIm = cvCreateImage(cvGetSize(src),IPL_DEPTH_64F,1);
//
//	IplImage* Image = cvCreateImage(cvGetSize(src),src->depth,src->nChannels);
//	IplImage* ImageDst = cvCreateImage(cvGetSize(src),src->depth,src->nChannels);
//
//	//傅里叶变换
//	FourierTrans(src,Fourier); 
//	//中心化                
//	FourierTransShift(Fourier, Image);          
//
//	//实现傅里叶逆变换，并对结果进行缩放
//	cvDFT(Fourier,dst,CV_DXT_INV_SCALE);
//	cvSplit(dst,ImageRe,ImageIm,0,0);
//
//	//对数组每个元素平方并存储在第二个参数中
//	cvPow(ImageRe,ImageRe,2);               
//	cvPow(ImageIm,ImageIm,2);
//	cvAdd(ImageRe,ImageIm,ImageRe,NULL);
//	cvPow(ImageRe,ImageRe,0.5);
//	cvMinMaxLoc(ImageRe,&m,&M,NULL,NULL);
//	scale = 255/(M - m);
//	shift = -m * scale;
//	//将shift加在ImageRe各元素按比例缩放的结果上，存储为ImageDst
//	cvConvertScale(ImageRe,ImageDst,scale,shift);
//
//	cvNamedWindow("傅里叶变换",0);
//	cvShowImage("傅里叶变换",Image);
//	cvNamedWindow("傅里叶逆变换",0);
//	cvShowImage("傅里叶逆变换",ImageDst);
//
//	//释放图像
//	cvReleaseImage(&src);
//	cvReleaseImage(&Image);
//	cvReleaseImage(&ImageIm);
//	cvReleaseImage(&ImageRe);
//	cvReleaseImage(&Fourier);
//	cvReleaseImage(&dst);
//	cvReleaseImage(&ImageDst);
//	return true;
//}