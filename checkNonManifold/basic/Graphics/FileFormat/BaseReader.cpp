#include "stdafx.h"
#include "BaseReader.h"
#include <string>
#include <iterator>
#include <cctype>

namespace sn3DCore
{
	bool BaseReader::can_u_read(const std::wstring& _filename) const 
	{
		// get file extension
		std::wstring extension;
		std::wstring::size_type pos(_filename.rfind(L"."));

		if (pos != std::wstring::npos)
		{ 
			extension = _filename.substr(pos+1, _filename.length()-pos-1);

			std::wstring::iterator iter = extension.begin();
			std::wstring::iterator iterEnd = extension.end();

			for(;iter!=iterEnd;iter++)
			{
				(*iter) = tolower(*iter);//convert to low case
			}
		}

		// locate extension in extension string
		return (get_extensions().find(extension) != std::wstring::npos);
	}

	bool BaseReader::check_extension(const std::wstring& _fname, const std::wstring& _ext) const
	{
		std::wstring cmpExt(_ext);

		std::wstring::iterator iter1 = cmpExt.begin();
		std::wstring::iterator iterEnd1 = cmpExt.end();

		for(;iter1!=iterEnd1;iter1++)
		{
			tolower(*iter1);//convert to low case
		}

		std::wstring::size_type pos(_fname.rfind(L"."));

		if (pos != std::wstring::npos && !_ext.empty() )
		{ 
			std::wstring ext;

			// extension without dot
			ext = _fname.substr(pos+1, _fname.length()-pos-1);

			std::wstring::iterator iter2 = ext.begin();
			std::wstring::iterator iterEnd2 = ext.end();

			for(;iter2!=iterEnd2;iter2++)
			{
				tolower(*iter2);//convert to low case
			}

			return ext == cmpExt;
		}
		return false;  
	}
}