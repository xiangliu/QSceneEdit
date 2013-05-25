#include "QSceneDisplay.h"
#include <QMessageBox>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <strstream>
#include <iomanip>
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
	sceneDisplayState = SceneState; // 平移物体
	selectModel=-1;
	glProjectionM=new GLdouble[16];
	glModelM=new GLdouble[16];
	glViewM=new int[4];

	//用于保存单个模型检索结果
	selected3DModel = -1;
	this->pObjectMatchResult = new ObjectMatRes[MODELSEARCHRESULTNUMBER];

	//模型推荐部分初始化工作
	isFirstRecommend = true;
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
	if (event->buttons() & Qt::LeftButton) //平移操作
	{
		if(sceneDisplayState == SceneState )  //对整个场景进行操作
		{
			float dx=float(point.x()-btnDown.x())*(plane[1]-plane[0])/width(); // 转换到视景体移动的距离
			float dy=float(btnDown.y()-point.y())*(plane[3]-plane[2])/height();
			plane[0]-=dx;
			plane[1]-=dx;
			plane[2]-=dy;
			plane[3]-=dy;
			//btnDown=point;
		}
		else if (sceneDisplayState== ObjectState && isSelectedModelValid())//ObjectTranslation
		{
			GLdouble ax,ay,az,bx,by,bz;
			int invert_y= height() - btnDown.y();
			gluUnProject(btnDown.x(),invert_y,0,glModelM,glProjectionM,glViewM,&ax,&ay,&az); // 找到坐标系中点
			invert_y=height()-point.y();
			gluUnProject(point.x(),invert_y,0,glModelM,glProjectionM,glViewM,&bx,&by,&bz); // 找到坐标系中点
			
			Model* model = scene->GetModel(selectModel);
			//Model* model=scene->sceneModels[selectModel];
			model->tx+=bx-ax;
			model->ty+=by-ay;
			model->tz+=bz-az;
		}
	}
	else if (event->buttons()&Qt::RightButton) //旋转操作
	{
		if ( sceneDisplayState == SceneState )
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
		}
		else if ( sceneDisplayState==ObjectState && isSelectedModelValid() )
		{
            Model* model = scene->GetModel(selectModel);
			//Model* model=scene->sceneModels[selectModel];
			model->xangle+=double(point.y()-btnDown.y())/3.6;
			model->yangle+=double(point.x()-btnDown.x())/3.6;
		}
		
		//btnDown=point;
	}
	btnDown=point;
    this->updateGL();
}

//仅仅用于拾取
void QSceneDisplay::mouseDoubleClickEvent(QMouseEvent *event)
{
	if(scene==NULL)
		return;
	//setMouseTracking(true);
	//btnDown=event->pos();

	if (event->button()==Qt::LeftButton)
	{

		if( ProcessSelection(event->pos().x(),event->pos().y()))
		{
			this->sceneDisplayState = ObjectState; //进入拾取阶段,表明拾取到了物体
		}
	}
	this->updateGL();
}


void QSceneDisplay::mousePressEvent(QMouseEvent *event)
{
	if(scene==NULL)
		return;
	setMouseTracking(true);
	btnDown=event->pos();
	//if (event->button()==Qt::LeftButton)
	//{
	//	//表明拾取阶段
	//	//if (sceneDisplayState==PrepareState)
	//	//{
	//		//ProcessSelection(btnDown.x(),btnDown.y());
	//	if( ProcessSelection(btnDown.x(),btnDown.y()))
	//	{
	//		this->sceneDisplayState = ObjectState; //进入拾取阶段,表明拾取到了物体
	//	}
	//	//}
	//	////表明检索到单个model之后点击加入场景之中
	//	//else if(this->sceneDisplayState == SearchSingleModel)
	//	//{

	//	//}
	//	//else if (state==3 && isSelectedModelValid())
	//	//{
	//	//	int invert_y=height()-btnDown.y();
	//	//	scene->sceneModels[selectModel]->ball.arcball_tranStart(btnDown.x(),invert_y);
	//	//}
	//	//else if (state==4 && isSelectedModelValid())
	//	//{
	//	//	int invert_y=height()-btnDown.y();
	//	//	scene->sceneModels[selectModel]->ball.arcball_rotStart(btnDown.x(),invert_y);
	//	//}
	//}
}

