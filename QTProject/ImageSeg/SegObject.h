#ifndef SEGOBJECT_CALSS_DEF
#define SEGOBJECT_CALSS_DEF

/************************************************************************/
/*2012/12/17
将tag从string改成 QString，为了方便处理，by liuxiang
*/
/************************************************************************/

#include <QPoint>
#include <QString>
#include "CvvImage.h"
#include <string>

class CSegObject
{
public:
	CSegObject(void);
	~CSegObject(void);

	CvvImage grayimg; // 黑白图
	CvvImage objimg; // 彩色图，用来进行纹理处理
	void SetGrayImg(CvvImage& img);
	void SetObjImg(CvvImage& img);
	//char* tag;
	//std::string tag;
	QString tag;
	std::string filename;
	double x;
	double y;
	double z;

	//用来记录被分割物体在原照片中的位置以及大小信息
    QPoint upLeft;
    QPoint downRight;

    //CPoint upLeft;   //CPoint is supported by MFC,not by QT
    //CPoint downRight;

};

#endif

