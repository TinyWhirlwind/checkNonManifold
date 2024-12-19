#ifndef MESH_IO_FILTER_H
#define MESH_IO_FILTER_H

#include "boost/smart_ptr.hpp"

#include <QtCore/QString>

namespace sn3DCore
{
	class sn3DMeshData;
}

/* Mesh二进制格式文件读写 */
class MeshIOFilter
{
public:
	MeshIOFilter();
	~MeshIOFilter()	{}

	bool Read(const QString& filename, sn3DCore::sn3DMeshData* m);
	bool Write(const QString& filename, sn3DCore::sn3DMeshData* m);

	boost::shared_ptr<sn3DCore::sn3DMeshData> Read(const QString& filename);
	bool Write(const QString& filename, boost::shared_ptr<sn3DCore::sn3DMeshData> m);
	
private:
	class PImpl;
	boost::shared_ptr<PImpl> impl;
};

#endif