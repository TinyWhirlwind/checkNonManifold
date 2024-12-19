#ifndef _SN3D_IO_MANAGER_
#define _SN3D_IO_MANAGER_

#include <set>
#include <QtCore/QString.h>
#include <boost/smart_ptr.hpp>

#include "BaseReader.h"
#include "BaseWriter.h"
#include "BaseImporter.h"
#include "BaseExporter.h"
#include "SceneObject/SceneMeshObj.h"

namespace sn3DCore
{
	class sn3DIOManager// : public Isn3DIO
	{
	public:
		static sn3DIOManager& instance();

	 public:
		bool Read(std::wstring & _filename, SceneObject * obj, bool isRemoveRedundant = true);
		bool Write(std::wstring& _filename, SceneObject* obj);
		bool Read(std::wstring & _filename, sn3DMeshModel * obj, bool isRemoveRedundant = true);
		bool Write(std::wstring & _filename, sn3DMeshModel * obj);
		bool Write(std::wstring & _filename, boost::shared_ptr<SceneObject> obj);		
		bool Read(const QString& _filename, sn3DMeshData* mesh, bool isRemoveRedundant = true);
		bool Write(const QString& _filename, sn3DMeshData* mesh);
		
		sn3DIOManager();
		~sn3DIOManager();
private:
		bool  read(const std::wstring& _filename, BaseImporter * & _bi);
		bool  read(const std::wstring& _filename, BaseImporter & _bi);
		bool  write(const std::wstring& _filename,BaseExporter & _be);
		bool  register_module(BaseReader* _bl)  // 注册读取模块
		{
			reader_modules_.insert(_bl);
			return true;
		}

		bool  register_module(BaseWriter* _bw)  //  注册写入模块
		{
			writer_modules_.insert(_bw);
			return true;
		}

	private:
		BaseWriter * find_writer(const std::wstring & _format);
		std::set<BaseReader*> reader_modules_; // 读入器
		std::set<BaseWriter*> writer_modules_; // 写入器
		std::wstring read_filters_;
		std::wstring write_filters_;
	};
}
#endif