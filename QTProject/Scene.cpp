#include "Scene.h"
#include "BaseFunction.h"
#include <fstream>
#include <iostream>
#include <sstream>

Scene::Scene(QObject* parent) :QObject(parent)
{
	Vsize=Vnsize=Vtsize=MtlSize=TextureNum=modelSize=0;
	points=NULL;
	vnormals=NULL;
	vtextures=NULL;
	materials=NULL;
	relationTable=NULL;
}


Scene::~Scene(void)
{
	if (relationTable!=NULL)
	{
		for (int i=0;i<modelSize;i++)
			delete []relationTable[i];
		delete []relationTable;
	}

	if(points!=NULL)
		delete points;

	if(vtextures!=NULL)
		delete vtextures;

	if (materials!=NULL)
		delete[] materials;

	for (int i=0;i<modelSize;i++)
		delete sceneModels[i];

	for (int i=0; i<faces.size();i++)
		delete faces[i];
}

// 读入场景，输入为场景地址
bool Scene::readScene(const char* filename )
{
	bool flag=readHelper(filename);
	need_bbox();
	need_bsphere();
	//mvisible=new Byte[modelSize];
	//memset(mvisible,1,modelSize);
	return flag;
}

// 确定文件对应类型
bool Scene::readHelper(const char* filename)
{
	if (!filename || *filename=='\0')
		return false;

	scenePath=filename;
	ExtractBasePath(filename);

	ifstream input(filename);
	if (!input)
	{
		cerr<<"Error: unable to open input file: "<<input<<endl;
		return false;
	}

	string buffer;
	while(!input.eof())
	{
		getline(input,buffer);

		if (buffer.substr(0,2)=="vn")
			Vnsize++;
		else if(buffer.substr(0,2)=="vt")
			Vtsize++;
		else if(buffer.substr(0,1)=="v")
			Vsize++;
		else if(buffer.substr(0,6)=="usemtl")
			TextureNum++;
	}

	input.close();
	points=new point[Vsize];
	vnormals=new vnormal[Vnsize];
	vtextures=new vtexture[Vtsize];

	return read_obj(filename);
}

