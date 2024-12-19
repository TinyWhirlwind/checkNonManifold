#ifndef _IMPORTER3DS_
#define _IMPORTER3DS_

#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include "../BaseReader.h"
#include "../../../Common/Point2.h"
#include "../../../Common/Point3.h"
#include"C3DStudio.h"


namespace sn3DCore
{

class Importer3ds : public BaseReader
{
public:

  bool read(std::wstring& _filename,BaseImporter * & _bi);

  bool read(std::wstring& _filename, BaseImporter& _bi);

  std::wstring get_description() const { return L"3ds";}

  std::wstring get_extensions()  const { return L"3ds"; }

public:

  Importer3ds();

  virtual ~Importer3ds(){}

private:

  C3DStudio m_reader;

  std::string path_;

};
}
#endif

