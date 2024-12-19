#ifndef _INFOBAR_
#define _INFOBAR_

#include <string>
#include "Common/Color4.h"
#include "SceneRender/SceneGL3D.h"

/* 信息面板绘制 */
class InfoBar
{
public:

  void GLRender(SceneGL3D * painter,int locx,int locy,int w,int h);

  void SetMaxMin(float maxv,int minv);

  void SetColor(Color4f values[]);

  void drawString(SceneGL3D * scene, std::string str);

  void  drawCNString(SceneGL3D * scene,const char* str);

  void  SetInfo(int v,int t);

  InfoBar();

private:

  int m_vNum;
  int m_tNum;

  float m_maxv;

  float m_minv;

  Color4f m_values[9];
};
#endif