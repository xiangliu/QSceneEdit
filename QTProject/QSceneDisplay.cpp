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
	sceneDisplayState = PrepareState; // 平移物体
	selectModel=-1;
	glProjectionM=new GLdouble[16];
	glModelM=new GLdouble[16];
	glViewM=new int[4];

	//用于保存单个模型检索结果
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
	GLfloat light_ambient[] = { 1.0, 1.0, 1.0, 1.0 };//环境光颜色
	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };//漫反射光颜色
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };//镜面反思光颜色
	GLfloat light_position[] = { 9999.0, 9999.0, 9999.0, 0.0 };//光源位置
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);//设置环境光
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);//设置漫反射光
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);//设置镜面反射光
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);//设置光源位置
	glEnable(GL_LIGHTING);//打开光线
	glEnable(GL_LIGHT0);//打开0号光源
	glEnable(GL_DEPTH_TEST);//打开深度测试

	glDepthFunc(GL_LEQUAL);							// 所作深度测试的类型
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);			// 真正精细的透视修正

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
			float dx=float(point.x()-btnDown.x())*(plane[1]-plane[0])/width(); // 转换到视景体移动的距离
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
			gluUnProject(btnDown.x(),invert_y,0,glModelM,glProjectionM,glViewM,&ax,&ay,&az); // 找到坐标系中点
			invert_y=height()-point.y();
			gluUnProject(point.x(),invert_y,0,glModelM,glProjectionM,glViewM,&bx,&by,&bz); // 找到坐标系中点
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
		xangle+=float(point.x()-btnDown.x())*90/width();  // 绕y旋转的角度，角度值
		yangle+=float(btnDown.y()-point.y())*90/height(); // 绕x轴旋转角度，角度值

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
		//表明拾取阶段
		//if (sceneDisplayState==PrepareState)
		//{
			//ProcessSelection(btnDown.x(),btnDown.y());
		if( ProcessSelection(btnDown.x(),btnDown.y()))
		{
			this->sceneDisplayState = ObjectSelected; //进入拾取阶段,表明拾取到了物体
		}
		//}
		////表明检索到单个model之后点击加入场景之中
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
	// 设置视点位置
	eye[0]=scene->bsphere.center[0];
	eye[1]=scene->bsphere.center[1];
	eye[2]=scene->bsphere.center[2]+radius;

	xangle=90;
	yangle=90;
	// 设置视景体的区域
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

	// 不绘制场景的墙壁
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
	sceneDisplayState = PrepareState; // 选择模型
}

