#include "QPictureDisplay.h"
//#include "ui_QPictureDisplay.h"

QPictureDisplay::QPictureDisplay(QWidget *parent) :
    QWidget(parent)
  //,
   // ui(new Ui::QPictureDisplay)
{
    //ui->setupUi(this);
	hState = emputy;
	SrcImage = NULL;
	sImage = NULL;

	colors = NULL; 
	Iselected = NULL;

	ClearPickupObject = false;

}

QPictureDisplay::~QPictureDisplay()
{
	//release the memory of image
	if (sImage!=NULL)
	{
		cvReleaseImage(&sImage);
		orgimg.Destroy();
		segimg.Destroy();
		resimg.Destroy();
		upimg.Destroy();
	}

	//�˴��Ͳ����ͷ��ˣ���Ϊmainwindow�Ѿ��ͷ�
	//if(SrcImage != NULL)
	//{
	//	delete SrcImage;
	//}

	if(Iselected !=NULL)
	{
	   delete []Iselected;
	}

	if(colors !=NULL)
	{
		delete[] colors;
	}

	//segmented image
	if(hState == segmented)
	{
		for (int i=0;i<ImageHeight;i++)
		{
			delete[] labels[i];
		}
		delete[] labels;
		
	}

	//release the picked object
	if(hState == objcetPicked)
	{
		for (int i=0;i<objects.size();i++)
			delete objects[i];
	}
	
}

//��mainwindow��ȡָ��
void QPictureDisplay::GetMainwMesg(QSegPictureDisplay *segPicDispWidget)
{
	segPictureDisplayWidget = segPicDispWidget;
}

void QPictureDisplay::SegImageAction()
{
	//ֻ����load image֮����ܽ���segmentation�׶�
	if(hState == loaded)
	{
		labels=new int *[sImage->height];
		for (int i=0;i<sImage->height;i++)
		{
			labels[i]=new int[sImage->width];
		}
		regionCount=MeanShift(sImage,labels);
		GenImage();
		hState = segmented;

		//����ò�ƻ��ý��ָ�Ľ����ʾ����
		this->update();
	}
}

//pick up the segmented object
void QPictureDisplay::PickImageAction()
{
	//��Ϊֻ�е�ǰһ�׶μ����ָ���ͼƬ���ܽ��뵽��һ�׶Σ���pickup object
	if(hState == segmented)
	{
	   hState = objcetPicked;
	   connect(this,SIGNAL(PickupObject(QImage *)),segPictureDisplayWidget,SLOT(PickupObject(QImage *)));
	}

}

void QPictureDisplay::GenImage()
{
	// Ϊ��������region���췽��
	colors=new int[256];   // ����ֱ�ӵ����ֲ�����
	Iselected=new int[256];
	memset(Iselected,0,256*4);

	CvRNG rng= cvRNG(cvGetTickCount());
	for(int i=0;i<regionCount;i++)
	{
		colors[i] = cvRandInt(&rng);
	}

	IplImage *Iplorgimg=orgimg.GetImage();
	cv::Mat org(Iplorgimg,0); // 0��������data,���Բ����ᴴ���µ�data
	IplImage *Iplresimg=resimg.GetImage();
	cv::Mat res(Iplresimg,0);
	IplImage *Iplsegimg=segimg.GetImage();
	cv::Mat seg(Iplsegimg,0);

	for (int i=0;i<SrcImage->height();i++)
		for (int j=0;j<SrcImage->width();j++)
		{
			int cl=labels[i][j];

			(Iplresimg->imageData+i*Iplresimg->widthStep)[j*Iplresimg->nChannels+2]=(colors[cl]>>16)&255;
			(Iplresimg->imageData+i*Iplresimg->widthStep)[j*Iplresimg->nChannels+1]=(colors[cl]>>8)&255;
			(Iplresimg->imageData+i*Iplresimg->widthStep)[j*Iplresimg->nChannels+0]=(colors[cl])&255;
		}

		cv::addWeighted(org,0.5,res,0.5,0,seg);
		upimg.Destroy();
		upimg.CopyOf(segimg);

		//state=2;
}

