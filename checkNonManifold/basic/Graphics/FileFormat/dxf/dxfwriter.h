#ifndef _DXFWRITER_
#define _DXFWRITER_


/*  ASC ÎÄ¼şĞ´ÈëÆ÷
  
*/

#include <string>
#include "../BaseWriter.h"

namespace sn3DCore
{
class DXFWriter : public BaseWriter
{
public:
  
  bool write(const std::string&, BaseExporter&);
  
  std::string get_description() const { return "dxf points"; }
  
  std::string get_extensions()  const { return "dxf"; }

  DXFWriter();

};
}
#endif

