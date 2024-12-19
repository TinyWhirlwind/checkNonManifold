#ifndef _IMPORTEROFF_
#define _IMPORTEROFF_

#include<string>
#include "../BaseReader.h"

namespace sn3DCore
{
class ImporterOFF : public BaseReader
{
public:
  
  std::string get_description() const { return "OFF/mesh"; }

  std::string get_extensions()  const { return "off"; }

  bool read(std::string& _filename,BaseImporter * & _bi);
  
  bool read(std::string& _filename, BaseImporter& _bi);

  ImporterOFF();

  virtual ~ImporterOFF() { }

};

}
#endif