#ifndef _IMPORTERASC_
#define _IMPORTERASC_


#include<string>
#include "../BaseReader.h"

namespace sn3DCore
{
class ImporterAsc : public BaseReader
{
public:
  
  std::wstring get_description() const { return L"ASC/points"; }

  std::wstring get_extensions()  const { return L"asc"; }

  bool read(const std::wstring& _filename,BaseImporter * & _bi);
  
  bool read(const std::wstring& _filename, BaseImporter& _bi);

  ImporterAsc();

  virtual ~ImporterAsc() { }

};

}
#endif