#include "stdafx.h"
#include "../GL/glew.h"
#include "3DModel.h"
#include <iostream>
#include "../glm/gtc/type_ptr.hpp"
#include "../glm/gtc/matrix_transform.hpp"
//#define BUFFER_OFFSET(i) ((char *)NULL + (i))
#define BUFFER_OFFSET(i) (reinterpret_cast<void*>(i))

#define GL_CHECK_ERRORS assert(glGetError()== GL_NO_ERROR);
#define printOpenGLError() printOglError(__FILE__, __LINE__)
int printOglError(char *file, int line)
{
    GLenum glErr;
    int    retCode = 0;
    glErr = glGetError();
    while (glErr != GL_NO_ERROR)
    {
        ///std::cout << "glError in file " << file << "@ line " << line << ": " << gluErrorString(glErr) << std::endl;
			std::cout << "glError in file " << file << "@ line " << line << std::endl;
        retCode = 1;
        glErr = glGetError();
    }
    return retCode;
}

///
/// default constructor
///
C3DModel::C3DModel()
{
	//m_uVBO = m_uVAOVertex = m_uVBOIndex = m_uVBOColor = 0;
	m_iIndexSize = 0;
	m_iNPoints = -1;
	m_vMaterial.clear();
	m_vMesh.clear();
	m_vTexture.clear();
	m_vTextureMatIndex.clear();
}

///
/// default destructor
///
C3DModel::~C3DModel()
{
	m_vMaterial.clear();
	m_vMesh.clear();
	m_vTexture.clear();
	m_vTextureMatIndex.clear();
	deleteBuffers();
	TRACE("model unloaded\n");
}

void C3DModel::deleteBuffers()
{
	//if (m_uVBO != 0)	glDeleteBuffers(1, &m_uVBO);
	//if (m_uVAOVertex != 0)	glDeleteBuffers(1, &m_uVAOVertex);
	//if (m_uVBOIndex != 0) glDeleteBuffers(1, &m_uVBOIndex);
	//if (m_uVBOColor != 0) glDeleteBuffers(1, &m_uVBOColor);
}

