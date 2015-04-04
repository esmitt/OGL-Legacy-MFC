#include "stdafx.h"
#include "BBox.h"

CBBox::CBBox(void)
{
  BuildDefaultBox();
}


void CBBox::BuildDefaultBox()
{
  // unit sphere
  m_p0 = CVector4Df(0.0f, 0.0f, 0.0f, 1.0f);
  m_p1 = CVector4Df(1.0f, 1.0f, 1.0f, 1.0f);

  m_scale = CVector4Df(1.0f,1.0f,1.0f,1.0f);

  // rotate 0 degrees around x
  m_qRotate = CVector4Df(0.0f, 1.0f, 0.0f, 0.0f);
  m_translate = CVector4Df(0.0f, 0.0f, 0.0f, 1.0f);

}


void CBBox::BuildModel()
{
  m_model.SetIdentity();
  m_model.Translate(m_translate[0], m_translate[1], m_translate[2]);
  m_model.Rotate(m_qRotate[0], m_qRotate[1], m_qRotate[2], m_qRotate[3]);
  m_model.Scale(m_scale[0], m_scale[1], m_scale[2]);
  m_model.Translate(-0.5f, -0.5f, -0.5f);

  m_invModel.SetIdentity();
  m_invModel.Translate(0.5f, 0.5f, 0.5f);
  m_invModel.Scale(1.0f/m_scale[0], 1.0f/m_scale[1], 1.0f/m_scale[2]);
  m_invModel.Rotate(-m_qRotate[0], m_qRotate[1], m_qRotate[2], m_qRotate[3]);
  m_invModel.Translate(-m_translate[0], -m_translate[1], -m_translate[2]);
}


CBBox::~CBBox(void)
{
}

// creates a box
void CBBox::Create(int x0, int y0, int x1, int y1, CMatrix4x4f &modelView, CMatrix4x4f &projection, vector<CVector4Df> &points, int viewportWidth, int viewportHeight)
{
  CMatrix4x4f invProjection(projection);
  if (invProjection.Invert() == false)
    return;

  float xmin, ymin, zmin, xmax, ymax, zmax;
  bool found = false;
  BuildDefaultBox();
  if (x1<x0) swap(x0,x1);
  if (y1<y0) swap(y0,y1);

  CMatrix4x4f viewport;
  viewport.Scale(viewportWidth/2.0f, viewportHeight/2.0f, 1.0f);
  viewport.Translate(1.0f, 1.0f, 1.0f);

  CMatrix4x4f invViewport;
  invViewport.Translate(-1.0f, -1.0f, -1.0f);
  invViewport.Scale(2.0f/viewportWidth, 2.0f/viewportHeight, 1.0f);

  // viewport selected area, z=0, in viewport space
	CVector4Df p0((float)x0, (float)y0, 0.0f, 1.0f);
	CVector4Df p1((float)x1, (float)y1, 0.0f, 1.0f);

  // viewport selected area in clipping space, z=-1
  p0 = invViewport * p0;
  p1 = invViewport * p1;


  CMatrix4x4f mvp = projection * modelView;
  for (int i=0; i<(int)points.size(); i++)
  {
    CVector4Df r = mvp * points[i];
    r.Normalize(); // esto es, dividir entre w, y dejar w en 1
    if (-1.0f<=r[0] && r[0] <= 1.0f && -1.0f<=r[1] && r[1] <= 1.0f && -1.0f<=r[2] && r[2] <= 1.0f) // inside the clipping space
    {
      // inside the selected area in clipping space
      if (p0[0] <= r[0] && r[0] <= p1[0] && p0[1] <= r[1] && r[1] <= p1[1])
      {
        if (!found)
        {
          found = true;
          xmin = xmax = r[0];
          ymin = ymax = r[1];
          zmin = zmax = r[2];          
        }
        else
        {
          xmin = __min(xmin, r[0]);
					xmax = __max(xmax, r[0]);
					ymin = __min(ymin, r[1]);
					ymax = __max(ymax, r[1]);
					zmin = __min(zmin, r[2]);
					zmax = __max(zmax, r[2]);
        }
      }
    }
  }

  //float average = (p1[0] - p0[0] + p1[1] - p0[1])/2.0f;
  if (found) // I found at least 1 point inside
  {
    // the box is centered in zmin
    p0[2] = p1[2] = zmin - 0.0001f;
  }
  else
  {
    // the front face is in a front clipping plane plus half box
    p0[2] = p1[2] = -1.0f + 0.01f;
  }

  // clipping space to eye space
  p0 = invProjection * p0; p0.Normalize();
  p1 = invProjection * p1; p1.Normalize();

  // now we are in eye space
  float average = (p1[0] - p0[0] + p1[1] - p0[1])/2.0f;
  p1[2] -= average;

  // now, we have the axis aligned bounding box in eye space... we can determine the scales factors
  // and translates
  m_scale = CVector4Df(p1[0] - p0[0], p1[1] - p0[1], p1[2] - p0[2], 1.0f);
  m_translate = (p0+p1) * 0.5f;    
  BuildModel();
}


