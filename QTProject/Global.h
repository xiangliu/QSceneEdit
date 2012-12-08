#ifndef GLOBAL_H
#define GLOBAL_H

#include <QImage>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/imgproc_c.h"

//�˺����������ڴ棬ע���ͷ�
IplImage *QImage2IplImage(QImage *src);

//�˺���ͬ���������ڴ棬ע���ͷ�
QImage *IplImage2QImage(IplImage *src) ;



#endif