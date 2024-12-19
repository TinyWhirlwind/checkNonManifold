#ifndef _BASE_READER_
#define _BASE_READER_
#include <iostream>
#include <string>
#include "BaseImporter.h"

namespace sn3DCore
{
	class BaseReader
	{
	public:

		// Reads a mesh given by a filename. 
		virtual bool read(const std::wstring& _filename,BaseImporter& _bi) = 0;

		virtual bool read(const std::wstring& _filename,BaseImporter * & _bi){return false;}

		// Returns true if reader can parse _filename (checks extension)
		virtual bool can_u_read(const std::wstring& _filename) const;

		// Returns a brief description of the file type that can be parsed.
		virtual std::wstring get_description() const = 0;

		// Returns a string with the accepted file extensions separated by a whitespace and in small caps.
		virtual std::wstring get_extensions() const = 0;

		// Return magic bits used to determine file format
		virtual std::wstring get_magic() const { return std::wstring(L""); }

	protected:
		// case insensitive search for _ext in _fname.
		bool check_extension(const std::wstring& _fname,const std::wstring& _ext) const;
	};
}
#endif

