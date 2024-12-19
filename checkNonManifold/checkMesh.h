#ifndef MESHCHECK_H
#define MESHCHECK_H

#ifdef MESHCHECK_EXPORTS
	#define MESHCHECK_API __declspec(dllexport)
#else
	#define MESHCHECK_API __declspec(dllimport)
#endif
#include "GeoData/MeshData.h"
#include <boost/smart_ptr.hpp>
namespace sn3DCore
{
	class sn3DMeshData;
}
class MESHCHECK_API MeshCheck
{
public:
	MeshCheck(boost::shared_ptr<sn3DCore::sn3DMeshData> mesh);
	MeshCheck();
	~MeshCheck();

public:
	void setMesh(boost::shared_ptr<sn3DCore::sn3DMeshData> mesh);
	boost::shared_ptr<sn3DCore::sn3DMeshData> getMesh();
	bool checkNonManifold();
	void removeNonManifold();
	void repairNonManifold();
	void removeDegenerateFaces();
	boost::shared_ptr<sn3DCore::sn3DMeshData> getMaxConnectedComponent();
	std::vector<boost::shared_ptr<sn3DCore::sn3DMeshData>> getAllConnectedComponent();
private:
	class PImpl;
	boost::shared_ptr<PImpl> impl_;
};
#endif