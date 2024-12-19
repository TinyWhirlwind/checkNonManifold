#ifndef IOX3D_H
#define IOX3D_H

#include"../../SceneModel/sn3dmeshmodel.h"
class IoX3DPlugin
{
public:
	
	bool open(const std::string &formatName, const std::string &fileName, sn3DMeshModel &m, int& mask);
	bool save(const std::string &formatName, const std::string &fileName, sn3DMeshModel &m, const int mask);
};

#endif