bool Scene::read_obj(const char* filename)
{
	ifstream input(filename);
	if (!input)
	{
		cerr<<"error: unable to open input file: "<<input<<endl;
		return false;
	}

	string buffer;
	string objName;
	int countV=0,countN=0,countT=0;
	// 用于拆分多个顶点构成的face
	vector<int> thisv;
	vector<int> thisvn;
	vector<int> thisvt;

	// 解析 group标签
	string curModelName;  // 当前模型的名称Name

	while (!input.eof())
	{
		getline(input,buffer);
		istringstream line(buffer);
		string temp;
		string f1,f2,f3;

		if(buffer.substr(0,2) == "vn")  {
			line>> temp>> f1>> f2>> f3;
			vnormals[countN][0] = atof(f1.c_str());
			vnormals[countN][1] = atof(f2.c_str());
			vnormals[countN][2] = atof(f3.c_str());
			++countN;
		}
		else if(buffer.substr(0,2) == "vt")  {
			line >> temp >> f1 >> f2;
			vtextures[countT][0] = atof(f1.c_str());
			vtextures[countT][1] = atof(f2.c_str());
			++countT;
		}				
		else if(buffer.substr(0,1) == "v")  {
			line>> temp>> f1>> f2>> f3;
			points[countV][0] = atof(f1.c_str());
			points[countV][1] = atof(f2.c_str());
			points[countV][2] = atof(f3.c_str());
			++countV;
		}
		else if (buffer.substr(0,1)=="f")
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
#ifdef DefMaterial
		else if (buffer.substr(0,6)=="usemtl")
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
		else if (buffer.substr(0,6)=="mtllib")
		{
			// temp=mtllib  f1=纹理地址
			line>>temp>>f1; 
			LoadMtl(f1);
		}
#endif // DefMaterial
		else if (buffer.substr(0,1)=="g")
		{
			line>>temp>>f1>>f2;  // 清除g Mesh的影响，f2包含物体名称
			// 处理Model Name
			vector<string> elements;
			split(f2,'_',elements);
			int eleSize=elements.size();
			string temp=elements[eleSize-1]; // 最后一个肯定是物体标签
			if (temp==curModelName)
				continue;
			else
			{
//				sceneModels[sceneModels.size()-1].faceEnd=faces.size();
				Model *model=new Model;
				model->faceStart=faces.size();
				model->name=curModelName=temp;
				model->tag=FindModelTag(temp);
				ModelMap.insert(pair<string,int>(model->name,(sceneModels.size())));
				sceneModels.push_back(model);
			}
		}
	}
	input.close();
	CompleteModelSetting();

	// 解析场景物体间的关系
	input.open(filename);
	input.clear();
	string curModel,tempModel;
	while (!input.eof())
	{
		getline(input,buffer);
		istringstream line(buffer);
		string tempG,tempMesh,info;
		if (buffer.substr(0,1)=="g")
		{
			int infoNum=CountSpaceNum(buffer)-2;
			line>>tempG>>tempMesh>>info;
			//string *info=new string[infoNum];
			// 获得curModel
			vector<string> tempStrings;
			split(info,'_',tempStrings);
			tempModel=tempStrings[(tempStrings.size()-1)];

			if (curModel==tempModel)
				continue; // 已经解析过了
			else
			{
				curModel=tempModel;
				for (int i=0;i<infoNum;i++)
				{
					vector<string> relationStrings;
					split(info,'_',relationStrings);
					// 关系都是每两个一组
					if (relationStrings.size()>1)
					{
						for(int i=0;i<relationStrings.size();i+=2)
						{
							string nameKey;
							if (relationStrings[i]=="group")
							{
								nameKey=relationStrings[i]+relationStrings[i+1];
								if (RelationMap.find(nameKey)==RelationMap.end())
								{
									vector<int> relations;
									relations.push_back(ModelMap[relationStrings[i+1]]); //push center
									if (curModel!=relationStrings[i+1])
									{
										relations.push_back(ModelMap[curModel]);
									}
									RelationMap.insert(pair<string,vector<int>>(nameKey,relations));
								}
								else
								{
									vector<int> relations=RelationMap[nameKey];
									if(relations[0]==ModelMap[curModel])
										continue;
									relations.push_back(ModelMap[curModel]);
									RelationMap[nameKey]=relations;
								}
							}
							else if (relationStrings[i]=="identical")
							{
								nameKey=relationStrings[i]+relationStrings[i+1];
								if (RelationMap.find(nameKey)==RelationMap.end())
								{
									vector<int> relations;
									relations.push_back(ModelMap[curModel]); //push center
									RelationMap.insert(pair<string,vector<int>>(nameKey,relations));
								}
								else
								{
									vector<int> relations=RelationMap[nameKey];
									relations.push_back(ModelMap[curModel]);
									RelationMap[nameKey]=relations;
								}
							}
							else if (relationStrings[i]=="support")
							{
								nameKey=relationStrings[i]+relationStrings[i+1];
								if (RelationMap.find(nameKey)==RelationMap.end())
								{
									vector<int> relations;
									if (relationStrings[i+1]=="Wall")
										relations.push_back(-1);
									else if (relationStrings[i+1]=="onWall")
										relations.push_back(-2);
									else if(relationStrings[i+1]=="hangWall")
										relations.push_back(-3);
									else
										relations.push_back(ModelMap[relationStrings[i+1]]);

									relations.push_back(ModelMap[curModel]); //push center
									RelationMap.insert(pair<string,vector<int>>(nameKey,relations));
								}
								else
								{
									vector<int> relations=RelationMap[nameKey];
									if(ModelMap[curModel]==relations[0])
										continue;
									relations.push_back(ModelMap[curModel]);
									RelationMap[nameKey]=relations;
								}
							}
							else if (relationStrings[i]=="vcontact")
							{
								nameKey=relationStrings[i]+relationStrings[i+1];
								if (RelationMap.find(nameKey)==RelationMap.end())
								{
									vector<int> relations;
									relations.push_back(ModelMap[relationStrings[i+1]]);
									relations.push_back(ModelMap[curModel]); //push center
									RelationMap.insert(pair<string,vector<int>>(nameKey,relations));
								}
								else
								{
									//vector<int> relations=RelationMap[nameKey];
									//relations.push_back(ModelMap[curModel]);
									//RelationMap[nameKey]=relations;
									//简单的双向关系，如果进入到这里表示出现错误
									cerr<<"vcontact关系定义错误"<<endl;
									return false;
								}
							}
							else if (relationStrings[i]=="enclosure")
							{
								nameKey=relationStrings[i]+relationStrings[i+1];
								if (RelationMap.find(nameKey)==RelationMap.end())
								{
									vector<int> relations;
									relations.push_back(ModelMap[relationStrings[i+1]]);
									relations.push_back(ModelMap[curModel]);
									RelationMap.insert(pair<string,vector<int>>(nameKey,relations));
								}
								else
								{
									vector<int> relations=RelationMap[nameKey];
									relations.push_back(ModelMap[curModel]);
									RelationMap[nameKey]=relations;
								}
							}
						}
					}
					line>>info;
				}
			}
		}
	}

	return true;
}

