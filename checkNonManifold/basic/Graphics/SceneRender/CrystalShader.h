#ifndef _CRYSTALSHADER_
#define _CRYSTALSHADER_

#include <string>
#include <gl/glew.h>

#include "Common/Point3.h"
#include "boost/smart_ptr.hpp"

namespace sn3DCore
{
	class sn3DMeshData;
}
class CrystalShader
{
public:
	void RenderCrystal(Point3f minp,Point3f maxp);

	void InitShader();

	void BeginShader(int w,int h);

	void EndShader();

public:
	CrystalShader();
	~CrystalShader()	{}

public:
	boost::shared_ptr<sn3DCore::sn3DMeshData> m_data;

	GLuint m_crystalList;
	bool m_init;
	GLuint m_texture;
	GLuint m_crystalVS,m_crystalFS,m_crystalProg;
	std::string  m_vs;
	std::string  m_fs;
};
#endif