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
	//int attribs[] = { WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
	//	WGL_CONTEXT_MINOR_VERSION_ARB, 1,
	//	WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
	//	0, 0 };
	//int attribs[] =
	//{
	//	WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
	//	WGL_CONTEXT_MINOR_VERSION_ARB, 2,
	//	WGL_CONTEXT_PROFILE_MASK_ARB,
	//	WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
	//	WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
	//	0
	//};
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
	//glEnable(GL_TEXTURE_2D);
	//glEnable(GL_CULL_FACE);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//to draw the points
	glPolygonOffset(1, 1);
	glEnable(GL_POLYGON_OFFSET_FILL);
	//glPointSize(1.5f);

	glClearColor(0.15f, 0.15f, 0.15f, 1.f);
	if (!m_model.load(strFilename.c_str()))
	{
		AfxMessageBox(L"Problems Loading the file");
	}
	else return;
	m_modelViewMatrix = glm::mat4x4(1);
}

void CGLRenderer::Reshape(CDC* pDC, int w, int h)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	if (h == 0) h = 1;
	float ratio = w / float(h);
	glViewport(0, 0, w, h);
	m_projMatrix = glm::perspective(COGLBasic::getInstance().fAngle, ratio, COGLBasic::getInstance().fNCP, COGLBasic::getInstance().fFCP);
	m_arcBall.Resize(w, h);

	int g_Width = w;
	int g_Height = h;
	glViewport(0, 0, g_Width, g_Height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (float)g_Width / g_Height, COGLBasic::getInstance().fNCP, COGLBasic::getInstance().fFCP);
	//glOrtho(-1, +1, -1, 1, -1, 1);
	glMatrixMode(GL_MODELVIEW);

}

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
		//red color
		glColor3f(0.05,0.05,0.2);
		glVertex2f(-1.0, -1.0);
		glVertex2f(1.0, -1.0);
		//blue color
		glColor3f(0.3,0.3,0.4);
		glVertex2f(1.0, 1.0);
		glVertex2f(-1.0, 1.0);
	glEnd();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_COLOR_MATERIAL); //disable color influence
	glPopAttrib();
	glLoadIdentity();
}

void CGLRenderer::DrawScene(CDC* pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	DrawGradientBackground();	

	//glClearColor(0.15f, 0.15f, 0.15f, 1.f);

	glm::vec3 center = m_model.getCenter();
	float scale = 1.f / m_model.getDiagonal();
	glTranslatef(0, 0, -0.5);
	glMultMatrixf(glm::value_ptr(m_arcBall.GetTransformation()));
	glScalef(scale, scale, scale);
	glTranslatef(-center.x, -center.y, -center.z);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	m_model.drawObject();
	glDisable(GL_TEXTURE_2D);
	glColor3f(0.8, 0.8, 0.8);
	m_model.drawPoints();
	//glEnable(GL_TEXTURE_2D);
	assert(glGetError() == GL_NO_ERROR);
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
	m_model.deleteBuffers();
	wglMakeCurrent(NULL, NULL);
	if (m_hrc)
	{
		wglDeleteContext(m_hrc);
		m_hrc = NULL;
	}
}