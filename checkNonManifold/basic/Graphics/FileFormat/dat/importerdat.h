#ifndef _IMPORTERDat_
#define _IMPORTERDat_

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include<string>
using namespace std;
#include "../BaseReader.h"
#include "../../../Common/Point2.h"
#include "../../../Common/Point3.h"


// Implementation of the OBJ format reader.

namespace sn3DCore
{

class ImporterDat : public BaseReader
{
public:

  ImporterDat();

  virtual ~ImporterDat() { }

  std::string get_description() const { return "Data/Wavefront"; }

  std::string get_extensions()  const { return "dat"; }

  bool read(std::string& _filename,BaseImporter * & _bi);
  
  bool read(std::string& _filename, BaseImporter& _bi){return true;}


private:

  bool read(FILE * fp, BaseImporter& _bi);

  std::string path_;

};
}


#endif

