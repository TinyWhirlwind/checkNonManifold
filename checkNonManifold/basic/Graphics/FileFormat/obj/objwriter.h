#ifndef _OBJWRITER_
#define _OBJWRITER_

/*  Obj ÎÄ¼þÐ´ÈëÆ÷  */

#include "../BaseWriter.h"
namespace sn3DCore
{
class OBJWriter : public BaseWriter
{
public:

   bool write(const std::wstring&, BaseExporter&);
  
   bool write(const std::wstring&, BaseExporter&, float *);

   std::wstring get_description() const { return L"Alias/Wavefront"; }
  
   std::wstring get_extensions()  const { return L"obj"; }

   OBJWriter();

private:

  std::string path_;

  //int getMaterial(OpenMesh::Vec3f _color) const;

  //int getMaterial(OpenMesh::Vec4f _color) const;

  //bool writeMaterial(std::fstream& _out, BaseExporter&, Options) const;

  //bool write(std::fstream& _out, BaseExporter&, Options) const;
};
}
#endif
