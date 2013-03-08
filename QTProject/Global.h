#ifndef GLOBAL_H
#define GLOBAL_H

//ԭ�е�һЩstdafx.h �е�����Ҳ�������ڸ��ļ��У�����һЩȫ�ֵ�define

#include <QImage>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/imgproc_c.h"

//****************************added by liuxiang
#define WINDOWWIDTH 700
#define WINDOWHEIGHT 700

#define SIZEIMPORTANCE 0.5
#define EDGEIMPORTANCE 0.5

//����������������������Ƶ�3Dģ�͵ĸ���
#define MODELSEARCHRESULTNUMBER 18

//����������������������Ƶĳ�������
#define SCENEFORDISPLAY 30

#define BUFSIZE 512

#define SIM_SQUARE(x) ((x)*(x))

//����һ���������ķָ���
#define MAXSEGMENTNUMBER 50

//****************************added by liuxiang

//���峡��������֮��relationship�����ࣺ 0--Ĭ���޹�ϵ��
//--group��ϵ: center-element 1; element-center -1
//--ˮƽ֧�֣�support����֧�� 5 ��֧�� -5 �� onwall���ӣ�2 -2 ���ذ� 4 -4 ���컨��3 -3
//-- ͬһ��ϵ��identical�� 6
// --��ֱ�Ӵ���vContact�� 7
//--��Χ��enclosure��8 -8

enum Relationship {nonRelation, group, support, identical, contact, enclosured };

//�˺����������ڴ棬ע���ͷ�
IplImage *QImage2IplImage(QImage *src);

//�˺���ͬ���������ڴ棬ע���ͷ�
QImage *IplImage2QImage(IplImage *src) ;


#endif