//void printvec4(glm::vec4 v)
//{
//	std::cout << v.r << " " << v.g << " " << v.b << std::endl;
//}
///
/// Method applies the materials to an object given a mtl
///
/// @param pMaterial
///
void C3DModel::applyMaterial(const aiMaterial* pMaterial)
{
	aiColor4D color;
	Material mat;

	//ambient component
	mat.cAmbient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
	if(pMaterial != NULL && AI_SUCCESS == aiGetMaterialColor(pMaterial, AI_MATKEY_COLOR_AMBIENT, &color))
		mat.cAmbient = glm::vec4(color.r, color.g, color.b, color.a);

	//diffuse component
	mat.cDiffuse = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
	if(pMaterial != NULL && AI_SUCCESS == aiGetMaterialColor(pMaterial, AI_MATKEY_COLOR_DIFFUSE, &color))
		mat.cDiffuse = glm::vec4(color.r, color.g, color.b, color.a);

	//specular component
	mat.cSpecular = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	if(pMaterial != NULL && AI_SUCCESS == aiGetMaterialColor(pMaterial, AI_MATKEY_COLOR_SPECULAR, &color))
		mat.cSpecular = glm::vec4(color.r, color.g, color.b, color.a);

	//emission component
	mat.cEmission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	if(pMaterial != NULL && AI_SUCCESS == aiGetMaterialColor(pMaterial, AI_MATKEY_COLOR_EMISSIVE, &color))
		mat.cEmission = glm::vec4(color.r, color.g, color.b, color.a);

	//shininness
	mat.fShininess = 0.0f;
	if(pMaterial != NULL)
	{
		unsigned int v1, v2;
		float shininess, strength;
		if(aiGetMaterialFloatArray(pMaterial, AI_MATKEY_SHININESS, &shininess, &v1) == AI_SUCCESS &&
			 aiGetMaterialFloatArray(pMaterial, AI_MATKEY_SHININESS, &strength, &v2) == AI_SUCCESS)
			 mat.fShininess = shininess * strength;
	}

	//add
	m_vMaterial.push_back(mat);
	//printvec4(mat.cDiffuse);
}
int global = 0;
///
/// Function to load a 3D object file
///
/// @param pNode
/// @param mMatrix
///
//void C3DModel::fillNode(const struct aiNode* pNode, glm::mat4 mMatrix)
void C3DModel::fillNode(const aiScene* scene, const struct aiNode* pNode, const glm::mat4 & mMatrix, int & iOffset, std::vector<Vertex> & vVertex, std::vector<unsigned int> & vIndex)
{
	aiMatrix4x4 aiMatrix = pNode->mTransformation;
	//update the transform
	glm::mat4x4 mModelMatrix = mMatrix * glm::mat4x4(aiMatrix.a1, aiMatrix.a2, aiMatrix.a3, aiMatrix.a4,
																									 aiMatrix.b1, aiMatrix.b2, aiMatrix.b3, aiMatrix.b4,
																									 aiMatrix.c1, aiMatrix.c2, aiMatrix.c3, aiMatrix.c4,
																									aiMatrix.d1, aiMatrix.d2, aiMatrix.d3, aiMatrix.d4);
	//std::cout << "1st" << std::endl;
	glm::mat4x4 mNormalMatrix = glm::transpose(glm::inverse(mMatrix));
	//std::cout << "number of meshes " << pNode->mNumMeshes << std::endl;
	int k;
	for(k = 0; k < pNode->mNumMeshes; k++)
	{
		const struct aiMesh* mesh = scene->mMeshes[pNode->mMeshes[k]];
		if(mesh->mMaterialIndex < scene->mNumMaterials && mesh->mMaterialIndex >= 0)
		{
			applyMaterial(scene->mMaterials[mesh->mMaterialIndex]);
			//printvec4( m_vMaterial[k].cDiffuse);
			//m_vMaterial.front();
		}
		else
			applyMaterial(NULL);
		Mesh m;
		m.iFirst = vIndex.size();
		m.iCount = mesh->mNumFaces * 3;
		m.iId = mesh->mMaterialIndex;
		m_vMesh.push_back(m);
		//mesh->mF
		int i;
		//std::cout << "2nd " << mesh->mNumFaces << "--" << std::endl;
		for(int t = 0; t < mesh->mNumFaces; t++)
		{
			const struct aiFace* face = &mesh->mFaces[t];
			//std::cout << &mesh->mFaces[t].mNumIndices << std::endl;
			unsigned int face_mode;
			//std::cout << "7" << std::endl;
			//if(face->mIndices == NULL)
			//{
			//std::cout << "8" << std::endl;
			//std::cout << face->mNumIndices  << std::endl;
			
			//std::cout << "faces"  << std::endl;
			switch(face->mNumIndices)
			{
			case 3:
				face_mode = GL_TRIANGLES;
				break;
			default:
				std::cout << "There are faces that are not triangulated!!!" << std::endl;
				//system("pause");
				//exit(0);
			}
			//std::cout << t << " " << face->mNumIndices << std::endl;
			for(i = 0; i < face->mNumIndices; i++)		// go through all vertices in face
				vIndex.push_back(iOffset + face->mIndices[i]); // get group index for current index
			//}
		}
		
		//std::cout << "1" << std::endl;
		aiColor4D color;
		for(i = 0; i < mesh->mNumVertices; ++i)
		{
			Vertex v;
			if(mesh->HasTextureCoords(0))	//check
				v.tTextureCoord = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
			else v.tTextureCoord = glm::vec2(0,0);
			v.vNormalCoord = glm::vec3(mesh->mNormals[i].x,mesh->mNormals[i].y,mesh->mNormals[i].z);
			v.pWorldCoord = mModelMatrix*glm::vec4(mesh->mVertices[i].x,mesh->mVertices[i].y,mesh->mVertices[i].z,1.0f);
			
			m_bbox.addPoint(glm::vec3(v.pWorldCoord));	//CHECK
			//std::cout << mesh->mMaterialIndex << std::endl;
			//scene->mMaterials[mesh->mMaterialIndex]
			//v.vColorDiffuse = glm::vec3();
			//printvec4(m_vMaterial.front().cDiffuse);
			v.vColorDiffuse = glm::vec4(1,0,0,1);
			if(aiGetMaterialColor(scene->mMaterials[mesh->mMaterialIndex], AI_MATKEY_COLOR_DIFFUSE, &color) == AI_SUCCESS){
				v.vColorDiffuse = glm::vec4(color.r, color.g, color.b, color.a);
				//printvec4(v.vColorDiffuse);
				//std::cout << color.r << " " << color.g << " " << color.b << std::endl;
			}
			//std::cout << m_vMaterial[mesh->mMaterialIndex].cDiffuse.r << std::endl;
			vVertex.push_back(v);
		}
		iOffset += mesh->mNumVertices;
	}
	//std::cout << "3 " << pNode->mNumChildren << std::endl;
	// draw all children
	for (k = 0; k < pNode->mNumChildren; k++)
	{
		fillNode(scene, pNode->mChildren[k], mModelMatrix, iOffset, vVertex, vIndex);
	}
}