void QSceneDisplay::wheelEvent(QWheelEvent *event)
{
	if (sceneDisplayState== SceneState)
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
	else if (sceneDisplayState ==ObjectState  && isSelectedModelValid())
	{
		double numDegrees = event->delta() / 8.0;
		double numSteps = numDegrees / 15.0;

		//scene->sceneModels[selectModel]->scaled *=pow(1.125, numSteps);
		////scene->sceneModels[selectModel]->scale+=event->delta()/100;
		Model* t = scene->GetModel(selectModel);
		t->scaled *= pow(1.125, numSteps);
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
	//if(sceneDisplayState > 2 && isSelectedModelValid())
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

	//***********旧的绘制模型和包围盒****************
	//scene->DrawScene();
	//if ( selectModel<0 )
	//{
	//	return;
	//}
	//else
	//{
	//	scene->sceneModels[selectModel]->DrawBbox();
	//}

	//新的绘制模型和包围盒方法
	scene->DrawSceneWithNewInsertModel();
	
	if ( selectModel<0 )
	{
		return;
	}
	else if( selectModel < this->scene->sceneModels.size() )
	{
		scene->sceneModels[selectModel]->DrawBbox();
	}
	else if(selectModel < this->scene->GetAllModelSize())
	{
		scene->newInsertModels[selectModel - this->scene->sceneModels.size()]->DrawBbox();
	}
	else
	{
		return ;
	}
	
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


bool QSceneDisplay::isSelectedModelValid()
{
	//if (selectModel<0 || selectModel>(scene->modelSize-1))
	//	return false;
	//return true;
	if (selectModel<0 || selectModel>(scene->GetAllModelSize()-1))
		return false;
	return true;
}

void QSceneDisplay::SetProjectionModelView()
{
	glGetDoublev(GL_PROJECTION_MATRIX,glProjectionM);
	glGetDoublev(GL_MODELVIEW_MATRIX,glModelM);
	glGetIntegerv(GL_VIEWPORT,glViewM);
}

////暂时弃用该方案，由于太复杂
////用于往场景中输入检索框，实现方式跟拾取一样，仅仅将拾取物体的id改成了新插入物体的id
//void QSceneDisplay::pickupCubeAction()
//{
//	//string toLowerCase;
//	sameModelListDialog = new QSameModelListDialog;
//	strstream ss;
//	string temp;
//	//更换模型类别必须在选定某个模型之后进行
//	if(this->sceneDisplayState == ObjectState)
//	{
//		string selectedLabel = scene->sceneModels[this->selectModel]->tag; 
//		transform(selectedLabel.begin(),selectedLabel.end(),selectedLabel.begin(),tolower);
//		for(int i = 1; i<19; i++)
//		{
//			ss<<i;
//			ss>>temp;
//			sameModelListDialog->objectFilepath.push_back( "3DModelDatabase\\"+selectedLabel+"\\"+selectedLabel+temp+"\\"+selectedLabel+temp);
//			ss.clear();
//		}
//
//		int modelListDialogWidth = this->width()*0.75;
//		int modelListDialogHeight = this->height()*0.75;
//		sameModelListDialog->DownloadModelImage(modelListDialogWidth,modelListDialogHeight);
//		sameModelListDialog->show();
//	}
//}


//用来响应菜单的单个模型检索功能
//该函数必须根据插入的检索框位置，通过检索算法查找到相应的模型列表，随后将模型列表传递给QModelListDialog，让其显示
//修改之后的思路为：如果当前用户在拾取模式下选择了某个物体，我们用第二种检索策略
//如果当前用户未选择人物物体，则使用第一种检索策略,根据物体重要程度来确定推荐的baseModel
void QSceneDisplay::searchInseartObject()  
{
	//创建dialog对话框
	modelListDialog = new QModelListDialog;
	//第二种检索策略
	if(this->sceneDisplayState == ObjectState)
	{
		 //计算检索列表
		 this->recommendModelsBySelectedLabel2(this->selectModel);
	}
	//第一种检索策略
	else 
	{
		 int maxImportantModel = 0;
		 float maxImportant = -1;
         for (int i = 0; i< scene->modelSize ; i++)
         {
			if( scene->modelImportance[i] >maxImportant && scene->sceneModels[i]->tag!="Wall"&& scene->sceneModels[i]->tag!="Wall1" && scene->sceneModels[i]->tag!="Wall2")
			{
				maxImportantModel = i;
				maxImportant = scene->modelImportance[i];
			}
         }

		 //计算检索列表
		 this->recommendModelsBySelectedLabel2(maxImportantModel);
	}

	////测试使用
	ofstream out;
	string outPutpath = "SearchResult\\RecommendLabelResult.txt";
	out.open(outPutpath);

	//构造推荐列表的数据,先给推荐列表排序，再选3,3,2,2,2,1,1,1,1,1,1一共11类label进行推荐，如果不够，随机推荐
	map<string,double>::iterator recoIt = recommendLabelAndWeight.begin();
	map<double,string>tempRecommend;
	while(recoIt!= recommendLabelAndWeight.end())
	{
		if(tempRecommend.count(recoIt->second))
		{
			recoIt->second += 0.00001;
		}
		else
		{
	        tempRecommend.insert(make_pair(recoIt->second,recoIt->first));
			recoIt++;
		}
	}

	string toLowerCase;
	map<double,string>::reverse_iterator recoIt1 = tempRecommend.rbegin();
	if(tempRecommend.size() >= 18)
	{

		for(int i = 0; i<18; i++)
		{
			toLowerCase = recoIt1->second;
			out<<toLowerCase<<endl;
			transform(toLowerCase.begin(),toLowerCase.end(),toLowerCase.begin(),tolower);
			//modelListDialog->objectFilepath.push_back( "3DModelDatabase\\"+recoIt1->second+"\\"+recoIt1->second+"1");
			modelListDialog->objectFilepath.push_back( "3DModelDatabase\\"+toLowerCase+"\\"+toLowerCase+"1"+"\\"+toLowerCase+"1");
			recoIt1++;
		}
		
	}

	out.close();

	//状态转换
	this->sceneDisplayState = ObjectState;

	//传递检索数据给modelListDialog，并显示
	//modelListDialog->pObjectMatchResult =this->pObjectMatchResult;
	connect(modelListDialog,SIGNAL(Inseart3DModel(int)),this,SLOT(Inseart3DModel(int)));
	int modelListDialogWidth = this->width()*0.75;
	int modelListDialogHeight = this->height()*0.75;
	modelListDialog->DownloadModelImage(modelListDialogWidth,modelListDialogHeight);
	modelListDialog->show();
	
}
//为插入的模型计算模型相关信息
void QSceneDisplay::generateInseartModleInformation(Model* insertModel)
{
	//用于给新插入的模型命名
	strstream ss;
	string tempModelSize;
	ss<<this->scene->modelSize;
	ss>>tempModelSize;
	insertModel->name = "inseartModel"+tempModelSize;
	insertModel->scene = scene;
	//insertModel->need_bbox();//此处不再使用，因为仅仅适应于场景内的物体，单独的物体还是用trimesh自带方法计算
	//insertModel->mesh->need_bbox();
	insertModel->need_bbox_newInseartOBJ();

	insertModel->visible = true;

	//float
	float sizeScale;  //计算模型伸缩比例大小
	float tempLength1 = insertModel->bbox.size().sumabs();  //求对角线的平方
	float sceneLength = scene->bbox.size().sumabs();
	sizeScale = sceneLength/(tempLength1*2.0);
	insertModel->scaled = sizeScale;
	//insertModel->scaled = 1.0;

	insertModel->need_bsphere_newInseartOBJ();

	insertModel->tx =( scene->bbox.center() - insertModel->bbox.center())[0];
	insertModel->ty =( scene->bbox.center() - insertModel->bbox.center())[1];
	insertModel->tz =( scene->bbox.center() - insertModel->bbox.center())[2];
}

//本函数响应QModelListDialog发射的信号，download被挑选的物体，随后再插入场景，让场景重绘
void QSceneDisplay::Inseart3DModel(int sModel)
{
	//首先记录用户的选择
	this->selected3DModel = sModel;
	
	Model* insertModel = new Model;
	//string modelFilepath(this->pObjectMatchResult[selected3DModel].name);
	string modelFilepath(this->modelListDialog->objectFilepath[sModel]);;
	//insertModel->ReadModel(modelFilepath+".obj");
	insertModel->ReadOBJ(modelFilepath+".obj");

	////由于原本插入模型的时候scene->modelSize没变，所以这里一定要加1
	//this->scene->sceneModels.push_back(insertModel);
	//this->scene->modelSize++;
	this->scene->newInsertModels.push_back(insertModel);
	this->scene->insertModelSize++;
	this->scene->allModelSize = this->scene->insertModelSize+ this->scene->modelSize;
	
	this->generateInseartModleInformation(insertModel);

	this->scene->ModelMap.insert(make_pair(insertModel->name,this->scene->GetAllModelSize()-1));

	selectModel = this->scene->GetAllModelSize()-1;
	//selectModel = 1;

	//重绘整个场景
	this->DrawScene();
	//insertModel->DrawTrimesh();
}



/************************************************************************/
/* 暂时被弃用
//基于用于选定的模型来推荐新的模型
//实现方案：1.读取不同场景类别下的所有标签集合
2.存储当前场景label集合 和输入场景label集合
3.读取不同场景下不同label的relevence label
4.按照既有方案计算推荐label集合
5.返回推荐label集合
*/
/************************************************************************/
void QSceneDisplay::recommendModelsBySelectedLabel(int recommendBasedModel)
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
		if(currentSceneLabels.count(scene->sceneModels[i]->tag))
		{
			currentSceneLabels[scene->sceneModels[i]->tag]++;
		}
		else
		{
			currentSceneLabels.insert(make_pair(scene->sceneModels[i]->tag,1));
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
					sceneLabelRelevence[tempSceneName][tempLable].insert(make_pair(labelId,temp1));
				}
			}
			else
			{
				map<int,double> temp;
				map<string,map<int,double>>temp1;
				map<int,map<string,map<int,double>>>temp2;
				temp.insert(make_pair(labelId1,relevence));
				temp1.insert(make_pair(tempLabel1,temp));
				temp2.insert(make_pair(labelId,temp1));
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
			temp2.insert(make_pair(labelId,temp1));
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
		if(recommendLabelAndWeight.size() <18)
		{
			//先读取所有类别场景中的label集合
			inPath = "MLData\\AllSceneLableAndCount.txt";	
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


/************************************************************************/
/*/第二版本的模型推荐算法
//基于用于选定的模型来推荐新的模型
//实现方案：1.读取不同场景类别下的所有标签集合
2.存储当前场景label集合 和输入场景label集合
3.读取不同场景下不同label的relevence label
4.按照既有方案计算推荐label集合
5.返回推荐label集合
*/
/************************************************************************/
void QSceneDisplay::recommendModelsBySelectedLabel2(int recommendBasedModel)
{
	//防止上一次的数据对本次推荐产生污染，所以必须对上次推荐数据进行清空
	if( !currentSceneLabels.empty())
	{
		currentSceneLabels.clear();
	}
	
	if (!recommendLabelAndWeight.empty())
	{
		recommendLabelAndWeight.clear();
	}
	
	//*********** 1. 读取每类场景中的所有标签*********
	string buffer;
	string tempSceneName,tempLable;
	int labelNumb;
	string inPath;
	ifstream in;

	//********** 2.存储当前场景label集合 和输入场景label集合<已经由mainwindow传递过来>*************
	//if(isFirstRecommend)  //可能在第二次推荐过程中有些模型增加或者被删除
	//{
		for(int i=0; i< scene->modelSize; i++)
		{
			if(scene->sceneModels[i]->visible) //仅仅当模型可见才需要添加，因为某些模型被删除也仅仅设置成不可见
			{
				if(currentSceneLabels.count(scene->sceneModels[i]->tag))
				{
					currentSceneLabels[scene->sceneModels[i]->tag]++;
				}
				else
				{
					currentSceneLabels.insert(make_pair(scene->sceneModels[i]->tag,1));
				}//else
			}//if

		}//for
	//}

	

	//****************** 3.读取不同场景下不同label的relevence label ***************************
	string tempLabel1;
	int labelId,labelId1;
	double relevence;

	if (isFirstRecommend) //仅仅需要初始化一次
	{
		inPath = "SearchResult\\LabelRelevencePMI.txt";
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
						sceneLabelRelevence[tempSceneName][tempLable].insert(make_pair(labelId,temp1));
					}
				}
				else
				{
					map<int,double> temp;
					map<string,map<int,double>>temp1;
					map<int,map<string,map<int,double>>>temp2;
					temp.insert(make_pair(labelId1,relevence));
					temp1.insert(make_pair(tempLabel1,temp));
					temp2.insert(make_pair(labelId,temp1));
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
				temp2.insert(make_pair(labelId,temp1));
				temp3.insert(make_pair(tempLable,temp2));
				sceneLabelRelevence.insert(make_pair(tempSceneName,temp3));
			}
			line.clear();
			buffer.clear();
		}
		in.clear();
		in.close();
	}


	
	//******************* 4.计算推荐label****************

	//******************relevent 模型部分***************
	string selectedLabel = scene->sceneModels[recommendBasedModel]->tag;  //获取被选定物体的tag
	int selectedLabelCount = currentSceneLabels[selectedLabel];
	map<string,map<int,double>>::iterator releventLabel = sceneLabelRelevence[sceneStyle][selectedLabel][selectedLabelCount].begin();
	map<int,double>::iterator labelCountIt;

	double maxPMI = -10.0; //用于对PMI值进行归一化
	double minPMI = 10.0; //用于对PMI值进行归一化
	double inteval; 

	//统计被选中label的relevent label
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
						recommendLabelAndWeight.insert(make_pair(releventLabel->first,labelCountIt->second));
						if(labelCountIt->second > maxPMI)
						{
							maxPMI = labelCountIt->second;
						}
						else if( labelCountIt->second < minPMI)
						{
							minPMI = labelCountIt->second;
						}
					}
				}
				labelCountIt++;
			}//while

		}
		else  //如果当前无此label，则直接将其添加进来
		{
			labelCountIt = sceneLabelRelevence[sceneStyle][selectedLabel][selectedLabelCount][releventLabel->first].begin();
			recommendLabelAndWeight.insert(make_pair(releventLabel->first,labelCountIt->second));
			
			if(labelCountIt->second > maxPMI)
			{
				maxPMI = labelCountIt->second;
			}
			else if( labelCountIt->second < minPMI)
			{
				minPMI = labelCountIt->second;
			}
	    }
		releventLabel++;
	}

	//对PMI进行归一化
	map<string,double>::iterator gyIt = recommendLabelAndWeight.begin();
	inteval = maxPMI - minPMI;
	if( inteval )
	{
		while( gyIt != recommendLabelAndWeight.end())
		{
			gyIt->second = 0.7*(gyIt->second-minPMI)/inteval;
			gyIt++;
		}
	}



	//**************** 输入场景和现有场景比对部分*********************
	map<string,int>::iterator  siIt= sourceSceneLabels.begin();
	while ( siIt != sourceSceneLabels.end())
	{
		if(currentSceneLabels.count(siIt->first))//表明现有场景已有输入照片中的该物体，则比对其数量
		{
			if( currentSceneLabels[siIt->first]  < siIt->second )//如果当前场景中某个label数量小于输入场景中该label数量，则加入到推荐列表
			{
				if(recommendLabelAndWeight.count(siIt->first))
				{
					recommendLabelAndWeight[siIt->first] +=0.1;
				}
				else
				{
					recommendLabelAndWeight.insert(make_pair(siIt->first,0.5));
				}
			}
		}
		siIt++;
	}//while


   //*****************读取该场景类别下该模型类别对应的relationship partner****************
	vector<string> partner;
	inPath = "SearchResult\\AllLabelRelationPartnerSet.txt";
	in.open(inPath);
	if(!in)
	{
		//
	}

	while(!in.eof())
	{
		getline(in,buffer);
		istringstream line(buffer);
		line>>tempSceneName>>tempLable>>tempLabel1;
		if ( tempSceneName==sceneStyle && tempLable ==selectedLabel)
		{
			partner.push_back(tempLabel1);
		}
	}
	in.clear();
	in.close();

	for (int i = 0; i < partner.size(); i++)
	{
		if(recommendLabelAndWeight.count(partner[i]))
		{
			recommendLabelAndWeight[partner[i]] +=0.2;
		}
		else
		{
			recommendLabelAndWeight.insert(make_pair(partner[i],0.5));
		}
	}


	//******************** 整个场景类别中所包含的模型类别的比对*****************
	
	if(isFirstRecommend) //先读取所有类别场景中的label集合
	{
		inPath = "MLData\\AllSceneLableAndCount.txt";	
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

	}
	
	//由于一次推荐的数量有限，所以需要控制数量，当已有推荐label 数目超过一定数量时，则不再启用
	if(recommendLabelAndWeight.size() <18)
	{
		////先读取所有类别场景中的label集合
		//inPath = "MLData\\AllSceneLableAndCount.txt";	
		//in.open(inPath);
		//if(!in)
		//{
		//	//
		//}
		////map<string,map<string,in>>sceneLabelAndCount;
		//while(!in.eof())
		//{
		//	getline(in,buffer);
		//	istringstream line(buffer);
		//	line>>tempSceneName>>tempLable>>labelNumb;

		//	if(sceneLabelAndCount.count(tempSceneName))
		//	{
		//		if( !sceneLabelAndCount[tempSceneName].count(tempLable))
		//		{
		//			sceneLabelAndCount[tempSceneName].insert(make_pair(tempLable,labelNumb));
		//		}
		//	}
		//	else
		//	{
		//		map<string,int>temp;
		//		temp.insert(make_pair(tempLable,labelNumb));
		//		sceneLabelAndCount.insert(make_pair(tempSceneName,temp));
		//	}
		//	line.clear();
		//	buffer.clear();
		//}
		//in.clear();
		//in.close();

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


	//输出结果
	//string tempRemoveLable;
	ofstream out;
	string outPath = "SearchResult\\RecommendLabelResult.txt";
	out.open(outPath);
	if(!out)
	{
		//
	}

	gyIt = recommendLabelAndWeight.begin();
	//while( gyIt != recommendLabelAndWeight.end() )
	//{
	//	out<<sceneStyle<<" "<<gyIt->first<<" "<<gyIt->second<<endl;
	//	gyIt++;
	//}

	//要对非法的label过滤，同时对于一些不需要的label（wall、window等）也要过滤
	while( gyIt != recommendLabelAndWeight.end() )
	{
		if(sceneLabelAndCount[sceneStyle].count(gyIt->first) && gyIt->first!= "Wall")
		{
			out<<gyIt->first<<" "<<setprecision(8)<<gyIt->second<<endl;
			gyIt++;
		}
		else
		{
			recommendLabelAndWeight.erase(gyIt++);
		}
		
	}

	out.close();

	//避免重复初始化等工作
	isFirstRecommend = false;
	
}

