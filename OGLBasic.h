#pragma once
typedef unsigned char MOUSE_OP;
const MOUSE_OP ROTATE = 0;
const MOUSE_OP TRANSLATE = 1;
const MOUSE_OP SCALE = 2;
class COGLBasic
{
public:
	static COGLBasic* instance;
	static COGLBasic* getInstance()
	{
		if (!instance)
			instance = new COGLBasic();
		return instance;
	}
	const float fNCP = 0.1f;
	const float fFCP = 100.f;
	const float fAngle = 45.f;
private:
	COGLBasic() {};                   // Constructor? (the {} brackets) are needed here.
};
