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
	unsigned char *pBmpBuf;//����ͼ�����ݵ�ָ��
	int bmpWidth;//ͼ��Ŀ�
	int bmpHeight;//ͼ��ĸ�
	RGBQUAD *pColorTable;//��ɫ��ָ��
	int biBitCount;//ͼ�����ͣ�ÿ����λ��
	int lineByte;

    double* Hue;
	double* Saturation;
	double* Intensity;//ת����HSI������

	//CString filePathName;
	//CString saveFilePathName;
	char *filePathName;
	char *saveFilePathName;

	HandleBMP(); //���캯��
	bool readBMP(char* filePath);//��ȡBMPͼ��
   // bool presentBMP(CDC *pDC,CPoint leftTop);
	bool saveBMP(char *bmpName, unsigned char *imgBuf, int width, int height, 
		int biBitCount, RGBQUAD *pColorTable);
	//bool drawBitmapFormFile(char*   BitmapFilePath,CDC   *pDC,CRect   rect) ;
	bool TurnGrey();//��ͼ����
	//��BMPͼ��ת����HSI
	bool BMPToHSI();
	bool HSIToBMP();//��HSIͼ��ת����BMP
	bool zhiFangTuStren();//ֱ��ͼ��ǿ
	bool linearSmoothing(int size);//����ƽ�����ݶ�3*3ģ�壻�����sizeΪģ���С
	bool UNLinearSmoothing();//������ƽ����ʵ�ֵ�����ֵ�˲���
	bool firstOrderDifferential();//һ��΢������
	bool secondOrderDifferential();//����΢������
	bool FourierTrans(IplImage *src, IplImage *dst);
	bool FourierTransShift(IplImage *src, IplImage *dst);
    bool Fourier();//����Ҷ�任
//private:
};
#endif