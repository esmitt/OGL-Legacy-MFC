#include "stdafx.h"
#include "GLRenderer.h"
#include "OGLBasic.h"


CGLRenderer::CGLRenderer() : m_hrc(0)
{
}


CGLRenderer::~CGLRenderer()
{
}

bool CGLRenderer::CreateGLContext(CDC* pDC)
{
	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 32;
	pfd.iLayerType = PFD_MAIN_PLANE;

	int nPixelFormat = ChoosePixelFormat(pDC->m_hDC, &pfd);

	if (nPixelFormat == 0) return false;

	BOOL bResult = SetPixelFormat(pDC->m_hDC, nPixelFormat, &pfd);

	if (!bResult) return false;

	HGLRC tempContext = wglCreateContext(pDC->m_hDC);
	wglMakeCurrent(pDC->m_hDC, tempContext);

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		AfxMessageBox(_T("GLEW is not initialized!"));
	}

	int attribs[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, 2,
		WGL_CONTEXT_MINOR_VERSION_ARB, 1,
		WGL_CONTEXT_FLAGS_ARB, 0,
		0
	};

	if (wglewIsSupported("WGL_ARB_create_context") == 1)
	{
		m_hrc = wglCreateContextAttribsARB(pDC->m_hDC, 0, attribs);
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(tempContext);
		wglMakeCurrent(pDC->m_hDC, m_hrc);
	}
	else
	{	//It's not possible to make a GL 3.x context. Use the old style context (GL 2.1 and before)
		m_hrc = tempContext;
	}

	//Checking GL version
	const GLubyte *GLVersionString = glGetString(GL_VERSION);

	//Or better yet, use the GL3 way to get the version number
	int OpenGLVersion[2];
	glGetIntegerv(GL_MAJOR_VERSION, &OpenGLVersion[0]);
	glGetIntegerv(GL_MINOR_VERSION, &OpenGLVersion[1]);

	if (!m_hrc) return false;
	return true;
}

void CGLRenderer::InitScene(std::string strFilename)
{
	//opengl init
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	glPolygonOffset(1, 1);
	glEnable(GL_POLYGON_OFFSET_FILL);

	glClearColor(0.15f, 0.15f, 0.15f, 1.f);
	if (!m_model.load(strFilename.c_str()))
	{
		AfxMessageBox(L"Problems Loading the file");
	}
	m_modelViewMatrix = glm::mat4x4(1);
	m_vSelected.resize(m_model.GetNVertex());
}

void CGLRenderer::Reshape(CDC* pDC, int w, int h)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	if (h == 0) h = 1;
	m_iWidth = w;
	m_iHeight = h;
	float ratio = w / float(h);
	glViewport(0, 0, w, h);
	
	m_projMatrix = glm::perspective(COGLBasic::getInstance().fAngle, ratio, COGLBasic::getInstance().fNCP, COGLBasic::getInstance().fFCP);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMultMatrixf(glm::value_ptr(m_projMatrix));
	m_arcBall.Resize((float)w, (float)h);

	glMatrixMode(GL_MODELVIEW);

}
void CGLRenderer::Clear()
{
	std::fill(m_vSelected.begin(), m_vSelected.end(), false);
}

void CGLRenderer::CreateBox(int x0, int y0, int x1, int y1)
{
	CMatrix4x4f modelview, projection;
	modelview.SetMatrix(glm::value_ptr(m_modelViewMatrix));//its just a copy
	projection.SetMatrix(glm::value_ptr(m_projMatrix));
	std::vector<CVector4Df> temp;
	std::vector<glm::vec3> data = m_model.getVertexData();
	std::vector<glm::vec3>::iterator it = data.begin();
	while (it != data.end())
	{
		CVector4Df tempVec;
		tempVec.v[0] = (*it).x;
		tempVec.v[1] = (*it).y;
		tempVec.v[2] = (*it).z;
		tempVec.v[3] = 1;
		temp.push_back(tempVec);
		it++;
	}
	modelview = modelview.Transpose();
	projection = projection.Transpose();
	m_box.Create(x0, y0, x1, y1, modelview, projection, temp, m_iWidth, m_iHeight);
}

void CGLRenderer::MoveBoxXY(int x, int y)
{
	CMatrix4x4f projection;
	projection.SetMatrix(glm::value_ptr(m_projMatrix));
	m_box.MoveXY(x, y, m_iWidth, m_iHeight, projection);
}

void CGLRenderer::SelectBox()
{
	CMatrix4x4f modelview;
	modelview.SetMatrix(glm::value_ptr(m_modelViewMatrix));
	modelview = modelview.Transpose();
	std::vector<CVector4Df> temp;
	std::vector<glm::vec3> data = m_model.getVertexData();
	std::vector<glm::vec3>::iterator it = data.begin();
	while (it != data.end())
	{
		CVector4Df tempVec;
		tempVec.v[0] = (*it).x;
		tempVec.v[1] = (*it).y;
		tempVec.v[2] = (*it).z;
		tempVec.v[3] = 1;
		temp.push_back(tempVec);
		it++;
	}
	m_box.Select(modelview, temp, m_vSelected);
}

