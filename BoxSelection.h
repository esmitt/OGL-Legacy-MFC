#pragma once
#include "glm\glm.hpp"

class CBoxSelection
{
private:
	glm::vec3 m_start, m_end;
	float m_fWidth, m_fHeight, m_fDepth;
public:
	CBoxSelection();
	~CBoxSelection();
	void Draw();
};

