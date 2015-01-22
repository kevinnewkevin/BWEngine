#include "MeshLoaderB3D.h"
#include <stdio.h>
#include <iostream>
#include <stdarg.h> 
#include "renderer/Mesh.h"
#include "renderer/Joint.h"
using namespace std;

#define DEBUG_B3D


MeshLoaderB3D::MeshLoaderB3D() :_meshCount(0), m_RootJoint(nullptr), m_TotalFrame(0)
{
}

MeshLoaderB3D::~MeshLoaderB3D()
{
}

bool MeshLoaderB3D::loadMesh(const char* file)
{
	FILE* fp = fopen(file, "rb");
	if (fp == NULL)
		return false;

	readChunk(fp);
	
	int nB3DVersion = 0;
	fread(&nB3DVersion, sizeof(int), 1, fp);

	printTree("B3D");
	while( checkSize(fp) )
	{
		string t = readChunk(fp);
		if (t == "TEXS"){
			readTEXS(fp);
		}
		else if (t == "BRUS"){
			readBRUS(fp);
		}
		else if (t == "NODE"){
			readNODE(fp);
		}
		exitChunk(fp);
	}
	fclose(fp);

	return true;
}

string MeshLoaderB3D::readString(FILE* fp)
{
	string str;
	char c;

	while (c = fgetc(fp))
	{
		str += c;
		if (c=='/0')
			break;
	}
	return str;
}

float MeshLoaderB3D::readFloat(FILE* fp)
{
	float f;
	fread(&f, sizeof(float), 1, fp);
	return f;
}

int MeshLoaderB3D::readInt(FILE* fp)
{
	int i;
	fread(&i, sizeof(int), 1, fp);
	return i;
}

bool MeshLoaderB3D::readVRTS(FILE* fp)
{
	Mesh* mesh = new Mesh;
	const int max_tex_coords = 3;
	int flags, tex_coord_sets, tex_coord_set_size;

	flags = readInt(fp);
	tex_coord_sets = readInt(fp);
	tex_coord_set_size = readInt(fp);

	if (tex_coord_sets >= max_tex_coords || tex_coord_set_size >= 4) // Something is wrong
	{
		printf("tex_coord_sets or tex_coord_set_size too big");
		return false;
	}

	//------ Allocate Memory, for speed -----------//

	int numberOfReads = 3;
	bool hasNormal = false;
	bool hasVertexColors = false;
	if (flags & 1)
	{
		hasNormal = true;
		numberOfReads += 3;
	}
	if (flags & 2)
	{
		numberOfReads += 4;
		hasVertexColors=true;
	}

	numberOfReads += tex_coord_sets*tex_coord_set_size;
	unsigned int size = m_Stack[m_Stack.size() - 1] - ftell(fp);
	//106407 16800
	unsigned int numberofPosition = size / sizeof(float) ;
	numberofPosition /= numberOfReads;

	int idx = 0;
	while( checkSize(fp))
	{
		float position[3];
		float normal[3]={0.f, 0.f, 0.f};
		float color[4]={1.0f, 1.0f, 1.0f, 1.0f};

		position[0] = readFloat(fp);
		position[1] = readFloat(fp);
		position[2] = readFloat(fp);

		if (flags & 1)
		{
			readFloat(fp);
			readFloat(fp);
			readFloat(fp);
		}
		if (flags & 2)
		{
			readFloat(fp);
			readFloat(fp);
			readFloat(fp);
			readFloat(fp);
		}
		float u, v;
		for (int i = 0; i < tex_coord_sets; ++i)
		{
			//for (int j = 0; j < tex_coord_set_size; ++j)
			{
				u = readFloat(fp);
				v = 1.0f - readFloat(fp);
			}
		}
	
		mesh->_positions.push_back(Vec3(position[0], position[1], position[2]));
		mesh->_texCoords.push_back(Vec2(u, v));
		idx++;
	}

	_meshVec.push_back(mesh);
	_meshCount++;
	return true;
}

string MeshLoaderB3D::readChunk(FILE* fp)
{
	string s;
	for (int i = 0; i < 4;++i)
	{
		s += char(readByte(fp));
	}
	
	unsigned int size = readInt(fp);
	unsigned int pos = ftell(fp);
	m_Stack.push_back(pos + size);
	return s;
}

int MeshLoaderB3D::readByte(FILE* fp)
{
	int b;
	fread(&b, 1, 1, fp);
	return b;
}

bool MeshLoaderB3D::checkSize(FILE* fp)
{
	unsigned int pos = ftell(fp);
	unsigned int size = m_Stack[m_Stack.size() - 1];
	return size > pos;
}

void MeshLoaderB3D::exitChunk(FILE* fp)
{
	m_lCurNodePos = m_Stack.back();
	m_Stack.pop_back();
}

void MeshLoaderB3D::readTEXS(FILE* fp)
{
	while (checkSize(fp))
	{
		printTree("read texs \n");
		SB3dTexture tex;
		tex.TextureName = readString(fp);
		tex.Flags = readInt(fp);
		tex.Blend = readInt(fp);
		tex.Xpos = readFloat(fp);
		tex.Ypos = readFloat(fp);
		tex.Xscale = readFloat(fp);
		tex.Yscale = readFloat(fp);
		tex.Angle = readFloat(fp);
	}
}

