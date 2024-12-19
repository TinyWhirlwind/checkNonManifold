#ifndef _DATWriter_
#define _DATWriter_

/*   DAT 文件写入器
   
   支持写入二进制STL文件 */

#include <string>
#include "../BaseWriter.h"


namespace sn3DCore
{
class DATWriter : public BaseWriter
{
public:
    
  bool write(const std::string&, BaseExporter&);

  bool write(const std::string& filename, BaseExporter& _be,float * mat);

  std::string get_description() const { return "DAT Format"; }
  
  std::string get_extensions()  const { return "dat"; }

  DATWriter();
  
};
}
#endif

