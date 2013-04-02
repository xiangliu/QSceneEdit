#include "QSceneDisplay.h"
#include <QMessageBox>
#include <fstream>
#include <iostream>
#include <algorithm>
using namespace std;

#define BUFFER_LENGTH 64
const float PI=3.1415926;
const float speed=PI/180;

QSceneDisplay::QSceneDisplay(QWidget *parent)
	: QGLWidget(parent)
{
	xangle=yangle=0.0;
	scale=1.0;
	scene=NULL;
	up[0]=up[2]=0;
	up[1]=1;
	sceneDisplayState = PrepareState; // ƽ������
	selectModel=-1;
	glProjectionM=new GLdouble[16];
	glModelM=new GLdouble[16];
	glViewM=new int[4];

	//���ڱ��浥��ģ�ͼ������
	selected3DModel = -1;
	this->pObjectMatchResult = new ObjectMatRes[MODELSEARCHRESULTNUMBER];
}

QSceneDisplay::~QSceneDisplay()
{
	delete []glProjectionM;
	delete []glModelM;
	delete []glViewM;
}

void QSceneDisplay::initializeGL()
{
	glShadeModel(GL_SMOOTH);

	glClearColor(1.0,1.0,1.0,1.0);
	glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
}

void QSceneDisplay::paintGL()
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(1,1,1,1);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	//glShadeModel(GL_FLAT);
	glShadeModel(GL_SMOOTH);
	GLfloat light_ambient[] = { 1.0, 1.0, 1.0, 1.0 };//��������ɫ
	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };//���������ɫ
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };//���淴˼����ɫ
	GLfloat light_position[] = { 9999.0, 9999.0, 9999.0, 0.0 };//��Դλ��
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);//���û�����
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);//�����������
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);//���þ��淴���
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);//���ù�Դλ��
	glEnable(GL_LIGHTING);//�򿪹���
	glEnable(GL_LIGHT0);//��0�Ź�Դ
	glEnable(GL_DEPTH_TEST);//����Ȳ���

	glDepthFunc(GL_LEQUAL);							// ������Ȳ��Ե�����
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);			// ������ϸ��͸������

	glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(plane[0],plane[1],plane[2],plane[3],-10000,10000);

	if(scene==NULL)
		return;
	gluLookAt(eye[0],eye[1],eye[2],scene->bsphere.center[0],scene->bsphere.center[1],scene->bsphere.center[2],up[0],up[1],up[2]);

	DrawScene();
}