void MeshLoaderB3D::readNODE(FILE* fp)
{
	if (m_ReadJoint == NULL)
	{
		m_RootJoint = new Joint;
		m_ReadJoint = m_RootJoint;
	}
	else
	{
		Joint* newJoint = new Joint;
		m_ReadJoint->children.push_back(newJoint);
		newJoint->parent = m_ReadJoint;
		m_ReadJoint = newJoint;
	}

	string str = readString(fp);
	printTree(str.c_str());

	Vec3 t=readVec3(fp);
	Vec3 s=readVec3(fp);
	Quat r=readQuat(fp);

	m_ReadJoint->name = str;
	m_ReadJoint->position = t;
	m_ReadJoint->scale = s;
	m_ReadJoint->rotation = r;

	while( checkSize(fp) ){
		string t=readChunk(fp);
		if( t=="MESH" ){
			readMesh(fp);
		}else if( t=="BONE" ){
			readBONE(fp);
		}else if( t=="ANIM" ){
			readANIM(fp);
		}else if( t=="KEYS" ){
			readKEY(fp);
		}else if( t=="NODE" ){
			readNODE(fp);
		}
		exitChunk(fp);
	}

	m_ReadJoint = m_ReadJoint->parent;
}

void MeshLoaderB3D::readBRUS(FILE* fp)
{
	int n_texs=readInt(fp);
	if( n_texs<0 || n_texs>8 ){
		printf( "Bad texture count" );
	}
	while( checkSize(fp) ){
		string name=readString(fp);
		// printTree(name.c_str());
		Vec3 color= readVec3(fp);
		float alpha=readFloat(fp);
		float shiny=readFloat(fp);
		/*int blend=**/readInt(fp);
		int fx=readInt(fp);

		//Textures
		for( int i=0;i<n_texs;++i ){
			int texid=readInt(fp);
		}
	}
}

void MeshLoaderB3D::readBONE(FILE* fp)
{
	int i = 0;
	while( checkSize(fp) ){
		int vertex=readInt(fp);
		float weight=readFloat(fp);
		m_ReadJoint->vertexIndices.push_back(vertex);
		m_ReadJoint->vertexWeights.push_back(weight);
#ifdef DEBUG_B3D
		i++;
#endif
	}
	printTree("vertex count: %d", i);
}

void MeshLoaderB3D::readMesh(FILE* fp)
{
	/*int matid=*/readInt(fp);

	//printTree("mesh");
	while( checkSize(fp) ){
		string t=readChunk(fp);
		if( t=="VRTS" ){
			readVRTS(fp);
		}else if( t=="TRIS" ){
			readTRIS( fp );
		}
		exitChunk(fp);
	}
}

void MeshLoaderB3D::printTree(const char *psz, ...)
{
	char sBuf[128];
	va_list ap;
	va_start(ap, psz);
	vsnprintf_s(sBuf, 128, 128, psz, ap);
	va_end(ap);

	for (int i = 0; i < m_Stack.size();i++)
		printf("-");

	printf(sBuf);
	printf("\n");
}

Vec3 MeshLoaderB3D::readVec3(FILE* fp)
{
	float x=readFloat(fp);
	float y=readFloat(fp);
	float z=readFloat(fp);
	return Vec3( x,y,z );
}

Quat MeshLoaderB3D::readQuat(FILE* fp)
{
	float w=-readFloat(fp);
	float x=readFloat(fp);
	float y=readFloat(fp);
	float z=readFloat(fp);
	return Quat( w,x,y,z );
}

void MeshLoaderB3D::readTRIS(FILE* fp){
	Mesh* mesh = _meshVec.back();
	int matid=readInt(fp);
	if( matid==-1 ){
		matid=0;
	}
	int size = m_Stack.back() - ftell(fp);
	int n_tris=size/12;

	for( int i=0;i<n_tris;++i ){
		int i0 = readUInt(fp);
		int i1 = readUInt(fp);
		int i2 = readUInt(fp);

		mesh->_indices.push_back(i0);
		mesh->_indices.push_back(i1);
		mesh->_indices.push_back(i2);
	}
}

unsigned int MeshLoaderB3D::readUInt(FILE* fp)
{
	unsigned int ret;
	fread(&ret, sizeof(unsigned int), 1, fp);
	return ret;
}

void MeshLoaderB3D::readANIM(FILE* fp){
	/*int flags=*/readInt(fp);
	int frames=readInt(fp);
	float fps=readFloat(fp);
	m_TotalFrame = frames;
}

void MeshLoaderB3D::readKEY(FILE* fp)
{
	int flags=readInt(fp);
	while( checkSize(fp) ){
		int frame=readInt(fp);
		if (flags & 1){
			Vec3 v = readVec3(fp);
			PositionKey k = { v, frame };
			m_ReadJoint->positionKeys.push_back(k);
		}
		if( flags & 2 ){
			Vec3 v = readVec3(fp);
			ScaleKey k = { v, frame };
			m_ReadJoint->scaleKeys.push_back(k);
		}
		if( flags & 4 ){
			Quat r = readQuat(fp);
			RotationKey k = { r, frame };
			m_ReadJoint->rotationKeys.push_back(k);
		}
	}
}