void Scene::ExtractBasePath( const char* filename )
{
	int pos=scenePath.find_last_of("\\");
	dirPath=scenePath.substr(0,pos+1);
}

void Scene::tess( const vector<int> &thisv,const vector<int> &thisvt,const vector<int> &thisvn )
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

		face->vt[0]=thisvt[0];
		face->vt[1]=thisvt[1];
		face->vt[2]=thisvt[2];

		face->vn[0]=thisvn[0];
		face->vn[1]=thisvn[1];
		face->vn[2]=thisvn[2];

		faces.push_back(face);

		sceneModels[sceneModels.size()-1]->pointMap.insert(pair<int,point>(face->v[0],points[face->v[0]]));
		sceneModels[sceneModels.size()-1]->pointMap.insert(pair<int,point>(face->v[1],points[face->v[1]]));
		sceneModels[sceneModels.size()-1]->pointMap.insert(pair<int,point>(face->v[2],points[face->v[2]]));

		return;
	}
	for(int i=2;i<thisv.size();i++)
	{
		Face *face=new Face;
		// 顶点
		face->v[0]=thisv[0];
		face->v[1]=thisv[i-1];
		face->v[2]=thisv[i];
		// 纹理
		face->vt[0]=thisvt[0];
		face->vt[1]=thisvt[i-1];
		face->vt[2]=thisvt[i];
		// 法向
		face->vn[0]=thisvn[0];
		face->vn[1]=thisvn[i-1];
		face->vn[2]=thisvn[i];

		faces.push_back(face);

		sceneModels[sceneModels.size()-1]->pointMap.insert(pair<int,point>(face->v[0],points[face->v[0]]));
		sceneModels[sceneModels.size()-1]->pointMap.insert(pair<int,point>(face->v[1],points[face->v[1]]));
		sceneModels[sceneModels.size()-1]->pointMap.insert(pair<int,point>(face->v[2],points[face->v[2]]));
	}
}