void QSceneDisplay::resizeGL( int width,int height )
{
	if (height==0)
		height=1;
	glViewport(0,0,width,height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(plane[0],plane[1],plane[2],plane[3],-10000,10000);

	glMatrixMode(GL_MODELVIEW);
}

void QSceneDisplay::mouseMoveEvent(QMouseEvent *event)
{
	if(scene==NULL)
		return;
	QPoint point=event->pos();
	if (event->buttons() & Qt::LeftButton)
	{
		if(sceneDisplayState == PrepareState )
		{
			float dx=float(point.x()-btnDown.x())*(plane[1]-plane[0])/width(); // ת�����Ӿ����ƶ��ľ���
			float dy=float(btnDown.y()-point.y())*(plane[3]-plane[2])/height();
			plane[0]-=dx;
			plane[1]-=dx;
			plane[2]-=dy;
			plane[3]-=dy;
			//btnDown=point;
		}
		else if (sceneDisplayState== ObjectTranslation && isSelectedModelValid())
		{
			GLdouble ax,ay,az,bx,by,bz;
			int invert_y=height()-btnDown.y();
			gluUnProject(btnDown.x(),invert_y,0,glModelM,glProjectionM,glViewM,&ax,&ay,&az); // �ҵ�����ϵ�е�
			invert_y=height()-point.y();
			gluUnProject(point.x(),invert_y,0,glModelM,glProjectionM,glViewM,&bx,&by,&bz); // �ҵ�����ϵ�е�
			Model* model=scene->sceneModels[selectModel];
			model->tx+=bx-ax;
			model->ty+=by-ay;
			model->tz+=bz-az;
		}
		else if(sceneDisplayState==ObjectRotation && isSelectedModelValid())
		{
			Model* model=scene->sceneModels[selectModel];
			model->xangle+=double(point.y()-btnDown.y())/3.6;
			model->yangle+=double(point.x()-btnDown.x())/3.6;
		}
	}
	else if (event->buttons()&Qt::RightButton)
	{
		xangle+=float(point.x()-btnDown.x())*90/width();  // ��y��ת�ĽǶȣ��Ƕ�ֵ
		yangle+=float(btnDown.y()-point.y())*90/height(); // ��x����ת�Ƕȣ��Ƕ�ֵ

		if (xangle>360)
			xangle-=360;
		else if(xangle<0)
			xangle+=360;

		if (yangle>360)
		{
			yangle-=360;
			up[1]*=-1;
		}
		else if(yangle<0)
		{
			yangle+=360;
			up[1]*=-1;
		}

		float anglex=speed*xangle;
		float angley=speed*yangle;

		eye[0]=sin(angley)*cos(anglex);
		eye[1]=cos(angley);
		eye[2]=sin(angley)*sin(anglex);
		eye=normalize(eye);

		eye=scene->bsphere.center+radius*eye;

		//btnDown=point;
	}
	btnDown=point;
    this->updateGL();
}

void QSceneDisplay::mousePressEvent(QMouseEvent *event)
{
	if(scene==NULL)
		return;
	setMouseTracking(true);
	btnDown=event->pos();
	if (event->button()==Qt::LeftButton)
	{
		//����ʰȡ�׶�
		//if (sceneDisplayState==PrepareState)
		//{
			//ProcessSelection(btnDown.x(),btnDown.y());
		if( ProcessSelection(btnDown.x(),btnDown.y()))
		{
			this->sceneDisplayState = ObjectSelected; //����ʰȡ�׶�,����ʰȡ��������
		}
		//}
		////��������������model֮�������볡��֮��
		//else if(this->sceneDisplayState == SearchSingleModel)
		//{

		//}
		//else if (state==3 && isSelectedModelValid())
		//{
		//	int invert_y=height()-btnDown.y();
		//	scene->sceneModels[selectModel]->ball.arcball_tranStart(btnDown.x(),invert_y);
		//}
		//else if (state==4 && isSelectedModelValid())
		//{
		//	int invert_y=height()-btnDown.y();
		//	scene->sceneModels[selectModel]->ball.arcball_rotStart(btnDown.x(),invert_y);
		//}
	}
}

void QSceneDisplay::wheelEvent(QWheelEvent *event)
{
	if (sceneDisplayState==PrepareState)
	{
		double numDegrees = -event->delta() / 8.0;
		double numSteps = numDegrees / 15.0;
		scale = pow(1.125, numSteps);
		double width=(plane[1]-plane[0])*scale;
		double height=(plane[3]-plane[2])*scale;
		double centerx=(plane[0]+plane[1])/2;
		double centery=(plane[3]+plane[2])/2;

		plane[0]=centerx-width/2;
		plane[1]=centerx+width/2;
		plane[2]=centery-height/2;
		plane[3]=centery+height/2;
	}
	else if (sceneDisplayState > 2 && isSelectedModelValid())
	{
		double numDegrees = event->delta() / 8.0;
		double numSteps = numDegrees / 15.0;
		scene->sceneModels[selectModel]->scaled *=pow(1.125, numSteps);;
		//scene->sceneModels[selectModel]->scale+=event->delta()/100;
	}
	this->updateGL();
}

void QSceneDisplay::DrawCoodinates()
{
	glTranslatef(0,0,-6);
	glBegin(GL_LINES);
	glColor3b(0,0,255);
	glVertex3f(0,-5000,0);
	glVertex3f(0,5000,0);
	glColor3b(0,255,0);
	glVertex3f(-5000,0,0);
	glVertex3f(5000,0,0);
	glColor3b(255,0,0);
	glVertex3f(0,0,-5000);
	glVertex3f(0,0,5000);
	glEnd();
}

void QSceneDisplay::SetDisScene( Scene* scene )
{
	this->scene=scene;
	radius=scene->bsphere.r;
	// �����ӵ�λ��
	eye[0]=scene->bsphere.center[0];
	eye[1]=scene->bsphere.center[1];
	eye[2]=scene->bsphere.center[2]+radius;

	xangle=90;
	yangle=90;
	// �����Ӿ��������
	double diam=2*scene->bsphere.r;
	double tmp=abs(scene->bsphere.center[0]);
	plane[0]=-(tmp+diam);
	plane[1]=-plane[0];
	tmp=abs(scene->bsphere.center[1]);
	plane[2]=-(tmp+diam);
	plane[3]=-plane[2];
	
	double aspect=(double)width()/height();
	if (aspect<1)
	{
		plane[2]/=aspect;
		plane[3]/=aspect;
	}
	else
	{
		plane[0]*=aspect;
		plane[1]*=aspect;
	}
	updateGL();
}

void QSceneDisplay::DrawScene()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//gluLookAt(eye[0],eye[1],eye[2],scene->bsphere.center[0],scene->bsphere.center[1],scene->bsphere.center[2],up[0],up[1],up[2]);
	if(sceneDisplayState > 2 && isSelectedModelValid())
		SetProjectionModelView();

	glInitNames();
	glPushName(0);

	// �����Ƴ�����ǽ��
	int occurs = -1;
	map<string, int>::iterator it = scene->ModelMap.find("Wall");
	if(it != scene->ModelMap.end())
	{
		occurs = it ->second;
		scene->sceneModels[occurs]->visible=false;
	}

	scene->DrawScene();

	if ( selectModel<0 )
	{
		return;
	}
	//else if(selectModel > this->scene->modelSize)
	//{
	//	this->scene->DrawModelSearchBBox();
	//}
	//else
	//{
		scene->sceneModels[selectModel]->DrawBbox();
	//}
	
}