void QPictureDisplay::GenGrayImage()
{
	int Iheight = SrcImage->height();
	int Iwidth = SrcImage->width();
	grayimg.Destroy();
	grayimg.Create(Iwidth,Iheight,24);
	IplImage *Iplgrayimg=grayimg.GetImage();
	
	for (int i=0;i<Iheight;i++)
		for (int j=0;j<Iwidth;j++)
		{
			int label=labels[i][j];
			if(Iselected[label])
			{
				(Iplgrayimg->imageData+i*Iplgrayimg->widthStep)[j*Iplgrayimg->nChannels+2]=0;
				(Iplgrayimg->imageData+i*Iplgrayimg->widthStep)[j*Iplgrayimg->nChannels+1]=0;
				(Iplgrayimg->imageData+i*Iplgrayimg->widthStep)[j*Iplgrayimg->nChannels+0]=0;
			}
			else
			{
				(Iplgrayimg->imageData+i*Iplgrayimg->widthStep)[j*Iplgrayimg->nChannels+2]=255;
				(Iplgrayimg->imageData+i*Iplgrayimg->widthStep)[j*Iplgrayimg->nChannels+1]=255;
				(Iplgrayimg->imageData+i*Iplgrayimg->widthStep)[j*Iplgrayimg->nChannels+0]=255;
			}
		}
}

//��������pickup object of segmented image
void QPictureDisplay::UpdateSelected(int x,int y)
{
	int label=labels[y][x];
	if (Iselected[label]==0)
		Iselected[label]=1;
	else
		Iselected[label]=0;
}

// �������²�����ͼƬUpImg
// SegImg�������������ԭʼͼ+�ֲ�
// UpImg�������¼���Ӧ����ʾ��ͼƬ
void QPictureDisplay::UpdateRes()
{
	// �ٶȹ���������ѡ����opencv��iplimage��������ת��
	int Theight = SrcImage->height();
	int Twidth = SrcImage->width();
	upimg.Destroy();
	upimg.CopyOf(segimg);
	for (int i=0;i<Theight;i++)
		for (int j=0;j<Twidth;j++)
		{
			int label=labels[i][j];
			if(Iselected[label])
			{
				IplImage *Iplupimg=upimg.GetImage();
				IplImage *Iplorgimg=orgimg.GetImage();

				double sr=(Iplorgimg->imageData+i*Iplorgimg->widthStep)[j*Iplorgimg->nChannels+2];
				double sg=(Iplorgimg->imageData+i*Iplorgimg->widthStep)[j*Iplorgimg->nChannels+1];
				double sb=(Iplorgimg->imageData+i*Iplorgimg->widthStep)[j*Iplorgimg->nChannels+0];

				(Iplupimg->imageData+i*Iplupimg->widthStep)[j*Iplupimg->nChannels+2]=(uchar)sr;
				(Iplupimg->imageData+i*Iplupimg->widthStep)[j*Iplupimg->nChannels+1]=(uchar)sg;
				(Iplupimg->imageData+i*Iplupimg->widthStep)[j*Iplupimg->nChannels+0]=(uchar)sb;
			}
		}
}

void QPictureDisplay::GenObj()
{
	int tHeight = SrcImage->height();
	int tWidth = SrcImage->width();
	obj.Destroy();
	obj.Create(tWidth,tHeight,24);
	for (int i=0;i<tHeight;i++)
		for (int j=0;j<tWidth;j++)
		{
			int label=labels[i][j];
			if(Iselected[label])
			{
				IplImage *Iplorgimg=orgimg.GetImage();
				double sr=(Iplorgimg->imageData+i*Iplorgimg->widthStep)[j*Iplorgimg->nChannels+2];
				double sg=(Iplorgimg->imageData+i*Iplorgimg->widthStep)[j*Iplorgimg->nChannels+1];
				double sb=(Iplorgimg->imageData+i*Iplorgimg->widthStep)[j*Iplorgimg->nChannels+0];

				IplImage *Iplobjimg=obj.GetImage();
				(Iplobjimg->imageData+i*Iplobjimg->widthStep)[j*Iplobjimg->nChannels+2]=(uchar)sr;
				(Iplobjimg->imageData+i*Iplobjimg->widthStep)[j*Iplobjimg->nChannels+1]=(uchar)sg;
				(Iplobjimg->imageData+i*Iplobjimg->widthStep)[j*Iplobjimg->nChannels+0]=(uchar)sb;
			}
			else
			{
				IplImage *Iplobjimg=obj.GetImage();
				(Iplobjimg->imageData+i*Iplobjimg->widthStep)[j*Iplobjimg->nChannels+2]=255;
				(Iplobjimg->imageData+i*Iplobjimg->widthStep)[j*Iplobjimg->nChannels+1]=255;
				(Iplobjimg->imageData+i*Iplobjimg->widthStep)[j*Iplobjimg->nChannels+0]=255;
			}
		}
		//	state=3;
}

