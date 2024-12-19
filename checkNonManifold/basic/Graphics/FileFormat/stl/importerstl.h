#ifndef _IMPORTERSTL_
#define _IMPORTERSTL_

#include <stdio.h>

#include "../BaseReader.h"
#include<windows.h>

namespace sn3DCore
{
class ImporterStl : public BaseReader
{
public:

	std::wstring get_description() const { return L"StereoLitho/STL"; }

	std::wstring get_extensions()  const { return L"stl"; }

	bool read(const std::wstring& _filename,BaseImporter * & _bi);

	bool read(const std::wstring& _filename, BaseImporter& _bi);

	// if it is binary there are 80 char of comment, the number fn of faces and then exactly fn*4*3 bytes.

	enum{STL_LABEL_SIZE=80};

	class STLFacet
	{
	 public:
		Point3f n;
		Point3f v[3];
		//  short attr;
	};

	enum STLError 
	{
		E_NOERROR,				// 0
		// Errori di open
		E_CANTOPEN,				// 1
		E_UNESPECTEDEOF       		        // 2
	};

	static const char *ErrorMsg(int error)
	{
		static const char * stl_error_msg[] =
		{
			"No errors",
			"Can't open file",
			"Premature End of file",
		};

		if(error>2 || error<0) return "Unknown error";
		else return stl_error_msg[error];
	};


	int read(BaseImporter& _bi, const TCHAR * filename);

	int OpenBinary(BaseImporter& _bi, const TCHAR * filename);
	
	int OpenAscii(BaseImporter& _bi, const TCHAR * filename);
}; 
}

#endif