void QSceneDisplay::ChooseModelAction()
{
	sceneDisplayState = PrepareState; // ѡ��ģ��
}

int QSceneDisplay::ProcessSelection( int xPos,int yPos )
{
	int returnValue = 0; //���ڶ��巵��ֵ��0��ʾδʰȡ�����壬1��ʾʰȡ������
	GLfloat aspect;
	GLuint selectBuff[BUFFER_LENGTH];
	GLint hits,viewport[4];

	glSelectBuffer(BUFFER_LENGTH,selectBuff);
	glGetIntegerv(GL_VIEWPORT,viewport);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	glRenderMode(GL_SELECT);
	glLoadIdentity();
	gluPickMatrix(xPos,viewport[3]-yPos,2,2,viewport);

	glOrtho(plane[0],plane[1],plane[2],plane[3],-10000,10000);
	gluLookAt(eye[0],eye[1],eye[2],scene->bsphere.center[0],scene->bsphere.center[1],scene->bsphere.center[2],up[0],up[1],up[2]);

	DrawScene();

	hits=glRenderMode(GL_RENDER);

	if(hits>=1)
	{
		ProcessModels(selectBuff);
		returnValue = 1;
	}

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);

	return returnValue;
}

void QSceneDisplay::ProcessModels( GLuint *pSelectBuff )
{
	int count;

	// ���ƶ�ջ���ж�������
	count=pSelectBuff[0]; 
	// ��ջ��β����zֵ��������
	selectModel=pSelectBuff[3];

	if (!isSelectedModelValid())
	{
		 QMessageBox::warning(this,tr("SelectObject"),tr("No Object Selected"),QMessageBox::Yes);
		 selectModel=-1;
		 return;
	}


}

void QSceneDisplay::TransModelAction()
{
	sceneDisplayState = ObjectTranslation;
}

void QSceneDisplay::RotateModelAction()
{
	//state=4;
	sceneDisplayState = ObjectRotation;
}

bool QSceneDisplay::isSelectedModelValid()
{
	if (selectModel<0 || selectModel>(scene->modelSize-1))
		return false;
	return true;
}

void QSceneDisplay::SetProjectionModelView()
{
	glGetDoublev(GL_PROJECTION_MATRIX,glProjectionM);
	glGetDoublev(GL_MODELVIEW_MATRIX,glModelM);
	glGetIntegerv(GL_VIEWPORT,glViewM);
}

