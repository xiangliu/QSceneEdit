#ifndef GLOBAL_H
#define GLOBAL_H

#include <QImage>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/imgproc_c.h"

//此函数开辟了内存，注意释放
IplImage *QImage2IplImage(QImage *src);

//此函数同样开辟了内存，注意释放
QImage *IplImage2QImage(IplImage *src) ;



#endif