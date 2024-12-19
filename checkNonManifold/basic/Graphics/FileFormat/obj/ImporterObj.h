#ifndef _IMPORTEROBJ_
#define _IMPORTEROBJ_

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <string>
using namespace std;

#include "Common/Point2.h"
#include "Common/Point3.h"
#include "FileFormat/BaseReader.h"

// Implementation of the OBJ format reader.

namespace sn3DCore
{

class ImporterObj : public BaseReader
{
public:

  ImporterObj();

  virtual ~ImporterObj() { }

  std::wstring get_description() const { return L"Alias/Wavefront"; }

  std::wstring get_extensions()  const { return L"obj"; }

  bool read(const std::string& _filename,BaseImporter * & _bi);
  
  bool read(const std::string& _filename, BaseImporter& _bi);

private:

  class Material
  {
  public:

    Material() { cleanup(); }

    void cleanup()
    {
      Kd_is_set_ = false;
      Ka_is_set_ = false;
      Ks_is_set_ = false;
      Tr_is_set_ = false;
    }

    bool is_valid(void) const 
    { return Kd_is_set_ || Ka_is_set_ || Ks_is_set_ || Tr_is_set_; }

    bool has_Kd(void) { return Kd_is_set_; }
    bool has_Ka(void) { return Ka_is_set_; }
    bool has_Ks(void) { return Ks_is_set_; }
    bool has_Tr(void) { return Tr_is_set_; }
      
    void set_Kd( float r, float g, float b ) 
    { Kd_=Point3f(r,g,b); Kd_is_set_=true; }

    void set_Ka( float r, float g, float b ) 
    { Ka_=Point3f(r,g,b); Ka_is_set_=true; }

    void set_Ks( float r, float g, float b ) 
    { Ks_=Point3f(r,g,b); Ks_is_set_=true; }
    
    void set_Tr( float t )
    { Tr_=t;            Tr_is_set_=true; }
  
    const Point3f& Kd( void ) const { return Kd_; }
    const Point3f& Ka( void ) const { return Ka_; }
    const Point3f& Ks( void ) const { return Ks_; }
    float  Tr( void ) const { return Tr_; }
  
	 std::string texture;
  private:

    Point3f Kd_;         bool Kd_is_set_; // diffuse
    Point3f Ka_;         bool Ka_is_set_; // ambient
    Point3f Ks_;         bool Ks_is_set_; // specular
    float Tr_;           bool Tr_is_set_; // transparency
  };

  typedef std::map<std::string, Material> MaterialList;

  MaterialList materials_;

  bool read_material( std::fstream& _in ,string & str);

 

private:

  bool read(FILE * fp, BaseImporter& _bi);

  std::string path_;

};
}


#endif