//��ʱ���ø÷���������̫����
//���������������������ʵ�ַ�ʽ��ʰȡһ����������ʰȡ�����id�ĳ����²��������id
void QSceneDisplay::pickupCubeAction()
{
	/*
	//����������ǰһ״ֻ̬��Ҫ����InseartObjectCube
	if(sceneDisplayState != InseartObjectCube)
	{
		//״̬ת��
		this->sceneDisplayState = InseartObjectCube;
		//�ڳ���֮�в��뱣��Ҫ���ļ������Model
		Model* insertModel = new Model;
		this->scene->sceneModels.push_back(insertModel);
		//��¼Ҫ����cube
		this->selectModel = this->scene->modelSize+1;

		//����cube�ͳ���
		this->DrawScene();
	}
	*/
}

//bool sortRecommendModel(map<>)
//{
//
//}
typedef pair<string, float> PAIR;  
int importanceCmp(const PAIR& x, const PAIR& y)  
{  
	return x.second > y.second;  
}  

//������Ӧ�˵��ĵ���ģ�ͼ�������
//�ú���������ݲ���ļ�����λ�ã�ͨ�������㷨���ҵ���Ӧ��ģ���б����ģ���б��ݸ�QModelListDialog��������ʾ
//�޸�֮���˼·Ϊ�������ǰ�û���ʰȡģʽ��ѡ����ĳ�����壬�����õڶ��ּ�������
//�����ǰ�û�δѡ���������壬��ʹ�õ�һ�ּ�������,����������Ҫ�̶���ȷ���Ƽ���baseModel
void QSceneDisplay::searchInseartObject()  
{
	//״̬ת��
	this->sceneDisplayState = SearchSingleModel;

	//����dialog�Ի���
	modelListDialog = new QModelListDialog;
	//�ڶ��ּ�������
	if(this->sceneDisplayState == ObjectSelected)
	{
		 //��������б�
		 this->SearchModelsBySelectedLabel(this->selectModel);
	}
	//��һ�ּ�������
	else 
	{
		 int maxImportantModel = 0;
		 float maxImportant = scene->modelImportance[0];
         for (int i = 1; i< scene->modelSize ; i++)
         {
			if( scene->modelImportance[i] >maxImportant)
			{
				maxImportantModel = i;
				maxImportant = scene->modelImportance[i];
			}
         }

		 //��������б�
		 this->SearchModelsBySelectedLabel(maxImportantModel);
	}

	//�����Ƽ��б������,�ȸ��Ƽ��б�������ѡ3,3,2,2,2,1,1,1,1,1,1һ��11��label�����Ƽ����������������Ƽ�
	sort(recommendLabelAndWeight.begin(),recommendLabelAndWeight.end(),importanceCmp);


	//���ݼ������ݸ�modelListDialog������ʾ
	modelListDialog->pObjectMatchResult =this->pObjectMatchResult;
	connect(modelListDialog,SIGNAL(Inseart3DModel(int)),this,SLOT(Inseart3DModel(int)));
	int modelListDialogWidth = this->width()*0.75;
	int modelListDialogHeight = this->height()*0.75;
	modelListDialog->DownloadModelImage(modelListDialogWidth,modelListDialogHeight);
	modelListDialog->show();
}


//��������ӦQModelListDialog������źţ�download����ѡ�����壬����ٲ��볡�����ó����ػ�
void QSceneDisplay::Inseart3DModel(int selectedModel)
{
	//���ȼ�¼�û���ѡ��
	this->selected3DModel = selectedModel;

	//��ζ�ȡ����ģ��
	Model* insertModel = this->scene->sceneModels[this->scene->sceneModels.size()-1];
	string modelFilepath(this->pObjectMatchResult[selected3DModel].name);
	insertModel->ReadModel(modelFilepath);
	//����ԭ������ģ�͵�ʱ��scene->modelSizeû�䣬��������һ��Ҫ��1
	this->scene->modelSize++;

	//������ϵͳ��״̬�趨��ObjectRotation,��Ϊ�����²���ģ�͵�BBox
	this->sceneDisplayState = ObjectRotation;

	//�ػ���������
	this->DrawScene();
}