void Scene::LoadMtl( string mtlPath )
{
	string path=dirPath+mtlPath;
	string mtlbuffer;
	ifstream mtlin(path);
	if (!mtlin)
	{
		cerr<<"Unable to open mtl file"<<endl;
		return;
	}
	while(!mtlin.eof())
	{
		getline(mtlin,mtlbuffer);
		if (mtlbuffer.substr(0,6)=="newmtl")
			MtlSize++;
	}
	materials=new Material[MtlSize];
	mtlin.close();

	mtlin.open(path);
	mtlin.clear();
	string temp;
	string a,b,s;
	int countMtl=0;
	while(!mtlin.eof())
	{
		getline(mtlin,mtlbuffer);
		istringstream line(mtlbuffer);
		if (mtlbuffer=="")
			continue;
		else if (mtlbuffer.substr(0,6)=="newmtl")
		{
			line>>temp>>materials[countMtl].name;
			countMtl++;
		}
		else if (mtlbuffer.substr(0,2)=="Ka")
		{
			line>>temp>>a>>b>>s;
			materials[countMtl-1].Ka[0]=atof(a.c_str());
			materials[countMtl-1].Ka[1]=atof(b.c_str());
			materials[countMtl-1].Ka[2]=atof(s.c_str());
			materials[countMtl-1].Ka[3]=1.0f;
		}
		else if (mtlbuffer.substr(0,2)=="Kd")
		{
			line>>temp>>a>>b>>s;
			materials[countMtl-1].Kd[0]=atof(a.c_str());
			materials[countMtl-1].Kd[1]=atof(b.c_str());
			materials[countMtl-1].Kd[2]=atof(s.c_str());
			materials[countMtl-1].Kd[3]=1.0f;
		}
		else if (mtlbuffer.substr(0,2)=="Ks")
		{
			line>>temp>>a>>b>>s;
			materials[countMtl-1].Ks[0]=atof(a.c_str());
			materials[countMtl-1].Ks[1]=atof(b.c_str());
			materials[countMtl-1].Ks[2]=atof(s.c_str());
			materials[countMtl-1].Ks[3]=1.0f;
		}
		else if (mtlbuffer.substr(0,6)=="map_Kd")
		{
			line>>temp>>materials[countMtl-1].picPath;
		}
	}
	mtlin.close();
}

std::string Scene::FindModelTag( string name )
{
	int pot=name.find_first_of('0');
	return name.substr(0,pot);
}

void Scene::CompleteModelSetting()
{
	modelSize=sceneModels.size();
	for (int i=0;i<modelSize-1;i++)
	{
		sceneModels[i]->faceEnd=sceneModels[i+1]->faceStart;
		sceneModels[i]->scene=this;
		sceneModels[i]->need_bbox();
		sceneModels[i]->need_bsphere();
		sceneModels[i]->tx=sceneModels[i]->bsphere.center[0];
		sceneModels[i]->ty=sceneModels[i]->bsphere.center[1];
		sceneModels[i]->tz=sceneModels[i]->bsphere.center[2];
		//sceneModels[i]->ball.arcball_setCenter(sceneModels[i]->bsphere.center);
	}
	sceneModels[modelSize-1]->faceEnd=faces.size();
	sceneModels[modelSize-1]->scene=this;
	sceneModels[modelSize-1]->need_bbox();
	sceneModels[modelSize-1]->need_bsphere();
	sceneModels[modelSize-1]->tx=sceneModels[modelSize-1]->bsphere.center[0];
	sceneModels[modelSize-1]->ty=sceneModels[modelSize-1]->bsphere.center[1];
	sceneModels[modelSize-1]->tz=sceneModels[modelSize-1]->bsphere.center[2];
	//sceneModels[modelSize-1]->ball.arcball_setCenter(sceneModels[modelSize-1]->bsphere.center);
}

void Scene::BuildRelationTable()
{
	relationTable=new int*[modelSize];
	for (int i=0;i<modelSize;i++)
		relationTable[i]=new int[modelSize];

	memset(relationTable,0,modelSize*modelSize*4);
	map<string,vector<int>>::iterator it;
	for (it=RelationMap.begin();it!=RelationMap.end();it++)
	{
		if (it->first=="group")
		{
			int center=it->second[0];
			for (int i=1;i<it->second.size();i++)
			{
				relationTable[center][it->second[i]]=1;
				relationTable[it->second[i]][center]=-1;
			}
		}
		else if (it->first=="identical")
		{
			int size=it->second.size();  // size可能较大
			for (int i=0;i<(size-1);i++)
			{
				for(int j=i+1;j<size;j++)
				{
					relationTable[it->second[i]][it->second[j]]=6;
					relationTable[it->second[j]][it->second[i]]=6;
				}
			}
		}
		else if (it->first=="support")
		{
			int size=it->second.size();  // size可能较大
			int support=it->second[0];
			if (support>0)
				for (int i=1;i<size;i++)
				{
					relationTable[support][i]=5;
					relationTable[i][support]=-5;
				}
			// Wall
			else if (support==-1)
			{
				int wall=ModelMap["Wall"];
				for (int i=1;i<size;i++)
				{
					relationTable[wall][i]=4;
					relationTable[i][wall]=-4;
				}
			}
			// onWall
			else if (support==-2)
			{
				int wall=ModelMap["Wall"];
				for (int i=1;i<size;i++)
				{
					relationTable[wall][i]=2;
					relationTable[i][wall]=-2;
				}
			}
			// hangWall
			else if (support==-3)
			{
				int wall=ModelMap["Wall"];
				for (int i=1;i<size;i++)
				{
					relationTable[wall][i]=3;
					relationTable[i][wall]=-3;
				}
			}
		}
		else if (it->first=="vcontact")
		{
			relationTable[it->second[0]][it->second[1]]=7;
			relationTable[it->second[1]][it->second[0]]=7;
		}
		else if (it->first=="enclosure")
		{
			int enclosure=it->second[0];
			for (int i=1;i<it->second.size();i++)
			{
				relationTable[enclosure][it->second[i]]=8;
				relationTable[it->second[i]][enclosure]=-8;
			}
		}
	}
}

