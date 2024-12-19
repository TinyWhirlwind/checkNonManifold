#ifndef _IMPORTERDXF_
#define _IMPORTERDXF_

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include "../BaseReader.h"
#include "../../../Common/Point2.h"
#include "../../../Common/Point3.h"

namespace sn3DCore
{

class ImporterDxf : public BaseReader
{
public:

  bool read(std::string& _filename,BaseImporter * & _bi);

  bool read(std::string& _filename, BaseImporter& _bi);
 
  std::string get_description() const { return "DXF File"; }

  std::string get_extensions()  const { return "dxf"; }
  

public:

  ImporterDxf();

  virtual ~ImporterDxf() { }

private:

  std::string path_;

};
}


#endif