///
/// Function to load a 3D object file
///
/// @param sFilename the filename of the 3d object
///
/// @return true if it is load correctly, false otherwise
///
bool C3DModel::load(const std::string & sFilename, glm::vec3 pCenterOn)
{
	TRACE("loading the file %s\n", sFilename.c_str());
	//aiScene* scene;
	Assimp::Importer importer;
	//std::cout << "hello" << std::endl;
	const aiScene* scene = importer.ReadFile(sFilename, aiProcess_Triangulate | aiProcess_GenSmoothNormals);
	
	if(!scene)
	{
		TRACE("Import error : %s\n", importer.GetErrorString());
		return false;
	}
	//std::cout << "1" << std::endl;
	m_bHasTextures = scene->HasTextures();
	//m_bHasTextures = true;
	//std::cout << " " << scene->mNumTextures << std::endl;
	//scene contains all the information
	int iOffset = 0;
	
	
	//std::cout << "before" << std::endl;
	glm::mat4 mymatrix;
	fillNode(scene, scene->mRootNode, glm::mat4(), iOffset, vVertex, vIndex);
//std::cout << "1" << std::endl;
	int iNumTriangles = 0;
	for(int k = 0;k < m_vMesh.size(); k++)
		iNumTriangles += (m_vMesh[k].iCount / 3);
//std::cout << "2" << std::endl;
	if(pCenterOn.x != 0 || pCenterOn.y != 0 || pCenterOn.z != 0)
	{
		m_bbox.resetPoints();
		for (int k = 0; k < vVertex.size(); k++)
		{
			glm::mat4 m = glm::translate(glm::mat4(1), -m_bbox.getCenter());
			glm::vec4 p = m * glm::vec4(vVertex[k].pWorldCoord.x, vVertex[k].pWorldCoord.y, vVertex[k].pWorldCoord.z,1);
			m = glm::translate(glm::mat4(1), pCenterOn);
			glm::vec4 p2 = m * p;
			//glm::vec4 p = m * glm::vec4(vVertex[k].pWorldCoord.x, vVertex[k].pWorldCoord.y, vVertex[k].pWorldCoord.z, 1);
			vVertex[k].pWorldCoord.x = p2.x;
			vVertex[k].pWorldCoord.y = p2.y;
			vVertex[k].pWorldCoord.z = p2.z;
			m_bbox.addPoint(glm::vec3(vVertex[k].pWorldCoord.x, vVertex[k].pWorldCoord.y, vVertex[k].pWorldCoord.z));
		}
	}
	
	std::vector<Vertex>::iterator it = vVertex.begin();
	while (it != vVertex.end())
	{
		glm::vec3 temp;
		glm::vec2 coordt;
		temp.x = (*it).pWorldCoord.x;
		temp.y = (*it).pWorldCoord.y;
		temp.z = (*it).pWorldCoord.z;
		coordt.s = (*it).tTextureCoord.s;
		coordt.t = (*it).tTextureCoord.t;
		m_Vertex.push_back(temp);
		m_TexCood.push_back(coordt);
		it++;
	}
	
	////creating the VAO for the model
	//glGenVertexArrays(1, &m_uVAO);
	//glBindVertexArray(m_uVAO);

	//	//creating the VBO
	//	glGenBuffers(1, &m_uVBO);
	//	glGenBuffers(1, &m_uVBOIndex);

	//	m_iIndexSize = vIndex.size();
	//	glBindBuffer(GL_ARRAY_BUFFER, m_uVBO);
	//	glBufferData(GL_ARRAY_BUFFER, vVertex.size() * sizeof(Vertex), &vVertex[0], GL_STATIC_DRAW);

	//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_uVBOIndex);
	//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, vIndex.size()*sizeof(GL_UNSIGNED_INT), &vIndex[0], GL_STATIC_DRAW);

	//	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(0)); //Vertex
	//	glEnableVertexAttribArray(0);

	//	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(sizeof(GL_FLOAT) * 4)); //Normals
	//	glEnableVertexAttribArray(1);
	//
	//	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(sizeof(GL_FLOAT) * 7)); //color
	//	glEnableVertexAttribArray(2);

	//	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(sizeof(GL_FLOAT) * 11)); //Tex Coord
	//	glEnableVertexAttribArray(3);

	//glBindVertexArray(0);	//VAO

	m_iNPoints = vVertex.size();
	////creating the VAO for the vertexs
	//glGenVertexArrays(1, &m_uVAOVertex);
	//glBindVertexArray(m_uVAOVertex);
	//	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(0)); //Vertex
	//	glEnableVertexAttribArray(0);
	//glBindVertexArray(0);	//VAO

	loadTextures(sFilename, scene);
	
	importer.FreeScene();
	//vIndex.clear();
	//vVertex.clear();

	TRACE("material: %d\n", m_vMaterial.size());
	TRACE("mesh: %d\n", m_vMesh.size());
	TRACE("texture: %d\n", m_vTexture.size());
	TRACE("texture mat index: %d\n", m_vTextureMatIndex.size());

	return true;
}

