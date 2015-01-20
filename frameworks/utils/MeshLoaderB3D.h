/*
http://www.blitzbasic.com/sdkspecs/sdkspecs/b3dfile_specs.txt
*/

#pragma once
#include <string>
#include "math/quat.h"
#include "math/vec3.h"
#include "math/vec2.h"
#include <vector>
using std::vector;
using std::string;

class Mesh;
class Joint;

class MeshLoaderB3D
{
public:
	MeshLoaderB3D();
	~MeshLoaderB3D();

	bool loadMesh(const char* file);

	struct SB3dChunkHeader
	{
		char name[4];
		int size;
	};

	struct SB3dTexture
	{
		string TextureName;
		int Flags;
		int Blend;
		float Xpos;
		float Ypos;
		float Xscale;
		float Yscale;
		float Angle;
	};

	struct SB3dMaterial
	{
		SB3dMaterial() : red(1.0f), green(1.0f),
			blue(1.0f), alpha(1.0f), shininess(0.0f), blend(1),
			fx(0)
		{
		//	for (unsigned int i=0; i<3; ++i)
				//Textures[i]=0;
		}
		//video::SMaterial Material;
		float red, green, blue, alpha;
		float shininess;
		int blend,fx;
		//SB3dTexture *Textures[video::MATERIAL_MAX_TEXTURES];
	};

	string	readString(FILE* fp);
	int		readInt(FILE* fp);
	unsigned int readUInt(FILE* fp);
	float	readFloat(FILE* fp);
	int     readByte(FILE* fp);
	Vec3    readVec3(FILE* fp);
	Quat readQuat(FILE* fp);

	void readTEXS(FILE* fp);
	bool readVRTS(FILE* fp);
	void readNODE(FILE* fp);
	void readBRUS(FILE* fp);
	void readBONE(FILE* fp);
	void readMesh(FILE* fp);
	void readTRIS(FILE* fp);
	void readANIM(FILE* fp);
	void readKEY(FILE* fp);

	string readChunk(FILE* fp);
	bool checkSize(FILE* fp);
	void exitChunk(FILE* fp);

	void printTree(const char *psz, ...);
	long m_lCurNodePos;
	int m_nLayer;
	vector<SB3dTexture> m_Texture;
	vector<unsigned int> m_Stack;

	vector<Mesh*> _meshVec;

	int           _meshCount;
//	unsigned int*	m_Indice;
//	vec2*			m_TexCoord;
//	vec3*			m_Position;
	Joint*			m_RootJoint;
	Joint*			m_ReadJoint;
	float			m_TotalFrame;
//	unsigned int    m_NumOfVertex;

};

