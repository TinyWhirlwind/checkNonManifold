#ifndef _STLWRITER_
#define _STLWRITER_

/*   STL 文件写入器
   
   支持写入二进制STL文件 */

#include <string>
#include "../BaseWriter.h"


namespace sn3DCore
{
class STLWriter : public BaseWriter
{
public:
    
  bool write(const std::wstring&, BaseExporter&);

  bool STLWriter::write(const std::wstring& filename, BaseExporter& _be,float * mat);

  std::wstring get_description() const { return L"Stereolithography Format"; }
  
  std::wstring get_extensions()  const { return L"stla stlb"; }

  STLWriter();
  
};
}
#endif