///
/// Method to load textures from the MTL file
///
void C3DModel::loadTextures(const std::string& sFilename, const aiScene* pScene)
{
	m_vTexture.reserve(pScene->mNumMaterials);
	m_vTextureMatIndex.reserve(pScene->mNumMaterials);
	for(int k = 0; k < pScene->mNumMaterials; k++)
	{
		CTexture emptyTex;
		m_vTexture.push_back(emptyTex);
		m_vTextureMatIndex.push_back(0);
	}

	bool repeated;
	unsigned int indexrep;
	int flo;
	for (int i = 0 ; i < pScene->mNumMaterials ; i++)
	{
		const aiMaterial* pMaterial = pScene->mMaterials[i];
		repeated = false;
		if(pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
		{
			aiString Path;
			if(pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
			{
				//Find if another textured with the same path have been loaded
				for(int j = 0 ; j < i && !repeated; j++)
				{
					const aiMaterial* pMaterial = pScene->mMaterials[j];
					if(pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
					{
						aiString Path2;
						if(pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path2, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
						{
							if(Path2 == Path)
							{
								repeated = true;
								indexrep = j;
							}
						}
					}
				}
				if(repeated)
				{
					m_vTextureMatIndex[i]	= indexrep;
				}
				else
				{
					//If it is a new texture create it
					flo = sFilename.find_last_of("/");
					if(m_vTexture[i].loadTexture((sFilename.substr(0,flo) + "/" + (std::string)Path.data).c_str()))
						m_vTextureMatIndex[i] = i;
				}
			}
		}
		// Load a white texture in case the model does not include its own texture
		if (!m_vTexture[i].isValid() && !repeated)
		{
			//Material m1 = m_vMaterial[i];
			m_vTexture[i].loadTexture("models/white.png");
			//pMaterial->
			//m_vTexture[i].loadColor("objects/white.png");
			m_vTextureMatIndex[i] = i;
		}
	}
}

///
/// Method to draw the object
///
void C3DModel::drawObject()
{
	//glBindVertexArray(m_uVAO);
	//for(int k = 0; k < m_vMesh.size(); k++)
	//{
	//	m_vTexture[m_vTextureMatIndex[m_vMesh[k].iId]].bindTexture();
	//	//glDrawRangeElements(GL_TRIANGLES,0,	m_iIndexSize, m_vMesh[k].iCount, GL_UNSIGNED_INT,BUFFER_OFFSET(sizeof(GL_UNSIGNED_INT)*m_vMesh[k].iFirst));
	//	glDrawElementsBaseVertex(GL_TRIANGLES, m_vMesh[k].iCount, GL_UNSIGNED_INT, NULL, m_vMesh[k].iFirst);
	//}
	//glBindVertexArray(0);
	//glEnableClientState(GL_COLOR_ARRAY);
	
	//glEnableClientState(GL_VERTEX_ARRAY);
	//glVertexPointer(3, GL_FLOAT, 0, &m_Vertex[0]);
	//glDrawArrays(GL_POINTS, 0, m_Vertex.size());
	//glDisableClientState(GL_VERTEX_ARRAY);
	for (int k = 0; k < m_vMesh.size(); k++)
	{
		m_vTexture[m_vTextureMatIndex[m_vMesh[k].iId]].bindTexture();
		for (int k = 0; k < vIndex.size(); k += 3)
		{
			glBegin(GL_TRIANGLES);
			glTexCoord2fv(glm::value_ptr(m_TexCood[vIndex[k]]));
			glVertex3fv(glm::value_ptr(m_Vertex[vIndex[k]]));

			glTexCoord2fv(glm::value_ptr(m_TexCood[vIndex[k + 1]]));
			glVertex3fv(glm::value_ptr(m_Vertex[vIndex[k + 1]]));

			glTexCoord2fv(glm::value_ptr(m_TexCood[vIndex[k + 2]]));
			glVertex3fv(glm::value_ptr(m_Vertex[vIndex[k + 2]]));
			glEnd();
		}
	}
	//assert(glGetError() == GL_NO_ERROR);
	//int j = m_vTextureMatIndex[m_vMesh[0].iId];
	//m_vTexture[j].bindTexture();
	//assert(glGetError() == GL_NO_ERROR);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	

	//glBegin(GL_TRIANGLES);
	//	//for (int k = 0; k < m_vMesh.size(); k++)
	//	{
	//		//glDrawElements(GL_TRIANGLES, m_vMesh[k].iCount, GL_UNSIGNED_INT, &vIndex[0]);
	//		//glDrawElementsBaseVertex(GL_TRIANGLES, m_vMesh[k].iCount, GL_UNSIGNED_INT, &vIndex[0], m_vMesh[k].iFirst);
	//		//glDrawArrays(GL_POINTS,)
	//		
	//	}
	//glEnd();

}

///
/// Method to draw points over surface
///
void C3DModel::drawPoints()
{
	for (int k = 0; k < vIndex.size(); k += 3)
	{
		glBegin(GL_POINTS);
		glVertex3fv(glm::value_ptr(m_Vertex[vIndex[k]]));
		glVertex3fv(glm::value_ptr(m_Vertex[vIndex[k + 1]]));
		glVertex3fv(glm::value_ptr(m_Vertex[vIndex[k + 2]]));
		glEnd();
	}
}