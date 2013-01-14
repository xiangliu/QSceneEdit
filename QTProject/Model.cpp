#include "Model.h"
#include <fstream>
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

void Model::need_bbox()
{
	if(pointMap.size()==0) //没有面片
		return;
	
	map<int,point>::iterator it;
	for(it=pointMap.begin();it!=pointMap.end();it++)
		bbox+=it->second;

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
