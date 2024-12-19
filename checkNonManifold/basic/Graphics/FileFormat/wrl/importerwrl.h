#ifndef _IMPORTERWRL_
#define _IMPORTERWRL_

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>

#include "../BaseReader.h"
#include "../../../Common/Point2.h"
#include "../../../Common/Point3.h"

#include "Wrl1Reader.h"

namespace sn3DCore
{

class ImporterWrl : public BaseReader
{
public:

 
  std::string get_description() const { return "WRL File"; }

  std::string get_extensions()  const { return "wrl"; }

  bool read(std::string& _filename,BaseImporter * & _bi);
  
  bool read(std::string& _filename, BaseImporter& _bi);

public:

  ImporterWrl();

  virtual ~ImporterWrl() { }



private:

  Wrl1Reader m_wrl1Reader;

  std::string path_;

};
}


#endif

