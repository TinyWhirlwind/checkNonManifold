#ifndef _BASE_WRITER_
#define _BASE_WRITER_

/*  BaseWriter 文件写入器抽象类

包含多种文件写入功能

*/

#include <iostream>
#include <string>
#include "BaseExporter.h"

namespace sn3DCore
{
	class BaseWriter
	{
	public:
		virtual ~BaseWriter(){}

		// Returns true if reader can parse _filename (checks extension)
		virtual bool can_u_write(const std::wstring& _filename) const;

		/// Write to file _filename. Data source specified by BaseExporter _be.
		virtual bool write(const std::wstring& _filename,BaseExporter& _be) = 0;

		virtual bool write(const std::wstring& _filename,BaseExporter& _be ,float * mat){return false;}

		/// Return short description of the supported file format.
		virtual std::wstring get_description() const = 0;

		/// Return file format's extension.
		virtual std::wstring get_extensions() const = 0;

		/// Returns expected size of file if binary format is supported else 0.
		virtual size_t binary_size(BaseExporter&) const { return 0; }

		// case insensitive search for _ext in _fname.
		bool check_extension(const std::wstring& _fname,const std::wstring& _ext) const;
	};
}

#endif

