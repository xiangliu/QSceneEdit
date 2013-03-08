#ifndef GLOBAL_H
#define GLOBAL_H

//原有的一些stdafx.h 中的数据也被保存在该文件中，包括一些全局的define

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

//用来定义检索后保留的最相似的3D模型的个数
#define MODELSEARCHRESULTNUMBER 18

//用来定义检索后保留的最相似的场景个数
#define SCENEFORDISPLAY 30

#define BUFSIZE 512

#define SIM_SQUARE(x) ((x)*(x))

//定义一个场景最大的分割数
#define MAXSEGMENTNUMBER 50

//****************************added by liuxiang

//定义场景中物体之间relationship的种类： 0--默认无关系，
//--group关系: center-element 1; element-center -1
//--水平支持（support），支撑 5 被支撑 -5 ； onwall（钟）2 -2 ；地板 4 -4 ，天花板3 -3
//-- 同一关系（identical） 6
// --竖直接触（vContact） 7
//--包围（enclosure）8 -8

enum Relationship {nonRelation, group, support, identical, contact, enclosured };

//此函数开辟了内存，注意释放
IplImage *QImage2IplImage(QImage *src);

//此函数同样开辟了内存，注意释放
QImage *IplImage2QImage(IplImage *src) ;


#endif