void Scene::DrawScene()
{
	for (int i=0;i<modelSize;i++)
	{
		if(sceneModels[i]->visible)
			sceneModels[i]->DrawModel();
	}
	glFlush();
}

void Scene::need_bbox()
{
	if (Vsize==0) // 没有顶点
		return;

	for (int i = 0; i < Vsize; i++)
		bbox += points[i];
	bbox.valid=true;
	//dprintf("Done.\n  x = %g .. %g, y = %g .. %g, z = %g .. %g\n",
	//	bbox.min[0], bbox.max[0], bbox.min[1],
	//	bbox.max[1], bbox.min[2], bbox.max[2]);
}

void Scene::DrawTest()
{
	glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//glTranslatef(-1,-1,-1);
	glTranslatef(0,0,-10);
	glBegin(GL_POLYGON);
	glVertex3f(2.0f,2.0f,1.0);
	glVertex3f(2.0f,0.0f,1.0);
	glVertex3f(0.0f,0.0f,1.0f);
	glVertex3f(0.0f,2.0f,1.0f);
	glEnd();
	//glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
	//static const GLfloat p1[3]={0.0,-1.0,2.0};
	//static const GLfloat p2[3]={1.73205001,-1.0,-1.0};
	//static const GLfloat p3[3]={-1.73205001,-1.0,-1.0};
	//static const GLfloat p4[3]={0.0,2.0,0.0};

	//static const GLfloat* const coords[4][3]={
	//	{p1,p2,p3},{p1,p3,p4},{p1,p4,p2},{p2,p4,p3}
	//};

	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
	//glTranslatef(0,0,-10);

	//for (int i=0;i<4;i++)
	//{
	//	glLoadName(i);
	//	glBegin(GL_TRIANGLES);
	//	/*qglColor(faceColors[i]);*/
	//	glColor3b(255,0,0);
	//	for (int j=0;j<3;j++)
	//	{
	//		glVertex3f(coords[i][j][0],coords[i][j][1],coords[i][j][2]);
	//	}
	//	glEnd();
	//}
}

void Scene::need_bsphere()
{
	if (Vsize==0)
		return;

	Miniball<3,float> mb;
	mb.check_in(points,Vsize);
	mb.build();
	bsphere.center = mb.center();
	bsphere.r = sqrt(mb.squared_radius());
	bsphere.valid = true; 

	//dprintf("Done.\n  center = (%g, %g, %g), radius = %g\n",
	//	bsphere.center[0], bsphere.center[1],
	//	bsphere.center[2], bsphere.r);
}

void Scene::DrawSimpleScene()
{
	if(Vsize==0)
		return;

	glColor3b(255,255,255);

	glBegin(GL_TRIANGLES);
	for (int i=0;i<faces.size();i++)
	{
		Face* face=faces[i];
		for(int j=0;j<3;j++)
		{
			glNormal3f(vnormals[face->vn[j]][0],vnormals[face->vn[j]][1],vnormals[face->vn[j]][2]);
			glVertex3f(points[face->v[j]][0],points[face->v[j]][1],points[face->v[j]][2]);
		}
	}
	glEnd();
	glFlush();
}




