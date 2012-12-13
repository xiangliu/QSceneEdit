#pragma once

#ifndef SEGOBJECT_CALSS_DEF
#define SEGOBJECT_CALSS_DEF

#include <QPoint>
#include "CvvImage.h"
#include <string>

class CSegObject
{
public:
	CSegObject(void);
	~CSegObject(void);

	CvvImage grayimg; // �ڰ�ͼ
	CvvImage objimg; // ��ɫͼ����������������
	void SetGrayImg(CvvImage& img);
	void SetObjImg(CvvImage& img);
	//char* tag;
	std::string tag;
	std::string filename;
	double x;
	double y;
	double z;

	//������¼���ָ�������ԭ��Ƭ�е�λ���Լ���С��Ϣ
    QPoint upLeft;
    QPoint downRight;

    //CPoint upLeft;   //CPoint is supported by MFC,not by QT
    //CPoint downRight;

};

#endif

