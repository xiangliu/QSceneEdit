#ifndef _LOADBMP_H
#define _LOADBMP_H

/************************************************************************/
/* it's used by 3D feature extract                                                                     */
/************************************************************************/

#include "Windows.h" 
#include <math.h>   
#include "cv.h"
#include "cxcore.h"
#include "highgui.h"
//#include "windowsx.h"



class HandleBMP
{
public:
	unsigned char *pBmpBuf;//读入图像数据的指针
	int bmpWidth;//图像的宽
	int bmpHeight;//图像的高
	RGBQUAD *pColorTable;//颜色表指针
	int biBitCount;//图像类型，每像素位数
	int lineByte;

    double* Hue;
	double* Saturation;
	double* Intensity;//转换成HSI的数据

	//CString filePathName;
	//CString saveFilePathName;
	char *filePathName;
	char *saveFilePathName;

	HandleBMP(); //构造函数
	bool readBMP(char* filePath);//读取BMP图像
   // bool presentBMP(CDC *pDC,CPoint leftTop);
	bool saveBMP(char *bmpName, unsigned char *imgBuf, int width, int height, 
		int biBitCount, RGBQUAD *pColorTable);
	//bool drawBitmapFormFile(char*   BitmapFilePath,CDC   *pDC,CRect   rect) ;
	bool TurnGrey();//将图像变灰
	//将BMP图像转换成HSI
	bool BMPToHSI();
	bool HSIToBMP();//将HSI图像转换成BMP
	bool zhiFangTuStren();//直方图增强
	bool linearSmoothing(int size);//线性平滑，暂定3*3模板；设参数size为模板大小
	bool UNLinearSmoothing();//非线性平滑，实现的是中值滤波器
	bool firstOrderDifferential();//一阶微分算子
	bool secondOrderDifferential();//二阶微分算子
	bool FourierTrans(IplImage *src, IplImage *dst);
	bool FourierTransShift(IplImage *src, IplImage *dst);
    bool Fourier();//傅里叶变换
//private:
};
#endif