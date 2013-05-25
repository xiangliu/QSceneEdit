#include "Model.h"
#include <fstream>
#include <sstream>
using namespace std;


Model::Model(void)
{
	visible=true;
	ModelMatrix=new GLdouble[16];
	scaled=1.0;
	xangle=yangle=0.0;
	faceAreaSum = NULL;
	samplePoints = NULL;
	pointNumber = 0;
	mesh=NULL;
}

Model::~Model(void)
{
	delete []ModelMatrix;
	if(faceAreaSum != NULL)
	{
		delete []faceAreaSum;
	}
	if(samplePoints != NULL)
	{
		delete []samplePoints;
	}
	if (mesh!=NULL)
		delete mesh;

	//*************新定义的关于有google warehouse导入模型的变量****************
	if(modelFaces.size() != 0)
	{
		for (int i = 0 ; i< modelFaces.size(); i++)
		{
			delete modelFaces[i];
		}
	}

	if(pointVect.size() != 0)
	{
		for (int i = 0 ; i< pointVect.size(); i++)
		{
			delete pointVect[i];
		}
	}

	if(normalVect.size() != 0)
	{
		for (int i = 0 ; i< normalVect.size(); i++)
		{
			delete normalVect[i];
		}
	}

}

void Model::DrawModel()
{
	glPushMatrix();
	glTranslated(tx,ty,tz);
	glRotated(yangle,0.0,1.0,0.0);
	glRotated(xangle,1.0,0.0,0.0);
	glScalef(scaled,scaled,scaled);
	glTranslatef(-bsphere.center[0],-bsphere.center[1],-bsphere.center[2]);

	glGetDoublev(GL_MODELVIEW_MATRIX,ModelMatrix);
	// load名称，方便拾取
	glLoadName(scene->ModelMap[this->name]);
	glColor3b(255,0,0);
	glBegin(GL_TRIANGLES);
	for (int i=faceStart;i<faceEnd;i++)
	{
		Face* face=scene->faces[i];
		for(int j=0;j<3;j++)
		{
			glNormal3f(scene->vnormals[face->vn[j]][0],scene->vnormals[face->vn[j]][1],scene->vnormals[face->vn[j]][2]);
			glVertex3f(scene->points[face->v[j]][0],scene->points[face->v[j]][1],scene->points[face->v[j]][2]);
		}
	}
	glEnd(); 
	glPopMatrix();
	glFlush();
}

//绘制新插入的模型
void Model::DrawNewInseartModel()
{
	glPushMatrix();
	glTranslated(tx,ty,tz);
	glRotated(yangle,0.0,1.0,0.0);
	glRotated(xangle,1.0,0.0,0.0);
	glScalef(scaled,scaled,scaled);
	glTranslatef(-bsphere.center[0],-bsphere.center[1],-bsphere.center[2]);

	glGetDoublev(GL_MODELVIEW_MATRIX,ModelMatrix);
	// load名称，方便拾取
	glLoadName(scene->ModelMap[this->name]);
	glColor3b(255,0,0);
	glBegin(GL_TRIANGLES);
	//for (int i=faceStart;i<faceEnd;i++)
	//{
	//	Face* face=scene->faces[i];
	//	for(int j=0;j<3;j++)
	//	{
	//		glNormal3f(scene->vnormals[face->vn[j]][0],scene->vnormals[face->vn[j]][1],scene->vnormals[face->vn[j]][2]);
	//		glVertex3f(scene->points[face->v[j]][0],scene->points[face->v[j]][1],scene->points[face->v[j]][2]);
	//	}
	//}

	for (int i=0;i<modelFaces.size();i++)
	{
		for(int j=0;j<3;j++)
		{
			vnormal* tv = normalVect[modelFaces[i]->vn[j]];
			point* tp = pointVect[modelFaces[i]->v[j]];
			glNormal3f((*tv)[0],(*tv)[1],(*tv)[2]);
			glVertex3f((*tp)[0],(*tp)[1],(*tp)[2]);
		}
	}

	glEnd();
	glPopMatrix();
	glFlush();
}

void Model::need_bbox()
{
	if(pointMap.size()==0) //没有面片
		return;
	
	map<int,point>::iterator it;
	for(it=pointMap.begin();it!=pointMap.end();it++)
		bbox+=it->second;

	bbox.valid=true;
}