/*如果当前为单个模型操作状态，那么必定有某个模型是被绘制了包围盒，selectedObject肯定有效
 所以处理过程中需要先将整体状态置为 sceneState，然后将selectedObject置为-1；再重绘
 如果本来就sceneState，则什么也不用做
*/
void QSceneDisplay::sceneOperationAction()
{
	if (sceneDisplayState == ObjectState)
	{
		this->selected3DModel = -1;
		this->sceneDisplayState = SceneState;
		//this->DrawScene();
		this->updateGL();
	}
}

//用于将场景中某个物体用同类别的模型进行替换
void QSceneDisplay::exchangeModelAction()
{
	sameModelListDialog = new QSameModelListDialog;
	strstream ss;
	string temp;
	//更换模型类别必须在选定某个模型之后进行
	if(this->sceneDisplayState == ObjectState)
	{
		//string selectedLabel = scene->sceneModels[this->selectModel]->tag; 
		Model* selectModelT = this->scene->GetModel(this->selectModel);
		string selectedLabel = selectModelT->tag;
		transform(selectedLabel.begin(),selectedLabel.end(),selectedLabel.begin(),tolower);
		for(int i = 1; i<19; i++)
		{
			ss<<i;
			ss>>temp;
			sameModelListDialog->objectFilepath.push_back( "3DModelDatabase\\"+selectedLabel+"\\"+selectedLabel+temp+"\\"+selectedLabel+temp);
			ss.clear();
		}

		int modelListDialogWidth = this->width()*0.75;
		int modelListDialogHeight = this->height()*0.75;
		sameModelListDialog->DownloadModelImage(modelListDialogWidth,modelListDialogHeight);
		sameModelListDialog->show();
	}
}

// 用于将场景中某个模型删除，事实上是设置成invisible
void QSceneDisplay::removeModelAction()
{
	if(this->sceneDisplayState == ObjectState)
	{
         Model* selectModelT = this->scene->GetModel(this->selectModel);
		 if (selectModelT != NULL)
		 {
			 selectModelT->visible = false;
			 this->sceneDisplayState = SceneState;
			 this->selectModel = -1;
			 //this->DrawScene();
			 this->updateGL();
		 }
	}
}
