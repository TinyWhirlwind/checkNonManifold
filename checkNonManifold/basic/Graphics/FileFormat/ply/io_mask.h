#ifndef _IOTRIMESH_IO_MASK
#define _IOTRIMESH_IO_MASK

  
class Mask
{
public:

  /*
  Bitmask for specifying what data has to be loaded or saved or it is present in a given plyfile;
*/

enum {
	IOM_NONE         = 0x0000,

  IOM_VERTCOORD    = 0x0001,
	IOM_VERTFLAGS    = 0x0002, 
	IOM_VERTCOLOR    = 0x0004,
	IOM_VERTQUALITY  = 0x0008,
	IOM_VERTNORMAL   = 0x0010,
	IOM_VERTTEXCOORD = 0x0020,

	IOM_FACEINDEX    = 0x0040,
	IOM_FACEFLAGS    = 0x0080,
	IOM_FACECOLOR    = 0x0100,
	IOM_FACEQUALITY  = 0x0200,
	IOM_FACENORMAL   = 0x0400,
	IOM_WEDGCOLOR    = 0x0800,
	IOM_WEDGTEXCOORD = 0x1000,
	IOM_WEDGTEXMULTI = 0x2000, // Se ha anche l'indice di texture esplicito
	IOM_WEDGNORMAL   = 0x4000,

	IOM_CAMERA       = 0x8000,

	IOM_FLAGS        = IOM_VERTFLAGS + IOM_FACEFLAGS,

	IOM_ALL          = 0xFFFF
};
//
//
//static void IOMask2String( int mask, char str[] )
//{
//	str[0] = 0;
//
//	strcat(str,"V:");
//	if( mask & IOM_VERTFLAGS    ) strcat(str,"flag,");
//	if( mask & IOM_VERTCOLOR    ) strcat(str,"color,");
//	if( mask & IOM_VERTQUALITY  ) strcat(str,"quality,");
//	if( mask & IOM_VERTTEXCOORD ) strcat(str,"texcoord,");
//	if( mask & IOM_VERTNORMAL ) strcat(str,"normal,");
//
//	strcat(str," F:");
//	if( mask & IOM_FACEFLAGS    ) strcat(str,"mask,");
//	if( mask & IOM_FACECOLOR    ) strcat(str,"color,");
//	if( mask & IOM_FACEQUALITY  ) strcat(str,"quality,");
//	if( mask & IOM_FACENORMAL   ) strcat(str,"normal,");
//
//	strcat(str," W:");
//	if( mask & IOM_WEDGCOLOR    ) strcat(str,"color,");
//	if( mask & IOM_WEDGTEXCOORD ) strcat(str,"texcoord,");
//	if( mask & IOM_WEDGNORMAL  ) strcat(str,"normal,");
//
//	if( mask & IOM_CAMERA ) strcat(str," camera");
//}
template <class MeshType> 
static void ClampMask(MeshType &m, int &mask)
{
  if( (mask & IOM_FACECOLOR)    && !HasPerFaceColor(m) ) mask = mask & (~IOM_FACECOLOR);
  if( (mask & IOM_WEDGTEXCOORD) && !HasPerWedgeTexCoord(m) ) mask = mask & (~IOM_WEDGTEXCOORD);
  if( (mask & IOM_WEDGNORMAL) && !m.HasPerWedgeNormal() ) mask = mask & (~IOM_WEDGNORMAL);
  if( (mask & IOM_VERTCOLOR) && !m.HasPerVertexColor() ) mask = mask & (~IOM_VERTCOLOR);
}

}; // end class


#endif