int QSceneDisplay::ProcessSelection( int xPos,int yPos )
{
	int returnValue = 0; //用于定义返回值，0表示未拾取到物体，1表示拾取到物体
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

	// 名称堆栈中有多少名字
	count=pSelectBuff[0]; 
	// 堆栈的尾部，z值最大的物体
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

//暂时弃用该方案，由于太复杂
//用于往场景中输入检索框，实现方式跟拾取一样，仅仅将拾取物体的id改成了新插入物体的id
void QSceneDisplay::pickupCubeAction()
{
	/*
	//插入检索框的前一状态只需要不是InseartObjectCube
	if(sceneDisplayState != InseartObjectCube)
	{
		//状态转换
		this->sceneDisplayState = InseartObjectCube;
		//在场景之中插入保存要画的检索框的Model
		Model* insertModel = new Model;
		this->scene->sceneModels.push_back(insertModel);
		//记录要画的cube
		this->selectModel = this->scene->modelSize+1;

		//绘制cube和场景
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

//用来响应菜单的单个模型检索功能
//该函数必须根据插入的检索框位置，通过检索算法查找到相应的模型列表，随后将模型列表传递给QModelListDialog，让其显示
//修改之后的思路为：如果当前用户在拾取模式下选择了某个物体，我们用第二种检索策略
//如果当前用户未选择人物物体，则使用第一种检索策略,根据物体重要程度来确定推荐的baseModel
void QSceneDisplay::searchInseartObject()  
{
	//状态转换
	this->sceneDisplayState = SearchSingleModel;

	//创建dialog对话框
	modelListDialog = new QModelListDialog;
	//第二种检索策略
	if(this->sceneDisplayState == ObjectSelected)
	{
		 //计算检索列表
		 this->SearchModelsBySelectedLabel(this->selectModel);
	}
	//第一种检索策略
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

		 //计算检索列表
		 this->SearchModelsBySelectedLabel(maxImportantModel);
	}

	//构造推荐列表的数据,先给推荐列表排序，再选3,3,2,2,2,1,1,1,1,1,1一共11类label进行推荐，如果不够，随机推荐
	sort(recommendLabelAndWeight.begin(),recommendLabelAndWeight.end(),importanceCmp);


	//传递检索数据给modelListDialog，并显示
	modelListDialog->pObjectMatchResult =this->pObjectMatchResult;
	connect(modelListDialog,SIGNAL(Inseart3DModel(int)),this,SLOT(Inseart3DModel(int)));
	int modelListDialogWidth = this->width()*0.75;
	int modelListDialogHeight = this->height()*0.75;
	modelListDialog->DownloadModelImage(modelListDialogWidth,modelListDialogHeight);
	modelListDialog->show();
}


//本函数响应QModelListDialog发射的信号，download被挑选的物体，随后再插入场景，让场景重绘
void QSceneDisplay::Inseart3DModel(int selectedModel)
{
	//首先记录用户的选择
	this->selected3DModel = selectedModel;

	//其次读取单个模型
	Model* insertModel = this->scene->sceneModels[this->scene->sceneModels.size()-1];
	string modelFilepath(this->pObjectMatchResult[selected3DModel].name);
	insertModel->ReadModel(modelFilepath);
	//由于原本插入模型的时候scene->modelSize没变，所以这里一定要加1
	this->scene->modelSize++;

	//将整个系统的状态设定成ObjectRotation,因为还有新插入模型的BBox
	this->sceneDisplayState = ObjectRotation;

	//重绘整个场景
	this->DrawScene();
}



/************************************************************************/
/*/
//基于用于选定的模型来推荐新的模型
//实现方案：1.读取不同场景类别下的所有标签集合
2.存储当前场景label集合 和输入场景label集合
3.读取不同场景下不同label的relevence label
4.按照既有方案计算推荐label集合
5.返回推荐label集合
*/
/************************************************************************/
void QSceneDisplay::SearchModelsBySelectedLabel(int recommendBasedModel)
{
	//*********** 1. 读取每类场景中的所有标签*********
	string buffer;
	string tempSceneName,tempLable;
	int labelNumb;
	string inPath;
	ifstream in;

	//********** 2.存储当前场景label集合 和输入场景label集合*************
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

	//****************** 3.读取不同场景下不同label的relevence label ***************************
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
		//map<string,map<string,map<int,map<string,map<int,double>>>>> sceneLabelRelevence;  //保存不同场景中不同物体label下的相关度
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


	//******************* 4.计算推荐label****************
	//map<string,float> recommendLabelAndWeight; 
	//map<string,map<string,map<int,map<string,map<int,double>>>>> sceneLabelRelevence;  //保存不同场景中不同物体label下的相关度
	//a. 选定模型的relevent模型
	//if(selectModel > 0) //表明选定了摸个模型
	//{
		//******************relevent 模型部分***************
		string selectedLabel = scene->sceneModels[recommendBasedModel]->tag;  //获取被选定物体的tag
		int selectedLabelCount = currentSceneLabels[selectedLabel];
		map<string,map<int,double>>::iterator releventLabel = sceneLabelRelevence[sceneStyle][selectedLabel][selectedLabelCount].begin();
		map<int,double>::iterator labelCountIt;

		while( releventLabel != sceneLabelRelevence[sceneStyle][selectedLabel][selectedLabelCount].end())
		{
			if(currentSceneLabels.count(releventLabel->first))//表明当前场景中已有次label
			{
				labelCountIt = sceneLabelRelevence[sceneStyle][selectedLabel][selectedLabelCount][releventLabel->first].begin();
				while( labelCountIt != sceneLabelRelevence[sceneStyle][selectedLabel][selectedLabelCount][releventLabel->first].end())
				{
					//如果当前场景拥有某label的数量<relevence label的数量，则将label加入
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
			else  //如果当前无此label，则直接将其添加进来
			{
				labelCountIt = sceneLabelRelevence[sceneStyle][selectedLabel][selectedLabelCount][releventLabel->first].begin();
				recommendLabelAndWeight.insert(make_pair(releventLabel->first,0.5*labelCountIt->second));
	    	}
			releventLabel++;
		}

		//**************** 输入场景和现有场景比对部分*********************
		//map<string,int>sourceSceneLabels;
		map<string,int>::iterator  siIt= sourceSceneLabels.begin();
		while ( siIt != sourceSceneLabels.end())
		{
			if(currentSceneLabels.count(siIt->first))//表明现有场景已有输入照片中的该物体，则比对其数量
			{
				if( currentSceneLabels[siIt->first]  < siIt->second )//如果当前场景中某个label数量小于输入场景中该label数量，则加入到推荐列表
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


		//******************** 整个场景类别的比对*****************
		//由于一次推荐的数量有限，所以需要控制数量，当已有推荐label 数目超过一定数量时，则不再启用
		if(recommendLabelAndWeight.size() <8)
		{
			//先读取所有类别场景中的label集合
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