void Model::need_bbox_newInseartOBJ()
{
	if(pointVect.size()==0) //没有面片
		return;

	point* tt;
	for (int i = 0 ; i<pointVect.size(); i++)
	{
		tt= pointVect[i];
		bbox += *tt;
	}

	bbox.valid=true;
}

void Model::need_bsphere()
{
	if(pointMap.size()==0) //没有面片
		return;

	Miniball<3,float> mb;
	//for (int i=faceStart;i<faceEnd;i++)
	//	for(int j=0;j<3;j++)
	//		mb.check_in(scene->points[scene->faces[i]->v[j]]);
	map<int,point>::iterator it;
	for(it=pointMap.begin();it!=pointMap.end();it++)
		mb.check_in(it->second);
	mb.build();
	bsphere.center=mb.center();
	bsphere.r=sqrt(mb.squared_radius());
	bsphere.valid=true;
}

void Model::need_bsphere_newInseartOBJ()
{
	if(pointVect.size()==0) //没有面片
		return;

	Miniball<3,float> mb;

	point* tt;
	for (int i = 0 ; i<pointVect.size(); i++)
	{
		tt= pointVect[i];
		mb.check_in(*tt);
	}

	mb.build();
	bsphere.center=mb.center();
	bsphere.r=sqrt(mb.squared_radius());
	bsphere.valid=true;
}

void Model::DrawBbox()
{
	//if(mesh == NULL)
	//{
	//	ReadModel(scene->scenePath);
	//}
	//DrawTrimesh();
	////if(pointNumber == 0)
	////{
	////	CalculateArea();
	////	GeneratePoints(1000);
	////}
	////DrawPoints();
	//if(mesh != NULL) return;
	//
	glPushMatrix();

	glMultMatrixd(ModelMatrix);
	glColor3f(1.0,0,0);
	glBegin(GL_LINE_STRIP);
	glVertex3f(bbox.min[0],bbox.max[1],bbox.min[2]);
	glVertex3f(bbox.min[0],bbox.min[1],bbox.min[2]);
	glVertex3f(bbox.max[0],bbox.min[1],bbox.min[2]);
	glVertex3f(bbox.max[0],bbox.max[1],bbox.min[2]);
	glVertex3f(bbox.min[0],bbox.max[1],bbox.min[2]);
	glEnd();

	glBegin(GL_LINE_STRIP);
	glVertex3f(bbox.max[0],bbox.min[1],bbox.min[2]);
	glVertex3f(bbox.max[0],bbox.max[1],bbox.min[2]);
	glVertex3f(bbox.max[0],bbox.max[1],bbox.max[2]);
	glVertex3f(bbox.max[0],bbox.min[1],bbox.max[2]);
	glVertex3f(bbox.max[0],bbox.min[1],bbox.min[2]);
	glEnd();

	glBegin(GL_LINE_STRIP);
	glVertex3f(bbox.max[0],bbox.max[1],bbox.max[2]);
	glVertex3f(bbox.max[0],bbox.min[1],bbox.max[2]);
	glVertex3f(bbox.min[0],bbox.min[1],bbox.max[2]);
	glVertex3f(bbox.min[0],bbox.max[1],bbox.max[2]);
	glVertex3f(bbox.max[0],bbox.max[1],bbox.max[2]);
	glEnd();

	glBegin(GL_LINE_STRIP);
	glVertex3f(bbox.min[0],bbox.max[1],bbox.min[2]);
	glVertex3f(bbox.min[0],bbox.min[1],bbox.min[2]);
	glVertex3f(bbox.min[0],bbox.min[1],bbox.max[2]);
	glVertex3f(bbox.min[0],bbox.max[1],bbox.max[2]);
	glVertex3f(bbox.min[0],bbox.max[1],bbox.min[2]);
	glEnd();
	glPopMatrix();
}

