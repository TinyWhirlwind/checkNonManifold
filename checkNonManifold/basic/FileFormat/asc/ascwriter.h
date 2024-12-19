#ifndef _ASCWRITER_
#define _ASCWRITER_


/*  ASC ÎÄ¼þÐ´ÈëÆ÷
  
*/

#include <string>
#include "../BaseWriter.h"

namespace sn3DCore
{
class ASCWriter : public BaseWriter
{
public:
  
  bool write(const std::string&, BaseExporter&);
  
  bool write(const std::string&, BaseExporter&, float *);

  std::wstring get_description() const { return L"pure points"; }
  
  std::wstring get_extensions()  const { return L"asc"; }

  ASCWriter();

};
}
#endif