/************************************************************************/
/*/
//��������ѡ����ģ�����Ƽ��µ�ģ��
//ʵ�ַ�����1.��ȡ��ͬ��������µ����б�ǩ����
2.�洢��ǰ����label���� �����볡��label����
3.��ȡ��ͬ�����²�ͬlabel��relevence label
4.���ռ��з��������Ƽ�label����
5.�����Ƽ�label����
*/
/************************************************************************/
void QSceneDisplay::SearchModelsBySelectedLabel(int recommendBasedModel)
{
	//*********** 1. ��ȡÿ�ೡ���е����б�ǩ*********
	string buffer;
	string tempSceneName,tempLable;
	int labelNumb;
	string inPath;
	ifstream in;

	//********** 2.�洢��ǰ����label���� �����볡��label����*************
	for(int i=0; i< scene->modelSize; i++)
	{
		if(currentSceneLabels.count(scene->lightSceneModels[i].modelTag))
		{
			currentSceneLabels[scene->lightSceneModels[i].modelTag]++;
		}
		else
		{
			currentSceneLabels.insert(make_pair(scene->lightSceneModels[i].modelTag,1));
		}
	}

	//****************** 3.��ȡ��ͬ�����²�ͬlabel��relevence label ***************************
	string tempLabel1;
	int labelId,labelId1;
	double relevence;
	inPath = "SearchResult\\LabelRelevence.txt";
	in.open(inPath);
	if(!in)
	{
		//
	}

	while(!in.eof())
	{
		getline(in,buffer);
		istringstream line(buffer);
		line>>tempSceneName>>tempLable>>labelId>>tempLabel1>>labelId1>>relevence;
		//map<string,map<string,map<int,map<string,map<int,double>>>>> sceneLabelRelevence;  //���治ͬ�����в�ͬ����label�µ���ض�
		if(sceneLabelRelevence.count(tempSceneName))
		{
			if(sceneLabelRelevence[tempSceneName].count(tempLable))
			{
				if(sceneLabelRelevence[tempSceneName][tempLable].count(labelId))
				{
					if(sceneLabelRelevence[tempSceneName][tempLable][labelId].count(tempLabel1))
					{
                          sceneLabelRelevence[tempSceneName][tempLable][labelId][tempLabel1].insert(make_pair(labelId1,relevence));
					}
					else
					{
						map<int,double> temp;
						temp.insert(make_pair(labelId1,relevence));
						sceneLabelRelevence[tempSceneName][tempLable][labelId].insert(make_pair(tempLabel1,temp));
					}
				}
				else
				{
					map<int,double> temp;
					map<string,map<int,double>>temp1;
					temp.insert(make_pair(labelId1,relevence));
					temp1.insert(make_pair(tempLabel1,temp));
					sceneLabelRelevence[tempSceneName][tempLable].insert(make_pair(tempLable,temp1));
				}
			}
			else
			{
				map<int,double> temp;
				map<string,map<int,double>>temp1;
				map<int,map<string,map<int,double>>>temp2;
				temp.insert(make_pair(labelId1,relevence));
				temp1.insert(make_pair(tempLabel1,temp));
				temp2.insert(make_pair(labelId,temp2));
				sceneLabelRelevence[tempSceneName].insert(make_pair(tempLable,temp2));
			}
		}
		else
		{
			map<int,double> temp;
			map<string,map<int,double>>temp1;
			map<int,map<string,map<int,double>>>temp2;
			map<string,map<int,map<string,map<int,double>>>>temp3;
			temp.insert(make_pair(labelId1,relevence));
			temp1.insert(make_pair(tempLabel1,temp));
			temp2.insert(make_pair(labelId,temp2));
			temp3.insert(make_pair(tempLable,temp2));
			sceneLabelRelevence.insert(make_pair(tempSceneName,temp3));
		}
		line.clear();
		buffer.clear();
	}
	in.clear();
	in.close();


	//******************* 4.�����Ƽ�label****************
	//map<string,float> recommendLabelAndWeight; 
	//map<string,map<string,map<int,map<string,map<int,double>>>>> sceneLabelRelevence;  //���治ͬ�����в�ͬ����label�µ���ض�
	//a. ѡ��ģ�͵�releventģ��
	//if(selectModel > 0) //����ѡ��������ģ��
	//{
		//******************relevent ģ�Ͳ���***************
		string selectedLabel = scene->sceneModels[recommendBasedModel]->tag;  //��ȡ��ѡ�������tag
		int selectedLabelCount = currentSceneLabels[selectedLabel];
		map<string,map<int,double>>::iterator releventLabel = sceneLabelRelevence[sceneStyle][selectedLabel][selectedLabelCount].begin();
		map<int,double>::iterator labelCountIt;

		while( releventLabel != sceneLabelRelevence[sceneStyle][selectedLabel][selectedLabelCount].end())
		{
			if(currentSceneLabels.count(releventLabel->first))//������ǰ���������д�label
			{
				labelCountIt = sceneLabelRelevence[sceneStyle][selectedLabel][selectedLabelCount][releventLabel->first].begin();
				while( labelCountIt != sceneLabelRelevence[sceneStyle][selectedLabel][selectedLabelCount][releventLabel->first].end())
				{
					//�����ǰ����ӵ��ĳlabel������<relevence label����������label����
					if(currentSceneLabels[releventLabel->first] < labelCountIt->first)
					{
						if(!recommendLabelAndWeight.count(releventLabel->first))
						{
							recommendLabelAndWeight.insert(make_pair(releventLabel->first,0.5*labelCountIt->second));
						}
					}
					labelCountIt++;
				}//while

			}
			else  //�����ǰ�޴�label����ֱ�ӽ�����ӽ���
			{
				labelCountIt = sceneLabelRelevence[sceneStyle][selectedLabel][selectedLabelCount][releventLabel->first].begin();
				recommendLabelAndWeight.insert(make_pair(releventLabel->first,0.5*labelCountIt->second));
	    	}
			releventLabel++;
		}

		//**************** ���볡�������г����ȶԲ���*********************
		//map<string,int>sourceSceneLabels;
		map<string,int>::iterator  siIt= sourceSceneLabels.begin();
		while ( siIt != sourceSceneLabels.end())
		{
			if(currentSceneLabels.count(siIt->first))//�������г�������������Ƭ�еĸ����壬��ȶ�������
			{
				if( currentSceneLabels[siIt->first]  < siIt->second )//�����ǰ������ĳ��label����С�����볡���и�label����������뵽�Ƽ��б�
				{
					if(recommendLabelAndWeight.count(siIt->first))
					{
						recommendLabelAndWeight[siIt->first] +=0.3;
					}
					else
					{
						recommendLabelAndWeight.insert(make_pair(siIt->first,0.3));
					}
				}
			}
			siIt++;
		}//while


		//******************** �����������ıȶ�*****************
		//����һ���Ƽ����������ޣ�������Ҫ�����������������Ƽ�label ��Ŀ����һ������ʱ����������
		if(recommendLabelAndWeight.size() <8)
		{
			//�ȶ�ȡ������𳡾��е�label����
			inPath = "SearchResult\\AllSceneLableAndCount.txt";	
			in.open(inPath);
			if(!in)
			{
				//
			}
			//map<string,map<string,in>>sceneLabelAndCount;
			while(!in.eof())
			{
				getline(in,buffer);
				istringstream line(buffer);
				line>>tempSceneName>>tempLable>>labelNumb;

				if(sceneLabelAndCount.count(tempSceneName))
				{
					if( !sceneLabelAndCount[tempSceneName].count(tempLable))
					{
						sceneLabelAndCount[tempSceneName].insert(make_pair(tempLable,labelNumb));
					}
				}
				else
				{
					map<string,int>temp;
					temp.insert(make_pair(tempLable,labelNumb));
					sceneLabelAndCount.insert(make_pair(tempSceneName,temp));
				}
				line.clear();
				buffer.clear();
			}
			in.clear();
			in.close();

			siIt = sceneLabelAndCount[sceneStyle].begin();
			while (siIt != sceneLabelAndCount[sceneStyle].end())
			{
				if( !recommendLabelAndWeight.count(siIt->first))
				{
					recommendLabelAndWeight.insert(make_pair(siIt->first,0.2));
				}
				siIt++;
			}
		}//if

	//}//if
	
}
