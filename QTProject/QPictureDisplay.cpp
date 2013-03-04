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

	//Iselected = NULL;
	Iselected=new int[256];     
	memset(Iselected,0,256*4);
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

	 delete []Iselected;
	 Iselected = NULL;
	
	//segmented image
	if(hState == segmented)
	{
		for (int i=0;i<ImageHeight;i++)
		{
			delete[] labels[i];
			labels[i]= NULL;
		}
		delete[] labels;
		labels = NULL;
	}

	//release the picked object
	if(hState == objcetPicked)
	{
		//for (int i=0;i<objects.size();i++)
		//	delete objects[i];
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
		//ֻ�Ǹ�ԭʼͼƬ����ÿ�����ر�ע���Ǹ����������Ŀ�
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
		//��Щ����Ӧ�÷ŵ�����������ɣ�����ֻ�Ǳ�־����pickup״̬����
	   //memset(Iselected,0,256*4);
	   //upimg.Destroy();
	   //upimg.CopyOf(segimg);
	   //GenObj();
	   //GenGrayImage();
	   hState = objcetPicked;
	   connect(this,SIGNAL(PickupObject(QImage *)),segPictureDisplayWidget,SLOT(PickupObject(QImage *)));
	}
	//else
	//{
	//	hState = objectRePicked;

	//}

}

//��meanshift�ָ��ԭʼ��Ƭÿ�����ض��лᱻ����һ������
//���˺�����������Ļ���֮�ϣ����ԭʼ��Ƭ���ɷָ�����Ƭ
void QPictureDisplay::GenImage()
{
	// Ϊ��������region���췽��
	//int *colors=new int[256];   // ����ֱ�ӵ����ֲ�����
	int colors[256];

	CvRNG rng= cvRNG(cvGetTickCount());
	for(int i=0;i<regionCount;i++)
	{
		colors[i] = cvRandInt(&rng);
	}

	//Mat ��opencv ��һ��nά��matrix������������ʾa matrix, image, optical flow map, 3-focal tensor etc.
	IplImage *Iplorgimg=orgimg.GetImage();
	cv::Mat org(Iplorgimg,0); // 0������data,���Բ����ᴴ���µ�data
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

		//org��ԭʼ��Ƭ����res�Ƿָ��ÿ��������ɫһ�µ���Ƭ��seg�ǽ����Ƭ
		//seg = org*0.5 + res*0.5 + 0 ;
		cv::addWeighted(org,0.5,res,0.5,0,seg);
		upimg.Destroy();
		upimg.CopyOf(segimg);
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
//���������Ĺ���Ϊ���ĳ�����ض�Ӧ������ѡ�У��������upimg�򱻻ָ���ԭʼͼƬ
void QPictureDisplay::UpdateRes()
{
	// �ٶȹ���������ѡ����opencv��iplimage�������ת��
	int Theight = SrcImage->height();
	int Twidth = SrcImage->width();

	//�˴�ȷ��ÿ��ֻ��ʾ��ǰ�����object���ϴε����������˾Ͳ�����ʾ��Ӧ������
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
	//��obj �� grayimg�ڴ˴�δ����ռ�����Ϊ���ǵĴ�С���̶�
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
			    //���ڶ���Ƭ���˾��д������Դ˴�����button down�ĵ���Ҫ������
				UpdateSelected(buttonDown.x()-ImageRect.topLeft().x(),buttonDown.y()-ImageRect.topLeft().y()); //��¼��ѡ������
				UpdateRes();     //�������±��ָ�ԭʼͼ�񣨼���ʾ����������ݣ�
				GenObj();        //��������pickup object
				GenGrayImage();  //�������ɻҶ�ͼ��
				emit PickupObject(IplImage2QImage(grayimg.GetImage()));
				this->update();
		}
	}	
}

void QPictureDisplay::wheelEvent(QWheelEvent *event)
{
}

void QPictureDisplay::GetSurroudRect(QPoint &A, QPoint &B)
{
	A.setX(grayimg.Width()+1);
	A.setY(grayimg.Height()+1);
	B.setX(-1);
	B.setY(-1);

	IplImage *Iplgrayimg=grayimg.GetImage();
	int h = grayimg.Height();
	int w = grayimg.Width();

	for (int i=0;i< h ;i++)
		for (int j=0;j< w;j++)
		{
			if ((Iplgrayimg->imageData+i*Iplgrayimg->widthStep)[j*Iplgrayimg->nChannels]==0)
			{
				// max x,y
				B.setX(j > B.x()? j: B.x() );
				B.setY(i > B.y()? i: B.y() );
				// min x,y
				A.setX(j < A.x()? j: A.x() );
				A.setY(i < A.y()? i: A.y() );
			}
		}	
}

//slot
//���ڴ����mainwindow��������signal�����汻�ָ�õ�����
void QPictureDisplay::SaveSegObject(QString tag, int weight)
{
	if(hState != objcetPicked)
	{
		//QMessgbox
		return;
	}
	QPoint A, B;
	this->GetSurroudRect(A,B);

	CSegObject *object=new CSegObject;
	object->SetGrayImg(grayimg);
	object->SetObjImg(obj);
	//object->filename= filename;

	object->upLeft = A;
	object->downRight = B;

	//********���ָ���������ļ�
	/*
	string str;
	strstream stream;
	stream<<"SearchSource\\"<<object->filename<<"_"<<this->ObjectPos<<"_black"<<".bmp";
	stream>>str;
	stream.clear();
	grayimg.Save(str.c_str()); // ��ڰ�

	stream<<"SearchSource\\"<<object->filename<<"_"<<this->ObjectPos<<"_rgb"<<".bmp";
	stream>>str;
	stream.clear();
	obj.Save(str.c_str()); // ��rgb
	*/
	//********************************

	//object->tag= tag.toStdString() ;
	object->tag = tag;
	objects.push_back(object);
	objectImportance.push_back(weight); //�����������Ҫ�̶�
	//this->ObjectPos++;

	//Ϊ�´�pickup��׼������մ˴�ѡ������
	memset(Iselected,0,256*4);
}

//������mainwindow�������б����object
//void QPictureDisplay::SaveSegObjectList(int &objectNum)
//{
//	objectNum = objects.size();
//	//if(objectNum != 0)
//	//{
//	//	objectList = new CSegObject*[objectNum];
//	//	if(objectList != NULL)
//	//	{
//	//		for(int i = 0; i<objectNum ;i++)
//	//		{
//	//			objectList[i] = objects[i];
//	//		}
//	//	}
//	//}
//}