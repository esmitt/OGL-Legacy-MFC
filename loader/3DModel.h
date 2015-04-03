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
#include "../glm/gtc/type_ptr.hpp"
#include "../glm/gtc/matrix_transform.hpp"

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
/// A class for loading and displaying and object using Open Asset Import Librery. Always is assume that model HAS TEXTURE
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
	int m_iNPoints;
	unsigned int m_iIndexSize;

	//his own modelmatrix
	glm::mat4 m_mModelMatrix;

	//also added before
	std::vector<glm::vec3> m_Vertex;
	//std::vector<glm::vec3> m_PointsTriangle;	//this structure repeat the vertexes to use the DrawArray
	std::vector<glm::vec2> m_TexCood;
	std::vector<unsigned int> vIndex;
	std::vector<Vertex> vVertex;	//the complete structure, only to read
	GLuint m_iDisplayList;

	/// Recursive to explore all the elements
	void fillNode(const aiScene* scene, const struct aiNode* pNode, const glm::mat4 & mMatrix, int & iOffset, std::vector<Vertex> & vVertex, std::vector<unsigned int> & vIndex);

	///Method applies the materials to an object given a mtl
	void applyMaterial(const aiMaterial* pMaterial);

	///Method to load the textures
	void loadTextures(const std::string& sFilename, const aiScene* pScene);

public:
	C3DModel();
	~C3DModel();

	///Method to load an object
	bool load(const std::string & sFilename, glm::vec3 pCenterOn = glm::vec3(0));

	///Draw the object using the VBO
	void drawObject();

	///Draw points using the VBO
	void drawPoints(const std::vector<bool> & vbSelected);

	///Get the center of the object
	inline glm::vec3 getCenter(){return m_bbox.getCenter();}

	///Get the number of vertexes
	inline int GetNVertex(){ return vVertex.size(); }

	///Get the lenght of the diagonal of bounding box
	inline float getDiagonal(){return m_bbox.getDiagonal();}

	///Get the model matrix of the model
	inline const glm::mat4 & getModelMatrix(){ return m_mModelMatrix; }

	///Get the array of vertex read-only
	inline const std::vector<glm::vec3> & getVertexData(){ return m_Vertex; }
};
#endif