void CBBox::DrawBox(float x0, float y0, float z0, float x1, float y1, float z1)
{
  //glEnable(GL_BLEND);
  glBegin(GL_LINES);
    glVertex3f(x0, y0, z0);
    glVertex3f(x0, y0, z1);

    glVertex3f(x0, y0, z1);
    glVertex3f(x0, y1, z1);

    glVertex3f(x0, y1, z1);
    glVertex3f(x0, y1, z0);

    glVertex3f(x0, y1, z0);
    glVertex3f(x0, y0, z0);

    glVertex3f(x0, y0, z0);
    glVertex3f(x1, y0, z0);

    glVertex3f(x1, y0, z0);
    glVertex3f(x1, y0, z1);

    glVertex3f(x1, y0, z1);
    glVertex3f(x1, y1, z1);

    glVertex3f(x1, y1, z1);
    glVertex3f(x1, y1, z0);

    glVertex3f(x1, y1, z0);
    glVertex3f(x1, y0, z0);

    glVertex3f(x0, y1, z0);
    glVertex3f(x1, y1, z0);

    glVertex3f(x0, y1, z1);
    glVertex3f(x1, y1, z1);

    glVertex3f(x0, y0, z1);
    glVertex3f(x1, y0, z1);
  glEnd();
  //glDisable(GL_BLEND);
}

void CBBox::Draw()
{
  glPushMatrix();
	glPushAttrib(GL_POLYGON_BIT);
  glColor4ub(255,255,255,255);
  glMatrixMode(GL_MODELVIEW);

  // uploading into opengl ... tranpose to get it in column major order
  glLoadMatrixf(m_model.Transpose().m_vector); m_model.Transpose();
  DrawBox(m_p0[0], m_p0[1], m_p0[2], m_p1[0], m_p1[1], m_p1[2]); 
	glPopAttrib();
  glPopMatrix();
}

void CBBox::Select(CMatrix4x4f &modelView, std::vector<CVector4Df> &points, std::vector<bool> &selected)
{
  CMatrix4x4f objToBox = m_invModel * modelView;
  for (int i=0; i<(int)points.size(); i++)
  {
    selected[i] = false;
    CVector4Df r = objToBox * points[i];
    r.Normalize(); // esto es, dividir entre w, y dejar w en 1
    if (0.0f<=r[0] && r[0] <= 1.0f && 0.0f<=r[1] && r[1] <= 1.0f && 0.0f<=r[2] && r[2] <= 1.0f) // inside the box
      selected[i] = true;
  }
}

void CBBox::MoveZ(int mouseDisplacementInY)
{
  float d = 0.05f * mouseDisplacementInY * sqrt(m_scale[0]*m_scale[0] + m_scale[1]*m_scale[1] + m_scale[2]*m_scale[2]);
  m_translate[2] += d;
  BuildModel();
}

void CBBox::MoveXY(int currX, int currY, int g_width, int g_height, CMatrix4x4f &projection)
{
	CMatrix4x4f invProjection(projection);
	if (invProjection.Invert() == false)
		return;
	CVector4Df center = CVector4Df(0.5f, 0.5f, 0.5f, 1.0f);
	center = (projection * m_model) * center;
	center.Normalize();
	// now, we have the center´s box in clipping space

	CMatrix4x4f invViewport;
	invViewport.Translate(-1.0f, -1.0f, -1.0f);
	invViewport.Scale(2.0f / g_width, 2.0f / g_height, 1.0f);

	CVector4Df mouseClip = invViewport * CVector4Df((float)currX, (float)currY, 0.0f, 1.0f);
	// now, we have the mouse and the box center in clipping space

	//CVector4Df translation = mouseClip - center;

	// traslation vector in eye space
	//translation = invProjection * translation;

	//m_translate = m_translate + translation;
	//float oldZ = m_translate[2];
	mouseClip[2] = center[2];
	mouseClip.Normalize();
	m_translate = invProjection * mouseClip;
	m_translate.Normalize();
	//m_translate[2] = oldZ;
	BuildModel();
}