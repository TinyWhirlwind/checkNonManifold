#include "stdafx.h"
#include "sn3DIOManager.h"

#include "stl/ImporterStl.h"
#include "ply/importerply.h"
#include "stl/STLWriter.h"
#include "obj/ObjWriter.h"
#include "MeshImporter.h"
#include "MeshExporter.h"
#include "PointsExporter.h"

#include <QtCore/QFileInfo>
#include <QtCore/QDir>

using namespace sn3DCore;

sn3DIOManager& sn3DIOManager::instance()
{
    static sn3DIOManager inst;
    return inst;
}
sn3DIOManager::sn3DIOManager()
{	
    ///////////////////////    初始化读入器    //////////////////////////
    BaseReader * newReader;

    newReader = new ImporterStl;  // 读入stl文件
    register_module(newReader);

    newReader = new ImporterPly;
    register_module(newReader);

    BaseWriter * newWriter;

    newWriter = new STLWriter;    // 写入stl文件
    register_module(newWriter);

    newWriter = new OBJWriter;    // 写入obj文件
    register_module(newWriter);
}

sn3DIOManager::~sn3DIOManager()
{
    std::set<BaseReader*>::iterator itr=reader_modules_.begin();
    for(; itr!=reader_modules_.end(); itr++)
    {
        delete *itr;
    }
    std::set<BaseWriter*>::iterator itw = writer_modules_.begin();
    for (; itw!=writer_modules_.end(); itw++)
    {
        delete *itw;
    }
}
bool sn3DIOManager::Read(std::wstring& _filename,sn3DMeshModel * obj, bool isRemoveRedundant /*= true*/)
{
    MeshImporter importer(obj);

    if (isRemoveRedundant)
        importer.SetRedundant();
    else
        importer.ClearRedundant();

    if(read(_filename,importer))
        return true;
    else
    {
        return false;
    }
}

bool sn3DIOManager::Write(std::wstring& _filename,sn3DMeshModel * obj)
{
    MeshExporter  exporter(obj);

    if(write(_filename,exporter))
        return true;
    else
    {
        return false;
    }
}
bool sn3DIOManager::Read(std::wstring & _filename, SceneObject * obj, bool isRemoveRedundant /*= true*/)
{
    MeshImporter importer(((sn3DMeshModel*)obj)->GetMeshData().get());

    if (isRemoveRedundant)
        importer.SetRedundant();
    else
        importer.ClearRedundant();

    if (!read(_filename, importer))
    {
        return false;
    }

    return true;
}

bool sn3DIOManager::Write(std::wstring& _filename, SceneObject* obj)
{
    switch (obj->GetType())
    {
    case SceneObject::TYPE_MESH:
        Write(_filename, (sn3DMeshModel *)obj);
        break;

    case SceneObject::TYPE_POINTS:
        Write(_filename, (sn3DPointsModel*)obj);
        break;

    default:
        return false;
        break;
    }
    return true;
}

bool sn3DIOManager::Write(std::wstring& _filename, boost::shared_ptr<SceneObject> obj)
{
    switch (obj->GetType())
    {
    case SceneObject::TYPE_MESH:
        Write(_filename, (sn3DMeshModel *)obj.get());
        break;

    case SceneObject::TYPE_POINTS:
        Write(_filename, (sn3DPointsModel*)obj.get());
        break;

    default:
        return false;
        break;
    }
    return true;
}

bool sn3DIOManager::Read(const QString& _filename, sn3DMeshData* obj, bool isRemoveRedundant /*= true*/)
{
    MeshImporter importer(obj);

    if (isRemoveRedundant)
        importer.SetRedundant();
    else
        importer.ClearRedundant();

    if (read(_filename.toStdWString(), importer))
        return true;
    else
    {
        return false;
    }
}

bool sn3DIOManager::Write(const QString& _filename, sn3DMeshData* obj)
{
    // 判断文件路径是否存在， 否则创建路径
    QFileInfo fileInfo(_filename);
    QDir dir = fileInfo.dir();
    if (!dir.exists())
    {
        dir.mkpath(dir.path());
        qDebug("ycx mkpath: %s. %s", dir.path().toStdString().c_str(), __FUNCTION__);
    }

    MeshExporter  exporter(obj);
    if (write(_filename.toStdWString(), exporter))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool sn3DIOManager::read(const std::wstring& _filename, BaseImporter& _bi)
{      
    std::set<BaseReader*>::const_iterator it     =  reader_modules_.begin();
    std::set<BaseReader*>::const_iterator it_end =  reader_modules_.end();

    // Try all registered modules
    for(; it != it_end; ++it)
        if((*it)->can_u_read(_filename))
        {
            bool ok = (*it)->read(_filename, _bi);//read data
            return ok;
        }
    return false;
}

bool sn3DIOManager::read(const std::wstring& _filename, BaseImporter * & _bi)
{      
    std::set<BaseReader*>::const_iterator it     =  reader_modules_.begin();
    std::set<BaseReader*>::const_iterator it_end =  reader_modules_.end();

    // Try all registered modules
    for(; it != it_end; ++it)
        if((*it)->can_u_read(_filename))
        {
            bool ok = (*it)->read(_filename, _bi);//read data
            return ok;
        }
    return false;
}

bool sn3DIOManager::write(const std::wstring& _filename, BaseExporter& _be)
{
    std::set<BaseWriter*>::const_iterator it     = writer_modules_.begin();
    std::set<BaseWriter*>::const_iterator it_end = writer_modules_.end();

    for(; it != it_end; ++it)
    {
        if ((*it)->can_u_write(_filename))
        {
            return (*it)->write(_filename, _be);
        }
    }
    // All modules failed to save*/
    return false;
}
