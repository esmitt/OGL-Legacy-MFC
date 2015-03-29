#pragma once
#include "loader/3DModel.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <string>
#include "ArcBall.h"
#include "OGLBasic.h"

class CGLRenderer
{
public:
	CGLRenderer();
	virtual ~CGLRenderer();
	bool CreateGLContext(CDC* pDC); 	// Creates OpenGL Rendering Context
	void InitScene(std::string strFilename);		// Scene preparation stuff
	void Reshape(CDC* pDC, int w, int h);	// Changing viewportCGLRenderer
	void DrawScene(CDC* pDC);		// Draws the scene
	void DestroyScene(CDC* pDC);		// Cleanup
	void MouseDown(int x, int y);
	void MouseMove(int x, int y, MOUSE_OP operation);
	glm::mat4 GetModelViewMatrix(){ return m_modelViewMatrix; }
	glm::mat4 GetProjectionMatrix(){ return m_projMatrix; }
protected:
	HGLRC	 m_hrc;                        // OpenGL Rendering Context 
	CArcBall m_arcBall;
	glm::vec3 m_vVertexColor;			//color of vertex drawn
private:
	C3DModel m_model;
	glm::mat4x4 m_projMatrix, m_modelViewMatrix;
	void DrawGradientBackground();
};