void Model::CalculateArea()
{
	if(faceAreaSum != NULL)
	{
		delete []faceAreaSum;
	}
	/*int totalTriangle = faceEnd - faceStart;
	faceAreaSum = new GLfloat[totalTriangle];
	GLfloat currentSum = 0;
	for(int i = 0;i<totalTriangle;i++)
	{
	Face* face = scene->faces[i+faceStart];
	point a = scene->points[face->v[0]] - scene->points[face->v[1]];
	point b = scene->points[face->v[2]] - scene->points[face->v[1]];
	currentSum += len(point(a[1]*b[2] - a[2]*b[1], a[2]*b[0] - a[0]*b[2], a[0]*b[1] - a[1]*b[0]))/2;
	faceAreaSum[i] = currentSum;
	}*/
	int totalTriangle = mesh->faces.size();
	faceAreaSum = new GLfloat[totalTriangle];
	GLfloat currentSum = 0;
	for(int i = 0;i<totalTriangle;i++)
	{
		TriMesh::Face face = mesh->faces[i];
		point a = mesh->vertices[face[0]] - mesh->vertices[face[1]];
		point b = mesh->vertices[face[2]] - mesh->vertices[face[1]];
		currentSum += len(point(a[1]*b[2] - a[2]*b[1], a[2]*b[0] - a[0]*b[2], a[0]*b[1] - a[1]*b[0]))/2;
		faceAreaSum[i] = currentSum;
	}
}

void Model::GeneratePoints(int total)
{
	if(samplePoints != NULL)
	{
		delete []samplePoints;
	}
	pointNumber = total;
	samplePoints = new point[total];
	int totalTriangle = mesh->faces.size();
	InitRand();
	GLfloat totalArea = faceAreaSum[totalTriangle - 1];
	for(int i=0;i<pointNumber;i++)
	{
		GLfloat randArea = totalArea * GetRand(1);
		//
		int down = 0;
		int up = totalTriangle-1;
		int current = -1;
		while(true)
		{
			current = (up + down) / 2;
			GLfloat curArea = faceAreaSum[current];
			if(curArea == randArea)
			{
				current++;
				break;
			}
			else if(curArea > randArea)
			{
				up = current;
				if(up == down)
				{
					break;
				}
			}
			else
			{
				down = current + 1;
				if(down == up)
				{
					current++;
					break;
				}
			}
		}
		//
		float r1 = sqrt(GetRand(1));
		float r2 = GetRand(1);
		float A = 1 - r1;
		float B = r1 * (1 - r2);
		float C = r1 * r2;
		TriMesh::Face face = mesh->faces[current];
		point pA = mesh->vertices[face[0]];
		point pB = mesh->vertices[face[1]];
		point pC = mesh->vertices[face[2]];
		for(int j=0;j<3;j++)
		{
			samplePoints[i][j] = A*pA[j] + B*pB[j] + C*pC[j];
		}
	}
}

void Model::DrawPoints()
{
	glPushMatrix();
	glTranslated(tx,ty,tz);
	glRotated(yangle,0.0,1.0,0.0);
	glRotated(xangle,1.0,0.0,0.0);
	glScalef(scaled,scaled,scaled);
	glTranslatef(-bsphere.center[0],-bsphere.center[1],-bsphere.center[2]);

	glGetDoublev(GL_MODELVIEW_MATRIX,ModelMatrix);
	// load名称，方便拾取
	glLoadName(scene->ModelMap[this->name]);
	glColor3b(255,0,0);
	glBegin(GL_POINTS);
	for (int i=0;i<pointNumber;i++)
	{
		glVertex3f(samplePoints[i][0],samplePoints[i][1],samplePoints[i][2]);
	}
	glEnd();
	glPopMatrix();
	glFlush();
}

