#include "MeshIOFilter.h"

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QDataStream>
#include <QtCore/QByteArray>
#include <GeoData/MeshData.h>
#include <GeoData/MeshTopology.h>

using namespace sn3DCore;
class MeshIOFilter::PImpl
{
public:
	PImpl()		{}
	~PImpl()	{}

	QString getPath(const QString& filename);

	void toStringP(Point3f& p, QString& strP);
	void toStringTri(Face* f, QString& strTri);

	void toPointP(const QString& str, Point3f& p);
	void toTri(const QString& str, Point3i& tri);
};

QString MeshIOFilter::PImpl::getPath(const QString& filename)
{
	QString str = QDir::toNativeSeparators(filename);
	
	int index = str.lastIndexOf("\\");
	if (index != -1)
	{
		str = str.left(index);
		return str;
	}

	return "";
}

void MeshIOFilter::PImpl::toStringP(Point3f& p, QString& strP)
{
	strP.sprintf("%f %f %f", p[0],p[1],p[2]);
}

void MeshIOFilter::PImpl::toStringTri(Face* f, QString& strTri)
{
	strTri.sprintf("%d %d %d", f->V(0)->Index(), f->V(1)->Index(),f->V(2)->Index());
}

void MeshIOFilter::PImpl::toPointP(const QString& str, Point3f& p)
{
	QStringList strList = str.split(" ");
	for (int i = 0; i < strList.size(); ++i)
	{
		if (!strList[i].isEmpty() && i < 3)
			p[i] = strList[i].toFloat();
	}
}

void MeshIOFilter::PImpl::toTri(const QString& str, Point3i& tri)
{
	QStringList strList = str.split(" ");
	for(int i=0; i<strList.size(); ++i)
	{
		if (!strList[i].isEmpty() && i < 3)
			tri[i] = strList[i].toInt();
	}
}

MeshIOFilter::MeshIOFilter()
{
	impl.reset(new PImpl);
}

bool MeshIOFilter::Read(const QString& filename, sn3DCore::sn3DMeshData* m)
{
	if (!QFileInfo::exists(filename))
	{
		qDebug("ycx file not exist. %s", filename.toStdString().c_str());
		return false;
	}

	QFile file(filename);
	if (!file.open(QFile::ReadOnly))
	{
		qDebug("ycx open file fialure. %s", filename.toStdString().c_str());
		return false;
	}

	QDataStream in(&file);

	////// 读取顶点数据 //////
	int vn;
	in >> vn;
	m->m_vertices.reserve(vn);
	
	QByteArray strP;
	Point3f p;
	for (int i = 0; i < vn; ++i)
	{
		in >> strP;
		impl->toPointP(QString(strP), p);
		m->AddVertex(p);
	}

	/////// 读取面片数据 ///////
	int fn;
	in >> fn;
	m->m_faces.reserve(fn);

	QByteArray strTri;
	Point3i tri;
	for (int i = 0; i < fn; ++i)
	{
		in >> strTri;
		impl->toTri(QString(strTri), tri);	
		m->AddFace(tri[0], tri[1], tri[2]);
	}

	// mesh dirty
	m->UpdateNormal();
	Topology::VertexFace(*m);
	Topology::FaceFace(*m);

	file.close();
	return true;
}

boost::shared_ptr<sn3DCore::sn3DMeshData> MeshIOFilter::Read(const QString& filename)
{
	boost::shared_ptr<sn3DCore::sn3DMeshData> m(new sn3DCore::sn3DMeshData);
	if (Read(filename, m.get()))
		return m;
	
	return 0;
}

bool MeshIOFilter::Write(const QString& filename, sn3DCore::sn3DMeshData* m)
{
	QString path = impl->getPath(filename);
	QDir dir(path);
	if (!dir.exists())
	{	
		//路径不存在，则创建
		dir.mkpath(path);
		qDebug("ycx mkpath: %s. %s", path.toStdString().c_str(), __FUNCTION__);
	}

	// 打开文件
	QFile file(filename);
	if (!file.open(QFile::WriteOnly))
	{
		qDebug("ycx open file failure. file: %s. %s", filename.toStdString().c_str(), __FUNCTION__);
		return false;
	}

	QDataStream out(&file);
	int vn = m->vn;
	int fn = m->fn;

	/////// 写顶点数据 ////////
	out << vn ;
	QString p;
	for (int i = 0; i < vn; ++i)
	{
		impl->toStringP(m->V(i)->P(), p);
		out << QByteArray(p.toStdString().c_str());
	}

	/////// 写面片数据 /////////
	out << fn;
	QString tri;
	for (int i = 0; i < fn; ++i)
	{
		impl->toStringTri(m->F(i), tri);
		out << QByteArray(tri.toStdString().c_str());
	}
	file.close();
	return true;
}

bool MeshIOFilter::Write(const QString& filename, boost::shared_ptr<sn3DCore::sn3DMeshData> m)
{
	return Write(filename, m.get());
}