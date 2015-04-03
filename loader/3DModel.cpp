#include "stdafx.h"
#include "../GL/glew.h"
#include "3DModel.h"
#include <iostream>

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
			TRACE(L"glError in file %s @line %d", file, line);
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
	m_iIndexSize = 0;
	m_iNPoints = -1;
	m_vMaterial.clear();
	m_vMesh.clear();
	m_vTexture.clear();
	m_vTextureMatIndex.clear();
	//m_PointsTriangle.clear();
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
	//m_PointsTriangle.clear();
	glDeleteLists(m_iDisplayList, 1);
	TRACE("model unloaded\n");
}

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
}

///
/// Function to load a 3D object file
///
/// @param pNode
/// @param mMatrix
///
void C3DModel::fillNode(const aiScene* scene, const struct aiNode* pNode, const glm::mat4 & mMatrix, int & iOffset, std::vector<Vertex> & vVertex, std::vector<unsigned int> & vIndex)
{
	aiMatrix4x4 aiMatrix = pNode->mTransformation;
	//update the transform
	glm::mat4x4 mModelMatrix = mMatrix * glm::mat4x4(aiMatrix.a1, aiMatrix.a2, aiMatrix.a3, aiMatrix.a4,
																									 aiMatrix.b1, aiMatrix.b2, aiMatrix.b3, aiMatrix.b4,
																									 aiMatrix.c1, aiMatrix.c2, aiMatrix.c3, aiMatrix.c4,
																									aiMatrix.d1, aiMatrix.d2, aiMatrix.d3, aiMatrix.d4);
	glm::mat4x4 mNormalMatrix = glm::transpose(glm::inverse(mMatrix));
	size_t k;
	for(k = 0; k < pNode->mNumMeshes; k++)
	{
		const struct aiMesh* mesh = scene->mMeshes[pNode->mMeshes[k]];
		if(mesh->mMaterialIndex < scene->mNumMaterials && mesh->mMaterialIndex >= 0)
		{
			applyMaterial(scene->mMaterials[mesh->mMaterialIndex]);
		}
		else
			applyMaterial(NULL);
		Mesh m;
		m.iFirst = vIndex.size();
		m.iCount = mesh->mNumFaces * 3;
		m.iId = mesh->mMaterialIndex;
		m_vMesh.push_back(m);
		size_t i;

		for(size_t t = 0; t < mesh->mNumFaces; t++)
		{
			const struct aiFace* face = &mesh->mFaces[t];
			unsigned int face_mode;
			switch(face->mNumIndices)
			{
			case 3:
				face_mode = GL_TRIANGLES;
				break;
			default:
				AfxMessageBox(L"There are faces that are not triangulated!!!");
			}
			for(i = 0; i < face->mNumIndices; i++)		// go through all vertices in face
				vIndex.push_back(iOffset + face->mIndices[i]); // get group index for current index
		}
		
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
			v.vColorDiffuse = glm::vec4(1,0,0,1);
			if(aiGetMaterialColor(scene->mMaterials[mesh->mMaterialIndex], AI_MATKEY_COLOR_DIFFUSE, &color) == AI_SUCCESS){
				v.vColorDiffuse = glm::vec4(color.r, color.g, color.b, color.a);
			}
			vVertex.push_back(v);
		}
		iOffset += mesh->mNumVertices;
	}
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
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(sFilename, aiProcess_Triangulate | aiProcess_GenSmoothNormals);
	
	if(!scene)
	{
		TRACE("Import error : %s\n", importer.GetErrorString());
		return false;
	}
	//m_bHasTextures = scene->HasTextures();
	int iOffset = 0;
	
	glm::mat4 mymatrix;
	fillNode(scene, scene->mRootNode, glm::mat4(), iOffset, vVertex, vIndex);
	int iNumTriangles = 0;
	for(size_t k = 0;k < m_vMesh.size(); k++)
		iNumTriangles += (m_vMesh[k].iCount / 3);
	if(pCenterOn.x != 0 || pCenterOn.y != 0 || pCenterOn.z != 0)
	{
		m_bbox.resetPoints();
		for (size_t k = 0; k < vVertex.size(); k++)
		{
			glm::mat4 m = glm::translate(glm::mat4(1), -m_bbox.getCenter());
			glm::vec4 p = m * glm::vec4(vVertex[k].pWorldCoord.x, vVertex[k].pWorldCoord.y, vVertex[k].pWorldCoord.z,1);
			m = glm::translate(glm::mat4(1), pCenterOn);
			glm::vec4 p2 = m * p;
			vVertex[k].pWorldCoord.x = p2.x;
			vVertex[k].pWorldCoord.y = p2.y;
			vVertex[k].pWorldCoord.z = p2.z;
			m_bbox.addPoint(glm::vec3(vVertex[k].pWorldCoord.x, vVertex[k].pWorldCoord.y, vVertex[k].pWorldCoord.z));
		}
	}
	else
	{
		//here the model is also centered, just need to be scaled
		glm::mat4 m = glm::translate(glm::mat4(1), -m_bbox.getCenter());
		float fScale = 1.f / getDiagonal();
		m_mModelMatrix = glm::scale(glm::mat4(1), glm::vec3(fScale)) * m;
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

	m_iNPoints = vVertex.size();
	loadTextures(sFilename, scene);
	//copy all triangles points (including repeted in a structure to use a drawarray
	//m_PointsTriangle.clear();
	//m_PointsTriangle.reserve(vIndex.size() * 3);
	//for (size_t k = 0; k < vIndex.size(); k += 3)
	//{
	//	glm::vec3 v1  = m_Vertex[vIndex[k]];
	//	glm::vec3 v2 = m_Vertex[vIndex[k + 1]];
	//	glm::vec3 v3 = m_Vertex[vIndex[k + 2]];
	//	m_PointsTriangle.push_back(v1);
	//	m_PointsTriangle.push_back(v2);
	//	m_PointsTriangle.push_back(v3);
	//}

	//a display list
	m_iDisplayList = glGenLists(1);
	// compile the display list, store a triangle in it
	glNewList(m_iDisplayList, GL_COMPILE);
		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		for (size_t k = 0; k < m_vMesh.size(); k++)
		{
			m_vTexture[m_vTextureMatIndex[m_vMesh[k].iId]].bindTexture();
			glTexCoordPointer(2, GL_FLOAT, 0, &m_TexCood[0]);
			glVertexPointer(3, GL_FLOAT, 0, &m_Vertex[0]);
			glDrawElements(GL_TRIANGLES, m_vMesh[k].iCount, GL_UNSIGNED_INT, &vIndex[0]);
		}
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisable(GL_TEXTURE_2D);
	glEndList();

	importer.FreeScene();

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
	for(size_t k = 0; k < pScene->mNumMaterials; k++)
	{
		CTexture emptyTex;
		m_vTexture.push_back(emptyTex);
		m_vTextureMatIndex.push_back(0);
	}

	bool repeated;
	unsigned int indexrep;
	int flo;
	for (size_t i = 0 ; i < pScene->mNumMaterials ; i++)
	{
		const aiMaterial* pMaterial = pScene->mMaterials[i];
		repeated = false;
		if(pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
		{
			aiString Path;
			if(pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
			{
				//Find if another textured with the same path have been loaded
				for(size_t j = 0 ; j < i && !repeated; j++)
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
			m_vTexture[i].loadTexture("models/white.png");
			m_vTextureMatIndex[i] = i;
		}
	}
}

///
/// Method to draw the object
///
void C3DModel::drawObject()
{
	glCallList(m_iDisplayList);
	//more slow but safe way to draw
	//for (size_t k = 0; k < m_vMesh.size(); k++)
	//{
	//	m_vTexture[m_vTextureMatIndex[m_vMesh[k].iId]].bindTexture();
	//	for (size_t k = 0; k < vIndex.size(); k += 3)
	//	{
	//	glBegin(GL_TRIANGLES);
	//	glTexCoord2fv(glm::value_ptr(m_TexCood[vIndex[k]]));
	//	glVertex3fv(glm::value_ptr(m_Vertex[vIndex[k]]));
	//	glTexCoord2fv(glm::value_ptr(m_TexCood[vIndex[k + 1]]));
	//	glVertex3fv(glm::value_ptr(m_Vertex[vIndex[k + 1]]));
	//	glTexCoord2fv(glm::value_ptr(m_TexCood[vIndex[k + 2]]));
	//	glVertex3fv(glm::value_ptr(m_Vertex[vIndex[k + 2]]));
	//	glEnd();
	//	}
	//}
}

///
/// Method to draw points over surface
///
void C3DModel::drawPoints(const std::vector<bool> & vbSelected)
{
	std::vector<glm::vec3> colors;
	colors.reserve(m_Vertex.size());
	const glm::vec3 colorSelected(1.0f, 0.5f, 0.0f);
	const glm::vec3 colorUnSelected(0.4f, 0.8f, 0.1f);
	for (size_t k = 0; k < m_Vertex.size(); k++)
	{
		if (vbSelected[k])
			colors.push_back(colorSelected);
		else
			colors.push_back(colorUnSelected);
	}
	//drawing
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, &m_Vertex[0]);
		glColorPointer(3, GL_FLOAT, 0, &colors[0]);
		glDrawArrays(GL_POINTS, 0, m_Vertex.size());
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glPopAttrib();
}