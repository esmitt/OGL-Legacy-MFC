#ifndef __3DMODEL_H__
#define __3DMODEL_H__

#include "BoundingBox.h"

#include "Texture.h"
#ifdef _WIN32
	#include "assimp/scene.h"
	#include "assimp/Importer.hpp"
	#include "assimp/postprocess.h"
#else
	#include <assimp/Importer.hpp>
	#include <assimp/scene.h>
	#include <assimp/postprocess.h>
#endif
#include <string>
#include <vector>

#ifdef _WIN32
	#pragma comment (lib,"lib/assimp.lib")
#endif

struct Vertex
{
		glm::vec4 pWorldCoord;
		glm::vec3 vNormalCoord;
		glm::vec4 vColorDiffuse;
		glm::vec2 tTextureCoord;
};

struct Mesh
{
	unsigned int iFirst;
	unsigned int iCount;
	unsigned int iId;
};

struct Material
{
		glm::vec4 cDiffuse;
		glm::vec4 cSpecular;
		glm::vec4 cAmbient;
		glm::vec4 cEmission;
		float fShininess;
};


///
/// Class C3DModel.
///
/// A class for loading and displaying and object using Open Asset Import Librery
///
///
class C3DModel
{
private:
	CBoundingBox m_bbox;
	std::vector<Material> m_vMaterial;
	std::vector<Mesh> m_vMesh;
	std::vector<CTexture> m_vTexture;
	std::vector<unsigned int> m_vTextureMatIndex;
	//unsigned int m_uVBO;
	//unsigned int m_uVBOIndex;
	//unsigned int m_uVBOColor;
	int m_iNPoints;
	GLuint m_uVAO;		//for the model
	GLuint m_uVAOVertex;	//for the vertexes
	unsigned int m_iIndexSize;
	bool m_bHasTextures;

	//also added before
	std::vector<glm::vec3> m_Vertex;
	std::vector<glm::vec2> m_TexCood;
	std::vector<unsigned int> vIndex;
	std::vector<Vertex> vVertex;


	/// Recursive to explore all the elements
	//void fillNode(const struct aiNode* pNode, glm::mat4 mMatrix);
	void fillNode(const aiScene* scene, const struct aiNode* pNode, const glm::mat4 & mMatrix, int & iOffset, std::vector<Vertex> & vVertex, std::vector<unsigned int> & vIndex);

	///Method applies the materials to an object given a mtl
	void applyMaterial(const aiMaterial* pMaterial);

	///Method to load the textures
	void loadTextures(const std::string& sFilename, const aiScene* pScene);

public:
	C3DModel();
	~C3DModel();

	///Return of have texture
	inline bool hasTexture(){return m_bHasTextures;}

	///Method to load an object
	bool load(const std::string & sFilename, glm::vec3 pCenterOn = glm::vec3(0));

	///Draw the object using the VBO
	void drawObject();

	///Draw points using the VBO
	void drawPoints();

	///Get the center of the object
	inline glm::vec3 getCenter(){return m_bbox.getCenter();}

	///Get the lenght of the diagonal of bounding box
	inline float getDiagonal(){return m_bbox.getDiagonal();}

	//void centerOnPoint(glm::vec3 p);
	void deleteBuffers();
};
#endif