void Model::SaveSceneModel()
{
	string dir=scene->dirPath+name+".obj";
	ofstream out(dir);
	
	out<<"# Alias OBJ Model File\n";
	
	vector<Face*> &faces=scene->faces;
	point* points=scene->points;
	vnormal *vnormals=scene->vnormals;
	int index=0;

#ifdef DefMaterial
	out<<"mtllib "<<scene->mtlPath<<"\n";

	vtexture* vtextures=scene->vtextures;
	Material* materials=scene->materials;

	int mtlSPos=0; // 找到对应纹理的位置
	for (;mtlSPos<scene->usemtlSlice.size();mtlSPos++)
		if (faceStart<scene->usemtlSlice[mtlSPos])
			break;
	int mtlEPos=0; // 找到对应纹理的位置
	for (;mtlEPos<scene->usemtlSlice.size();mtlEPos++)
		if (faceEnd<scene->usemtlSlice[mtlEPos])
			break;
	mtlEPos++;
	int count=faceStart;
	for (int i=mtlSPos;i<mtlEPos;i++)
	{
		out<<"usemtl "<<materials[scene->mtlMark[i-1]].name<<"\n";

		if (faceEnd<scene->usemtlSlice[i])
		{
			for(;count<faceEnd;count++)
			{
				Face* face=faces[count];
				for(int j=0;j<3;j++)
				{
					// v
					out<<"v "<<points[face->v[j]][0]<<" "<<points[face->v[j]][1]<<" "<<points[face->v[j]][2]<<"\n";
					out<<"vn "<<vnormals[face->vn[j]][0]<<" "<<vnormals[face->vn[j]][1]<<" "<<vnormals[face->vn[j]][2]<<"\n";
					out<<"vt "<<vtextures[face->vt[j]][0]<<" "<<vtextures[face->vt[j]][1]<<"\n";
					index++;
				}
				// 对应f v/vt/vn的格式，也可以同下面的处理直接默认，为了后续操作能够重用，按照标准格式进行
				out<<"f "<<(index-2)<<"/"<<(index-2)<<"/"<<(index-2)<<" "<<(index-1)<<"/"<<(index-1)<<"/"<<(index-1)<<" "<<index<<"/"<<index<<"/"<<index<<"\n";
			}
			break;
		}

		for(int m=count;m<scene->usemtlSlice[i];m++)
		{
			Face* face=faces[m];
			for(int j=0;j<3;j++)
			{
				// v
				out<<"v "<<points[face->v[j]][0]<<" "<<points[face->v[j]][1]<<" "<<points[face->v[j]][2]<<"\n";
				out<<"vn "<<vnormals[face->vn[j]][0]<<" "<<vnormals[face->vn[j]][1]<<" "<<vnormals[face->vn[j]][2]<<"\n";
				out<<"vt "<<vtextures[face->vt[j]][0]<<" "<<vtextures[face->vt[j]][1]<<"\n";
				index++;
			}
			// 对应f v/vt/vn的格式，也可以同下面的处理直接默认，为了后续操作能够重用，按照标准格式进行
			out<<"f "<<(index-2)<<"/"<<(index-2)<<"/"<<(index-2)<<" "<<(index-1)<<"/"<<(index-1)<<"/"<<(index-1)<<" "<<index<<"/"<<index<<"/"<<index<<"\n";
			count++;
		}
	}

#endif

#ifndef DefMaterial
	for(int i=faceStart;i<faceEnd;i++)
	{
		Face* face=faces[i];
		for(int j=0;j<3;j++)
		{
			// v
			out<<"v "<<points[face->v[j]][0]<<" "<<points[face->v[j]][1]<<" "<<points[face->v[j]][2]<<"\n";
			out<<"vn "<<vnormals[face->vn[j]][0]<<" "<<vnormals[face->vn[j]][1]<<" "<<vnormals[face->vn[j]][2]<<"\n";
			index++;
		}
		out<<"f "<<(index-2)<<" "<<(index-1)<<" "<<index<<"\n";
		// 默认一个点配一个法向，因此，无需再f后直接指定
	}
#endif
	out.close();
}

void Model::ReadModel( string filename )
{
	mesh=TriMesh::read(filename.c_str());
}

void Model::PCAOperation()
{
	pca_rotate(mesh);
	mesh->need_bbox();
	mesh->need_bsphere();
	do
	{
		scale(mesh,1/mesh->bsphere.r);
		mesh->need_bbox();
		mesh->need_bsphere();
		trans(mesh,-mesh->bsphere.center);
		mesh->need_bbox();
		mesh->need_bsphere();
	}while(mesh->bsphere.r > 1.0f || mesh->bsphere.r < 0.999f || len(mesh->bsphere.center) > 0.001f);
}