void CGLRenderer::DrawBox()
{
	m_box.Draw();
}
//void CGLRenderer::Select(int x0, int y0, int x1, int y1)
//{
//	if (x1<x0) std::swap(x0, x1);
//	if (y1<y0) std::swap(y0, y1);
//	glm::mat4 viewport;
//	viewport = glm::scale(glm::mat4(1), glm::vec3(m_iWidth, m_iHeight, 1.0f));
//	viewport = glm::scale(viewport, glm::vec3(1.0f / 2.0f, 1.0f / 2.0f, 1.0f)));
//	viewport = glm::translate(viewport, glm::vec3(1.0f, 1.0f, 0.0f));
//
//	glm::mat4 mvp = m_projMatrix * modelView;
//	for (int i = 0; i<points.size(); i++)
//	{
//		CVector4Df r = mvp * points[i];
//		r.Normalize(); // esto es, dividir entre w, y dejar w en 1
//		if (-1.0f <= r[0] && r[0] <= 1.0f && -1.0f <= r[1] && r[1] <= 1.0f && -1.0f <= r[2] && r[2] <= 1.0f) // inside the frustum
//		{
//			// inside the square?
//			r = viewport * r;
//			r.Normalize(); // esto es, dividir entre w, y dejar w en 1
//			glm::normalize(glm::vec3(0));
//			if (x0 <= r[0] && r[0] <= x1 && y0 <= r[1] && r[1] <= y1)
//				selected[i] = true;
//			else
//				selected[i] = false;
//		}
//		else
//			selected[i] = false;
//	}
//}

void CGLRenderer::DrawGradientBackground()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glPushAttrib(GL_CURRENT_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glBegin(GL_QUADS);
		//bottom color
		glColor3f(0.05f,0.05f,0.2f);
		glVertex2f(-1.0f, -1.0f);
		glVertex2f(1.0f, -1.0f);
		//up color
		glColor3f(0.3f,0.3f,0.4f);
		glVertex2f(1.0f, 1.0f);
		glVertex2f(-1.0f, 1.0f);
	glEnd();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_COLOR_MATERIAL); //disable color influence
	glPopAttrib();
	glLoadIdentity();
}

void CGLRenderer::DrawNormalBackground()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

const float DEG2RAD = 3.14159f / 180.f;
void CGLRenderer::DrawIndicator()
{
	glPushMatrix();
	glLoadIdentity();
	glMultMatrixf(glm::value_ptr(m_mViewMatrix));
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	float ratio = (float)m_iWidth / (float)m_iHeight;
	gluPerspective(90.0, ratio, 0.001, 100);
	float radius = 0.2f;
	glBegin(GL_LINE_LOOP);
	glColor3ub(225, 155, 155);
	for (int i = 0; i < 360; i++)
	{
		float degInRad = i*DEG2RAD;
		glVertex3f(cos(degInRad)*radius, sin(degInRad)*radius, 0);
	}
	glEnd();

	glBegin(GL_LINE_LOOP);
	glColor3ub(155, 225, 155);
	for (int i = 0; i < 360; i++)
	{
		float degInRad = i*DEG2RAD;
		glVertex3f(0, cos(degInRad)*radius, sin(degInRad)*radius);
	}
	glEnd();
	glBegin(GL_LINE_LOOP);
	glColor3ub(155, 155, 225);
	for (int i = 0; i < 360; i++)
	{
		float degInRad = i*DEG2RAD;
		glVertex3f(cos(degInRad)*radius, 0, sin(degInRad)*radius);
	}
	glEnd();
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void CGLRenderer::DrawScene(CDC* pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	DrawGradientBackground();	
	//DrawNormalBackground();
	m_mViewMatrix = glm::translate(glm::mat4(1), glm::vec3(0, 0, -0.5)) * m_arcBall.GetTransformation();
	m_modelViewMatrix = m_mViewMatrix * m_model.getModelMatrix();
	glMultMatrixf(glm::value_ptr(m_modelViewMatrix));
	//DrawIndicator();	//it does not works for now
	m_model.drawObject();
	m_model.drawPoints(m_vSelected);
}

void CGLRenderer::MouseDown(int x, int y)
{
	m_arcBall.OnMouseDown(glm::ivec2(x, y));
}

void CGLRenderer::MouseMove(int x, int y, MOUSE_OP operation)
{
	m_arcBall.OnMouseMove(glm::ivec2(x, y), operation);
}

void CGLRenderer::DestroyScene(CDC* pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	wglMakeCurrent(NULL, NULL);
	if (m_hrc)
	{
		wglDeleteContext(m_hrc);
		m_hrc = NULL;
	}
}