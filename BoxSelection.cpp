#include "stdafx.h"
#include "BoxSelection.h"
#include "glm/gtc/type_ptr.hpp"

//start is the lowest/left/bottom position in 3D
//end is the highest/right/up position in 3D
CBoxSelection::CBoxSelection()
{
	m_fWidth = m_fHeight = m_fDepth = 0.10f;
	m_start = glm::vec3(0);
	m_end = glm::vec3(m_start.x + m_fWidth, m_start.y + m_fHeight, m_start.z + m_fDepth);
}

CBoxSelection::~CBoxSelection()
{
}

void CBoxSelection::Draw()
{
	glPushMatrix();
	glPushAttrib(GL_POLYGON_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth(2.0);
	glLoadIdentity();
	glTranslatef(-0.2, -0.2, -((m_end.z + m_start.z)/2));
	glBegin(GL_QUADS);
		glVertex3f(m_start.x, m_start.y, m_start.z);
		glVertex3f(m_start.x + m_fWidth, m_start.y, m_start.z);
		glVertex3f(m_start.x + m_fWidth, m_start.y + m_fHeight, m_start.z);
		glVertex3f(m_start.x, m_start.y + m_fHeight, m_start.z);

		glVertex3f(m_start.x, m_start.y, m_start.z + m_fDepth);
		glVertex3f(m_start.x + m_fWidth, m_start.y, m_start.z + m_fDepth);
		glVertex3f(m_start.x + m_fWidth, m_start.y + m_fHeight, m_start.z + m_fDepth);
		glVertex3f(m_start.x, m_start.y + m_fHeight, m_start.z + m_fDepth);
	glEnd();
	glBegin(GL_LINES);
		glVertex3f(m_start.x, m_start.y, m_start.z);
		glVertex3f(m_start.x, m_start.y, m_start.z + m_fDepth);

		glVertex3f(m_start.x, m_start.y + m_fHeight, m_start.z);
		glVertex3f(m_start.x, m_start.y + m_fHeight, m_start.z + m_fDepth);

		glVertex3f(m_start.x + m_fWidth, m_start.y, m_start.z);
		glVertex3f(m_start.x + m_fWidth, m_start.y, m_start.z + m_fDepth);

		glVertex3f(m_start.x + m_fWidth, m_start.y + m_fHeight, m_start.z);
		glVertex3f(m_start.x + m_fWidth, m_start.y + m_fHeight, m_start.z + m_fDepth);
	glEnd();

	glPopAttrib();
	glPopMatrix();
}