void Model::DrawTrimesh()
{
	glPushMatrix();

	//glTranslated(tx,ty,tz);
	//glRotated(yangle,0.0,1.0,0.0);
	//glRotated(xangle,1.0,0.0,0.0);
	//glScalef(scale,scale,scale);

	glTranslated(tx,ty,tz);
	glRotated(yangle,0.0,1.0,0.0);
	glScalef(scaled,scaled,scaled);

	glTranslatef(-mesh->bsphere.center[0],-mesh->bsphere.center[1],-mesh->bsphere.center[2]);
	glBegin(GL_TRIANGLES);
	for(int i=0;i<mesh->faces.size();i++)
	{
		for (int j=0;j<2;j++)
		{
			glNormal3f(mesh->normals[mesh->faces[i][j]][0],mesh->normals[mesh->faces[i][j]][1],mesh->normals[mesh->faces[i][j]][2]);
			glVertex3f(mesh->vertices[mesh->faces[i][j]][0],mesh->vertices[mesh->faces[i][j]][1],mesh->vertices[mesh->faces[i][j]][2]);
		}

	}
	glEnd();
	glPopMatrix();
	glFlush();
}

void Model::InitRand()
{
	seed((unsigned)time(NULL));
    //qRand = gsl_qrng_alloc (gsl_qrng_sobol, 1);
}

float Model::GetRand(int type)
{
	if(type == 1)
	{
		return randf_co();
	}
	if(type == 2)
	{
		double v[1];
        //gsl_qrng_get(qRand, v);
		return v[0];
	}
}

void Model::EndRand()
{
    //gsl_qrng_free(qRand);
}


//自己写的read obj模型的方法，trimesh无法读取此类模型的obj文件（单个从google warehouse中download的模型）
void Model::ReadOBJ(string filePath)
{
	ifstream input(filePath);
	if (!input)
	{
		cerr<<"error: unable to open input file: "<<input<<endl;
		return ;
	}

	string buffer;
	string objName;
	int countV=0,countN=0;

	//用于临时保存模型的point和normal，以便后续将其保存至face之中
	//vector<point> tempPointVect;
	//vector<vnormal> tempNormalVect;
	point* tp;  //临时变量
	vnormal* tv; //临时变量

	// 用于拆分多个顶点构成的face
	vector<int> thisv;
	vector<int> thisvn;

#ifdef DefMaterial
	vector<int> thisvt;
	int countT=0;
#endif
	
	//// 解析 group标签
	//string curModelName;  // 当前模型的名称Name

	while (!input.eof())
	{
		getline(input,buffer);
		istringstream line(buffer);
		string temp;
		string f1,f2,f3;

		if(buffer.substr(0,3) == "vn ")  {
			line>> temp>> f1>> f2>> f3;
			//vnormals[countN][0] = atof(f1.c_str());
			//vnormals[countN][1] = atof(f2.c_str());
			//vnormals[countN][2] = atof(f3.c_str());
			//++countN;
			tv = new vnormal;
			(*tv)[0] =  atof(f1.c_str());
			(*tv)[1] =  atof(f2.c_str());
			(*tv)[2] =  atof(f3.c_str());
			normalVect.push_back(tv);
		}
		else if(buffer.substr(0,2) == "v ")  {
			line>> temp>> f1>> f2>> f3;
			/*points[countV][0] = atof(f1.c_str());
			points[countV][1] = atof(f2.c_str());
			points[countV][2] = atof(f3.c_str());
			++countV;*/
			tp = new point;
			(*tp)[0] = atof(f1.c_str());
			(*tp)[1] = atof(f2.c_str());
			(*tp)[2] = atof(f3.c_str());
			pointVect.push_back(tp);
		}
#ifndef DefMaterial
		else if (buffer.substr(0,2)=="f ")
		{
			thisv.clear();
			thisvn.clear();

			const char *c=buffer.c_str();
			while (1) {
				while (*c && *c != '\n' && !isspace(*c))
					c++;
				while (*c && isspace(*c))
					c++;
				int v,vn,vt;
				if (sscanf(c, "%d/%d/%d", &v,&vt,&vn) != 3)
					break;
				v--;
				vn--;
				thisv.push_back(v);
				thisvn.push_back(vn);
			}
			//tess(thisv,thisvn);
			ModelTess(thisv,thisvn);
		}
#endif
#ifdef DefMaterial
		else if (buffer.substr(0,2)=="f ")
		{
			thisv.clear();
			thisvn.clear();
			thisvt.clear();

			const char *c=buffer.c_str();
			while (1) {
				while (*c && *c != '\n' && !isspace(*c))
					c++;
				while (*c && isspace(*c))
					c++;
				int v,vn,vt;
				if (sscanf(c, "%d/%d/%d", &v,&vt,&vn) != 3)
					break;
				v--;
				vt--;
				vn--;
				thisv.push_back(v);
				thisvn.push_back(vn);
				thisvt.push_back(vt);
			}
			tess(thisv,thisvt,thisvn);
		}
		else if(buffer.substr(0,3) == "vt ")  {
			line >> temp >> f1 >> f2;
			vtextures[countT][0] = atof(f1.c_str());
			vtextures[countT][1] = atof(f2.c_str());
			++countT;
		}
		else if (buffer.substr(0,7)=="usemtl ")
		{
			string name; // usemtl name;
			line>>temp>>name;
			usemtlSlice.push_back(faces.size());

			int sc=0;
			for (;sc<MtlSize;sc++)
			{
				if(name==materials[sc].name)
					break;
			}
			mtlMark.push_back(sc);
		}
		else if (buffer.substr(0,7)=="mtllib ")
		{
			// temp=mtllib  f1=纹理地址
			line>>temp>>mtlPath; 
			LoadMtl(mtlPath);
		}
#endif // DefMaterial
		//else if (buffer.substr(0,2)=="g ")
		//{
		//	line>>temp>>f1>>f2;  // 清除g Mesh的影响，f2包含物体名称
		//	// 处理Model Name
		//	vector<string> elements;
		//	split(f2,'_',elements);
		//	int eleSize=elements.size();
		//	string temp=elements[eleSize-1]; // 最后一个肯定是物体标签
		//	if (temp==curModelName)
		//		continue;
		//	else
		//	{
		//		//				sceneModels[sceneModels.size()-1].faceEnd=faces.size();
		//		Model *model=new Model;
		//		model->faceStart=faces.size();
		//		model->name=curModelName=temp;
		//		model->tag=FindModelTag(temp);
		//		ModelMap.insert(pair<string,int>(model->name,(sceneModels.size())));
		//		sceneModels.push_back(model);
		//	}
		//}
	}

#ifdef DefMaterial
	usemtlSlice.push_back(INT_MAX);
#endif

	input.close();
	//CompleteModelSetting();
}

