#ifndef _IOTRIMESH_IO_PLY
#define _IOTRIMESH_IO_PLY


#include "plylib.h"
  
/** Additional data needed or useful for parsing a ply mesh.
This class can be passed to the ImporterPLY::Open() function for 
- retrieving additional per-vertex per-face data
- specifying a callback for long ply parsing
- knowing what data is  contained in a ply file
*/

class PlyInfo
{
public:
  typedef ply::PropDescriptor PropDescriptor ;

  PlyInfo()
  {
    status=0;
    mask=0;
 
    vdn=fdn=0;

    VertexData=FaceData=0;
  }

  /// Store the error codes enconutered when parsing a ply
  int status;

  /// It returns a bit mask describing the field present in the ply file
  int mask;  

  
  /// the number of per-vertex descriptor (usually 0)
  int vdn;
  /// The additional vertex descriptor that a user can specify to load additional per-vertex non-standard data stored in a ply
  PropDescriptor *VertexData;
  /// the number of per-face descriptor (usually 0)
  int fdn;
  
  /// The additional vertex descriptor that a user can specify to load additional per-face non-standard data stored in a ply
  PropDescriptor *FaceData;

  /// a string containing the current ply header. Useful for showing it to the user.
  std::string header;

enum Error
{
		// Funzioni superiori
    E_NO_VERTEX       = ply::E_MAXPLYERRORS+1,			// 14
	E_NO_FACE         = ply::E_MAXPLYERRORS+2,				// 15
	E_SHORTFILE       = ply::E_MAXPLYERRORS+3,			// 16
	E_NO_3VERTINFACE  = ply::E_MAXPLYERRORS+4,		// 17
	E_BAD_VERT_INDEX  = ply::E_MAXPLYERRORS+5,		// 18
	E_NO_6TCOORD      = ply::E_MAXPLYERRORS+6,			// 19
	E_DIFFER_COLORS   = ply::E_MAXPLYERRORS+7,	
  E_MAXPLYINFOERRORS= ply::E_MAXPLYERRORS+8// 20
};

}; // end class

#endif
