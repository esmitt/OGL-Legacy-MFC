#pragma once

#include "Vector4Df.h"
#include "Matrix4x4f.h"
#include <vector>

using namespace std;

//Class which represents the box into the scene
class CBBox
{
public:
  // extreme points in its own box coordinates system
  CVector4Df m_p0;
  CVector4Df m_p1;

  CVector4Df m_scale;
  CVector4Df m_translate;
  CVector4Df m_qRotate;
  
  // own model view matriz, from object to eye space 
  CMatrix4x4f m_model;

  // from eye space to box space. 
  CMatrix4x4f m_invModel;

  CBBox(void);
  ~CBBox(void);

  // creates a box
  void Create(int x0, int y0, int x1, int y1, CMatrix4x4f &modelView, CMatrix4x4f &projection, vector<CVector4Df> &points, int viewportWidth, int viewportHeight);

  void Draw();

  void Select(CMatrix4x4f &modelView, std::vector<CVector4Df> &points, std::vector<bool> &selected);
  void MoveXY(int currX, int currY, int g_width, int g_height, CMatrix4x4f &projection);
  void MoveZ(int mouseDisplacementInY);
private:
  void DrawBox(float x0, float y0, float z0, float x1, float y1, float z1);
  void BuildModel();
  void BuildDefaultBox();
};