//改写的trimesh方法tess
void Model::ModelTess(const vector<int> &thisv,const vector<int> &thisvn)
{
	if(thisv.size()<3)
		return;
	if (thisv.size()==3)
	{
		Face* face=new Face;
		// 顶点
		face->v[0]=thisv[0];
		face->v[1]=thisv[1];
		face->v[2]=thisv[2];

		face->vn[0]=thisvn[0];
		face->vn[1]=thisvn[1];
		face->vn[2]=thisvn[2];

		//faces.push_back(face);
		modelFaces.push_back(face);

	/*	sceneModels[sceneModels.size()-1]->pointMap.insert(pair<int,point>(face->v[0],points[face->v[0]]));
		sceneModels[sceneModels.size()-1]->pointMap.insert(pair<int,point>(face->v[1],points[face->v[1]]));
		sceneModels[sceneModels.size()-1]->pointMap.insert(pair<int,point>(face->v[2],points[face->v[2]]));*/

		return;
	}

	for(int i=2;i<thisv.size();i++)
	{
		Face *face=new Face;
		// 顶点
		face->v[0]=thisv[0];
		face->v[1]=thisv[i-1];
		face->v[2]=thisv[i];

		// 法向
		face->vn[0]=thisvn[0];
		face->vn[1]=thisvn[i-1];
		face->vn[2]=thisvn[i];

		//faces.push_back(face);
		modelFaces.push_back(face);
		
		/*sceneModels[sceneModels.size()-1]->pointMap.insert(pair<int,point>(face->v[0],points[face->v[0]]));
		sceneModels[sceneModels.size()-1]->pointMap.insert(pair<int,point>(face->v[1],points[face->v[1]]));
		sceneModels[sceneModels.size()-1]->pointMap.insert(pair<int,point>(face->v[2],points[face->v[2]]));*/
	}
}