void QPictureDisplay::ResetSelectedImage()
{
	memset(Iselected,0,256*4);
	upimg.Destroy();
	upimg.CopyOf(segimg);
	this->GenObj();
	this->GenGrayImage();
	//this->UpdateAllViews(NULL);
}

void QPictureDisplay::paintEvent(QPaintEvent *e)
{
	//hanldeImage >0 represents the image has been loaded
	if(hState == loaded )
	{
		//�˴�δ������Ƭ��widget������
		int x = width()/2 - SrcImage->width()/2;
        int y = height()/2 - SrcImage->height()/2;
		QPainter painter(this);
		//*SrcImage = SrcImage->scaled(width(),height());
		//painter.drawImage(QPoint(0,0),*SrcImage);
		painter.drawImage(QPoint(x,y),*SrcImage);
	}
	else if(hState == segmented || hState == objcetPicked)
	{
		int x = width()/2 - SrcImage->width()/2;
		int y = height()/2 - SrcImage->height()/2;
		QPainter painter(this);
		IplImage *temp1 = upimg.GetImage();
		QImage *t2 = IplImage2QImage(temp1);
		painter.drawImage(QPoint(x,y),*t2);
	}
	
}

//��Ӧ�����image��Ȼ������ʾ����
void QPictureDisplay::OpenImageFile(QImage *SourceImage)
{
    SrcImage = SourceImage;  //QImage
	sImage = QImage2IplImage(SrcImage);

	//��¼load image's rect
	int x = width()/2 - SrcImage->width()/2;
	int y = height()/2 - SrcImage->height()/2;
	ImageRect.setCoords(x,y,x+SrcImage->width(),y+SrcImage->height());

	//�������Ӧ����Ƭ�������ڴ�ռ䣬��׼���ָ��ʱ��ʹ��
	if(hState != emputy)
	{
		orgimg.Destroy();
		segimg.Destroy();
		resimg.Destroy();
		upimg.Destroy();
	}
	orgimg.CopyOf(sImage);
	segimg.CopyOf(sImage); //��������ý����ǿ����ڴ�ռ�
	resimg.CopyOf(sImage);
	upimg.CopyOf(sImage);

	hState = loaded;
    //repaint to show the image
	this->update();
}

////change QImage to IplImage���˴��������ڴ�ռ�
//IplImage *QPictureDisplay::QImage2IplImage(QImage *src)
//{
//	int width = src->width();
//	int height = src->height();
//	CvSize Size;
//	Size.height = height;
//	Size.width = width;
//	IplImage *res = cvCreateImage(Size, IPL_DEPTH_8U, 3);
//	for (int y = 0; y < height; ++y)
//	{
//		for (int x = 0; x < width; ++x)
//		{
//			QRgb rgb = src->pixel(x, y);
//			cvSet2D(res, y, x, CV_RGB(qRed(rgb), qGreen(rgb), qBlue(rgb)));
//		}
//	}
//	return res;
//}

////change IplImage to QImage 
//QImage *QPictureDisplay::IplImage2QImage(IplImage *src)
//{
//	QImage *res;
//	cvCvtColor(src,src,CV_BGR2RGB);
//	uchar *imgData=(uchar *)src->imageData;
//	//Constructs an image with the given width, height and format, that uses an existing memory buffer
//	res=new QImage(imgData,src->width,src->height,QImage::Format_RGB888);
//	return res;
//}

void QPictureDisplay::mouseMoveEvent(QMouseEvent *event)
{

}

void QPictureDisplay::mousePressEvent(QMouseEvent *event)
{
	//hanleImage == 3 represent the process entered into the pickup section
	if(hState == objcetPicked)
	{
		setMouseTracking(true);
		buttonDown = event->pos();
		if (event->button()==Qt::LeftButton && ImageRect.contains(buttonDown))
		{
			    //���ڶ���Ƭ���˾��д��������Դ˴�����button down�ĵ���Ҫ������
				UpdateSelected(buttonDown.x()-ImageRect.topLeft().x(),buttonDown.y()-ImageRect.topLeft().y());
				UpdateRes();
				GenObj();
				GenGrayImage();
				emit PickupObject(IplImage2QImage(grayimg.GetImage()));
				this->update();
				//UpdateAllViews(NULL);
		}
	}	
}

void QPictureDisplay::wheelEvent(QWheelEvent *event)
{
}